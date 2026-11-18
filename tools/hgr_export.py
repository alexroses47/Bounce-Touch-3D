#!/usr/bin/env python3
"""Export KA3D .hgr scenes for viewing: glTF binary, standalone HTML viewer, PNG render.

Usage:
  hgr_export.py glb  scene.hgr out.glb  [options]
  hgr_export.py html scene.hgr out.html [options]   (three.js viewer, open in a browser)
  hgr_export.py png  scene.hgr out.png  [options]   (software render, no dependencies)

Options:
  --textures DIR   folder with the .ntx files (default: ../resources/textures next to
                   the scene's folder, or the scene's folder)
  --lib DIR        folder searched for `load = file.hgr[,node]` references (repeatable;
                   default: resources/models). Referenced objects are instanced.
  --size WxH       PNG size (default 960x540)
  --view top|camera|iso   PNG camera (default iso); or eye=x,y,z:target=x,y,z[:fov=deg]

Nothing leaves the machine: textures are embedded in the output file and the HTML
viewer only fetches three.js from a CDN.

Coordinate system: KA3D/hgr is Y-up, left-handed (DirectX). The glTF root node
mirrors Z (scale -1) and scales 0.01, so the scene is right-handed, roughly metres.
"""
import base64
import json
import math
import os
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import hgr  # noqa: E402
import ntx  # noqa: E402

ROOT_SCALE = 0.01


# ----------------------------------------------------------------- scene helpers

def mat_mul(a, b):
    """Affine 3x4 * 3x4."""
    return [[sum(a[i][k] * b[k][j] for k in range(3)) + (a[i][3] if j == 3 else 0.0)
             for j in range(4)] for i in range(3)]


def mat_apply(m, v):
    return tuple(m[r][0] * v[0] + m[r][1] * v[1] + m[r][2] * v[2] + m[r][3] for r in range(3))


IDENTITY = [[1.0, 0, 0, 0], [0, 1.0, 0, 0], [0, 0, 1.0, 0]]


class TextureCache:
    def __init__(self, folder):
        self.folder = folder
        self.files = {}
        if folder and os.path.isdir(folder):
            for f in os.listdir(folder):
                if f.lower().endswith(".ntx"):
                    self.files[f[:-4].lower()] = os.path.join(folder, f)
        self.cache = {}

    def get(self, name):
        """name like 'foo.tga' -> (w, h, rgba, info) or None."""
        key = os.path.splitext(os.path.basename(name.replace("\\", "/")))[0].lower()
        if key not in self.cache:
            path = self.files.get(key)
            self.cache[key] = ntx.read_ntx(open(path, "rb").read()) if path else None
        return self.cache[key]


class Library:
    """Resolves `load = file.hgr[,node]` user properties."""

    def __init__(self, folders):
        self.folders = [f for f in folders if f and os.path.isdir(f)]
        self.scenes = {}

    def scene(self, fname):
        if fname not in self.scenes:
            self.scenes[fname] = None
            for d in self.folders:
                p = os.path.join(d, fname)
                if os.path.isfile(p):
                    self.scenes[fname] = hgr.load(p)
                    break
        return self.scenes[fname]


def node_props(scene, name):
    text = scene.user_properties.get(name)
    return hgr.parse_properties(text) if text else []


def prop(scene, name, key):
    for k, v in node_props(scene, name):
        if k.lower() == key:
            return v
    return None


def is_collision_only(scene, node):
    return (prop(scene, node.name, "collisionmesh") or "").lower() == "this"


def flatten(scene, lib, depth=0, base=IDENTITY, subtree=None, sky=False):
    """Yield (scene, node, world 3x4, role) for every node, instancing library loads.

    role: 'visual', 'collision', 'skybox', 'trigger', 'dummy', 'camera', 'light', 'lines', 'node'.
    Nodes named *skybox* or with a `skybox` property (and their subtrees) get 'skybox'.
    """
    children = {}
    for i, n in enumerate(scene.nodes):
        children.setdefault(n.parent, []).append(i)
    if subtree is None:
        roots = children.get(-1, [])
        root_tm = base
    else:
        idx = next((i for i, n in enumerate(scene.nodes) if n.name == subtree), None)
        if idx is None:
            return
        roots = [idx]
        # place the referenced node at the loading dummy: drop its own transform
        root_tm = base

    def walk(i, parent_tm, is_root, sky):
        n = scene.nodes[i]
        tm = parent_tm if (is_root and subtree is not None) else mat_mul(parent_tm, n.transform)
        sky = sky or "skybox" in n.name.lower() or prop(scene, n.name, "skybox") not in (None, "0")
        if sky and n.kind == "mesh":
            role = "skybox"
        elif n.kind == "mesh":
            role = "collision" if is_collision_only(scene, n) else "visual"
        elif n.kind == "dummy":
            role = "trigger" if n.name.startswith("T_") else "dummy"
        else:
            role = n.kind
        yield scene, n, tm, role
        load = prop(scene, n.name, "load")
        if load and lib and depth < 2:
            parts = [p.strip() for p in load.split(",")]
            sub = lib.scene(parts[0])
            if sub is not None:
                yield from flatten(sub, lib, depth + 1, tm, parts[1] if len(parts) > 1 else None, sky)
        for c in children.get(i, []):
            yield from walk(c, tm, False, sky)

    for r in roots:
        yield from walk(r, root_tm, True, sky)


# ----------------------------------------------------------------- glTF export

class GlbBuilder:
    def __init__(self):
        self.bin = bytearray()
        self.gltf = {"asset": {"version": "2.0", "generator": "bounce-port hgr_export.py"},
                     "scene": 0, "scenes": [{"nodes": [0]}], "nodes": [], "meshes": [],
                     "materials": [], "textures": [], "images": [], "samplers": [
                         {"magFilter": 9729, "minFilter": 9987, "wrapS": 10497, "wrapT": 10497}],
                     "accessors": [], "bufferViews": [], "buffers": [],
                     "extensionsUsed": ["KHR_materials_unlit"]}

    def view(self, data, target=None):
        while len(self.bin) % 4:
            self.bin.append(0)
        bv = {"buffer": 0, "byteOffset": len(self.bin), "byteLength": len(data)}
        if target:
            bv["target"] = target
        self.bin += data
        self.gltf["bufferViews"].append(bv)
        return len(self.gltf["bufferViews"]) - 1

    def accessor(self, data, ctype, count, atype, target, mn=None, mx=None):
        acc = {"bufferView": self.view(data, target), "componentType": ctype,
               "count": count, "type": atype}
        if mn is not None:
            acc["min"], acc["max"] = mn, mx
        self.gltf["accessors"].append(acc)
        return len(self.gltf["accessors"]) - 1

    def finish(self):
        self.gltf["buffers"] = [{"byteLength": len(self.bin)}]
        for k in ("textures", "images"):
            if not self.gltf[k]:
                del self.gltf[k]
        js = json.dumps(self.gltf, separators=(",", ":")).encode()
        js += b" " * (-len(js) % 4)
        binp = bytes(self.bin) + b"\0" * (-len(self.bin) % 4)
        total = 12 + 8 + len(js) + 8 + len(binp)
        return (struct.pack("<III", 0x46546C67, 2, total) + struct.pack("<II", len(js), 0x4E4F534A)
                + js + struct.pack("<II", len(binp), 0x004E4942) + binp)


def to_gltf_matrix(m):
    return [m[0][0], m[1][0], m[2][0], 0.0, m[0][1], m[1][1], m[2][1], 0.0,
            m[0][2], m[1][2], m[2][2], 0.0, m[0][3], m[1][3], m[2][3], 1.0]


def build_glb(scene, textures, lib):
    b = GlbBuilder()
    g = b.gltf
    g["nodes"].append({"name": "hgr_root", "matrix": to_gltf_matrix(
        [[ROOT_SCALE, 0, 0, 0], [0, ROOT_SCALE, 0, 0], [0, 0, -ROOT_SCALE, 0]]), "children": []})
    prim_cache, mat_cache, img_cache = {}, {}, {}

    def image(texname):
        if texname not in img_cache:
            t = textures.get(texname)
            if t is None:
                img_cache[texname] = None
            else:
                w, h, rgba, info = t
                alpha = any(rgba[i] != 255 for i in range(3, len(rgba), 4))
                g["images"].append({"bufferView": b.view(ntx.png_bytes(w, h, rgba)),
                                    "mimeType": "image/png", "name": texname})
                g["textures"].append({"source": len(g["images"]) - 1, "sampler": 0})
                img_cache[texname] = (len(g["textures"]) - 1, alpha)
        return img_cache[texname]

    def material(sc, ix):
        key = (id(sc), ix)
        if key not in mat_cache:
            m = sc.materials[ix]
            col = m.vectors.get("DIFFUSEC", [1, 1, 1, 1])
            mat = {"name": m.name or f"mat{ix}", "extensions": {"KHR_materials_unlit": {}},
                   "doubleSided": False, "extras": {"shader": m.shader},
                   "pbrMetallicRoughness": {"metallicFactor": 0, "roughnessFactor": 1}}
            pbr = mat["pbrMetallicRoughness"]
            if "BASEMAP" in m.textures:
                tex = image(sc.textures[m.textures["BASEMAP"]][0])
                if tex:
                    pbr["baseColorTexture"] = {"index": tex[0]}
                    if tex[1]:
                        mat["alphaMode"] = "MASK"
                        mat["alphaCutoff"] = 0.5
            else:
                pbr["baseColorFactor"] = [col[0], col[1], col[2], 1.0]
            g["materials"].append(mat)
            mat_cache[key] = len(g["materials"]) - 1
        return mat_cache[key]

    def gl_prim(sc, pi):
        key = (id(sc), pi)
        if key not in prim_cache:
            p = sc.primitives[pi]
            pos = p.positions()
            mn = [min(v[k] for v in pos) for k in range(3)]
            mx = [max(v[k] for v in pos) for k in range(3)]
            attrs = {"POSITION": b.accessor(struct.pack("<%df" % (3 * len(pos)), *[c for v in pos for c in v]),
                                            5126, len(pos), "VEC3", 34962, mn, mx)}
            uv = p.uvs()
            if uv is not None:
                attrs["TEXCOORD_0"] = b.accessor(struct.pack("<%df" % (2 * len(uv)), *[c for v in uv for c in v]),
                                                 5126, len(uv), "VEC2", 34962)
            ind = b.accessor(struct.pack("<%dH" % len(p.indices), *p.indices), 5123,
                             len(p.indices), "SCALAR", 34963)
            prim_cache[key] = {"attributes": attrs, "indices": ind, "material": material(sc, p.material)}
        return prim_cache[key]

    for sc, n, tm, role in flatten(scene, lib):
        node = {"name": n.name, "matrix": to_gltf_matrix(tm),
                "extras": {"kind": n.kind, "role": role, "props": sc.user_properties.get(n.name, "")}}
        if n.kind == "mesh" and n.extra["primitives"]:
            g["meshes"].append({"name": n.name, "primitives": [gl_prim(sc, pi) for pi in n.extra["primitives"]]})
            node["mesh"] = len(g["meshes"]) - 1
        if n.kind == "dummy":
            node["extras"]["box"] = [n.extra["box_min"], n.extra["box_max"]]
        g["nodes"].append(node)
        g["nodes"][0]["children"].append(len(g["nodes"]) - 1)
    return b.finish()


# ----------------------------------------------------------------- HTML viewer

HTML = r"""<!doctype html>
<html lang="es"><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>HGR viewer</title>
<style>
:root{--bg:#1d2027;--panel:#2a2e37;--text:#e8e8ea;--muted:#9aa0ab;--accent:#ff5a4e}
@media (prefers-color-scheme: light){:root:not([data-theme="dark"]){--bg:#eef0f3;--panel:#ffffff;--text:#1d2027;--muted:#5d6470;--accent:#d63a2f}}
html,body{margin:0;height:100%;background:var(--bg);color:var(--text);font:13px/1.4 system-ui,sans-serif}
#c{position:fixed;inset:0}
#ui{position:fixed;top:12px;left:12px;max-width:min(360px,calc(100vw - 24px));background:var(--panel);border-radius:8px;padding:10px 12px;box-shadow:0 2px 10px #0004}
#ui h1{font-size:14px;margin:0 0 6px}#ui label{display:block;color:var(--muted)}
#info{position:fixed;bottom:12px;left:12px;right:12px;max-width:520px;max-height:40vh;overflow:auto;background:var(--panel);border-radius:8px;padding:10px 12px;white-space:pre-wrap;font:12px/1.4 ui-monospace,monospace;display:none}
#info b{color:var(--accent)}
</style></head><body>
<canvas id="c"></canvas>
<div id="ui"><h1>__TITLE__</h1><div id="stats" style="color:var(--muted)"></div>
<label><input type="checkbox" id="t-visual" checked> Geometría visible</label>
<label><input type="checkbox" id="t-skybox" checked> Cielo (skybox)</label>
<label><input type="checkbox" id="t-collision"> Colisión (collisionmesh)</label>
<label><input type="checkbox" id="t-trigger"> Triggers (T_*)</label>
<label><input type="checkbox" id="t-dummy"> Dummies</label>
<div style="color:var(--muted);margin-top:6px">Arrastra para orbitar, rueda para zoom, clic en un objeto para ver sus propiedades.</div></div>
<div id="info"></div>
<script type="importmap">{"imports":{"three":"https://cdn.jsdelivr.net/npm/three@0.160.0/build/three.module.js","three/addons/":"https://cdn.jsdelivr.net/npm/three@0.160.0/examples/jsm/"}}</script>
<script type="module">
import * as THREE from 'three';
import {OrbitControls} from 'three/addons/controls/OrbitControls.js';
import {GLTFLoader} from 'three/addons/loaders/GLTFLoader.js';
const GLB="__GLB__";
const bin=Uint8Array.from(atob(GLB),c=>c.charCodeAt(0)).buffer;
const canvas=document.getElementById('c');
const renderer=new THREE.WebGLRenderer({canvas,antialias:true});
renderer.setPixelRatio(devicePixelRatio);
const scene=new THREE.Scene();scene.background=new THREE.Color(__BG__);
const camera=new THREE.PerspectiveCamera(60,1,0.1,5000);
const controls=new OrbitControls(camera,canvas);
const groups={visual:[],skybox:[],collision:[],trigger:[],dummy:[]};
const pickables=[];
new GLTFLoader().parse(bin,'',gltf=>{
  const root=gltf.scene;scene.add(root);
  // collect first: the loop adds helper boxes, which must not be visited again
  const all=[];root.traverse(o=>all.push(o));
  all.forEach(o=>{
    const ex=o.userData||{};const role=ex.role;
    if(o.isMesh){
      const sh=(o.material.userData&&o.material.userData.shader)||'';
      if(sh.includes('-mul')){o.material.blending=THREE.MultiplyBlending;o.material.transparent=true;o.material.premultipliedAlpha=true;}
      if(sh.includes('nozwrite'))o.material.depthWrite=false;
      if(o.material.map)o.material.map.colorSpace=THREE.SRGBColorSpace;
    }
    if(ex.kind==='mesh'){
      if(role==='skybox'){o.traverse(m=>{if(m.isMesh){m.renderOrder=-1;m.material.depthWrite=false;m.material.depthTest=false;}});}
      if(role==='collision'){o.traverse(m=>{if(m.isMesh)m.material=new THREE.MeshBasicMaterial({color:0x33c3ff,wireframe:true});});}
      (groups[role]||[]).push(o);pickables.push(o);
    }
    if(ex.kind==='dummy'&&ex.box){
      const [a,b]=ex.box;const size=new THREE.Vector3(b[0]-a[0],b[1]-a[1],b[2]-a[2]);
      if(size.lengthSq()>0){
        const geo=new THREE.BoxGeometry(Math.max(size.x,1),Math.max(size.y,1),Math.max(size.z,1));
        const m=new THREE.Mesh(geo,new THREE.MeshBasicMaterial({color:role==='trigger'?0xffb02e:0x8fe36b,wireframe:true}));
        m.position.set((a[0]+b[0])/2,(a[1]+b[1])/2,(a[2]+b[2])/2);m.userData={...ex,name:o.name};o.add(m);
        groups[role].push(m);pickables.push(m);
      }
    }
  });
  for(const k of Object.keys(groups)){const cb=document.getElementById('t-'+k);const set=()=>groups[k].forEach(o=>o.visible=cb.checked);cb.onchange=set;set();}
  scene.updateMatrixWorld(true);  // the root carries the 0.01 scale / Z mirror
  const box=new THREE.Box3();groups.visual.forEach(o=>box.expandByObject(o));
  if(box.isEmpty())box.setFromObject(root);
  const c=box.getCenter(new THREE.Vector3()),r=box.getSize(new THREE.Vector3()).length()/2||10;
  controls.target.copy(c);camera.position.set(c.x+r*0.6,c.y+r*0.7,c.z+r*0.6);camera.near=r/500;camera.far=r*20;camera.updateProjectionMatrix();
  document.getElementById('stats').textContent=__STATS__;
});
const ray=new THREE.Raycaster();const info=document.getElementById('info');
canvas.addEventListener('click',e=>{
  const p=new THREE.Vector2(e.clientX/innerWidth*2-1,-e.clientY/innerHeight*2+1);ray.setFromCamera(p,camera);
  const hit=ray.intersectObjects(pickables.filter(o=>{let v=true;o.traverseAncestors(a=>{if(!a.visible)v=false});return v&&o.visible}),true)[0];
  if(!hit){info.style.display='none';return;}
  let o=hit.object;while(o&&!(o.userData&&o.userData.kind))o=o.parent;
  const ex=o.userData;const esc=s=>String(s).replace(/[&<>]/g,c=>({'&':'&amp;','<':'&lt;','>':'&gt;'}[c]));
  info.innerHTML='<b>'+esc(ex.name||o.name)+'</b> ('+esc(ex.kind)+', '+esc(ex.role)+')\n'+esc(ex.props||'(sin propiedades)');info.style.display='block';
});
function resize(){renderer.setSize(innerWidth,innerHeight,false);camera.aspect=innerWidth/innerHeight;camera.updateProjectionMatrix();}
addEventListener('resize',resize);resize();
renderer.setAnimationLoop(()=>{controls.update();renderer.render(scene,camera);});
</script></body></html>
"""


def build_html(scene, glb, title):
    bg = prop(scene, "D_start", "bgcolor") or "8fc8ff"
    s = hgr.summary(scene)
    stats = f"{s['triangles']} triángulos · {s['primitives']} primitivas · {sum(s['nodes'].values())} nodos"
    return (HTML.replace("__TITLE__", title).replace("__GLB__", base64.b64encode(glb).decode())
            .replace("__BG__", "0x" + bg[-6:]).replace("__STATS__", json.dumps(stats)))


# ----------------------------------------------------------------- PNG software render

def render_png(scene, textures, lib, size, view):
    W, H = size
    tris = []  # (world verts, uvs, tex or None, color)
    for sc, n, tm, role in flatten(scene, lib):
        if n.kind != "mesh" or role != "visual":
            continue
        for pi in n.extra["primitives"]:
            p = sc.primitives[pi]
            m = sc.materials[p.material]
            if "-mul" in m.shader:
                continue
            tex = textures.get(sc.textures[m.textures["BASEMAP"]][0]) if "BASEMAP" in m.textures else None
            col = tuple(int(c * 255) for c in m.vectors.get("DIFFUSEC", [1, 1, 1])[:3])
            pos = [mat_apply(tm, v) for v in p.positions()]
            uv = p.uvs()
            for i in range(0, len(p.indices) - 2, 3):
                a, b2, c = p.indices[i:i + 3]
                tris.append(((pos[a], pos[b2], pos[c]),
                             (uv[a], uv[b2], uv[c]) if uv else None, tex, col))
    if not tris:
        raise SystemExit("nothing to render")
    pts = [v for t in tris for v in t[0]]
    mn = [min(p[k] for p in pts) for k in range(3)]
    mx = [max(p[k] for p in pts) for k in range(3)]
    ctr = [(mn[k] + mx[k]) / 2 for k in range(3)]
    rad = math.dist(mn, mx) / 2

    # camera basis (left-handed: forward +Z in camera space)
    cam = next(((tm, n) for sc, n, tm, role in flatten(scene, None) if n.kind == "camera"), None)
    if view.startswith("eye="):
        # eye=x,y,z:target=x,y,z[:fov=grados] (la misma cámara que el port)
        parts = dict(p.split("=") for p in view.split(":"))
        eye = [float(v) for v in parts["eye"].split(",")]
        tgt = [float(v) for v in parts["target"].split(",")]
        fov = float(parts.get("fov", "55")) * math.pi / 180
        fwd = [tgt[k] - eye[k] for k in range(3)]
        ln = math.sqrt(sum(c * c for c in fwd))
        fwd = [c / ln for c in fwd]
        wup = [0, 1, 0]
        right = [wup[1] * fwd[2] - wup[2] * fwd[1], wup[2] * fwd[0] - wup[0] * fwd[2], wup[0] * fwd[1] - wup[1] * fwd[0]]
        ln = math.sqrt(sum(c * c for c in right))
        right = [c / ln for c in right]
        up = [fwd[1] * right[2] - fwd[2] * right[1], fwd[2] * right[0] - fwd[0] * right[2], fwd[0] * right[1] - fwd[1] * right[0]]
    elif view == "camera" and cam:
        tm, cn = cam
        eye = [tm[0][3], tm[1][3], tm[2][3]]
        right = [tm[0][0], tm[1][0], tm[2][0]]
        up = [tm[0][1], tm[1][1], tm[2][1]]
        fwd = [tm[0][2], tm[1][2], tm[2][2]]
        fov = cn.extra["fov"]
    else:
        if view == "top":
            fwd = [0, -1, 0.0001]
        else:
            fwd = [-0.5, -0.7, 0.5]
        ln = math.sqrt(sum(c * c for c in fwd))
        fwd = [c / ln for c in fwd]
        fov = 0.9
        dist = rad / math.tan(fov / 2) * 1.05
        eye = [ctr[k] - fwd[k] * dist for k in range(3)]
        wup = [0, 0, 1] if view == "top" else [0, 1, 0]
        right = [wup[1] * fwd[2] - wup[2] * fwd[1], wup[2] * fwd[0] - wup[0] * fwd[2], wup[0] * fwd[1] - wup[1] * fwd[0]]
        ln = math.sqrt(sum(c * c for c in right))
        right = [c / ln for c in right]
        up = [fwd[1] * right[2] - fwd[2] * right[1], fwd[2] * right[0] - fwd[0] * right[2], fwd[0] * right[1] - fwd[1] * right[0]]
    f = (H / 2) / math.tan(fov / 2)
    near = rad * 0.001

    def proj(p):
        d = [p[k] - eye[k] for k in range(3)]
        x = sum(d[k] * right[k] for k in range(3))
        y = sum(d[k] * up[k] for k in range(3))
        z = sum(d[k] * fwd[k] for k in range(3))
        return x, y, z

    bg = prop(scene, "D_start", "bgcolor") or "8fc8ff"
    bgc = bytes(int(bg[i:i + 2], 16) for i in (0, 2, 4))
    img = bytearray(bgc * (W * H))
    zbuf = [float("inf")] * (W * H)
    for (vs, uvs, tex, col) in tris:
        cs = [proj(v) for v in vs]
        if any(c[2] <= near for c in cs):
            continue
        sx = [W / 2 + c[0] / c[2] * f for c in cs]
        sy = [H / 2 - c[1] / c[2] * f for c in cs]
        iz = [1 / c[2] for c in cs]
        x0, x1 = max(int(min(sx)), 0), min(int(max(sx)) + 1, W - 1)
        y0, y1 = max(int(min(sy)), 0), min(int(max(sy)) + 1, H - 1)
        if x0 > x1 or y0 > y1:
            continue
        area = (sx[1] - sx[0]) * (sy[2] - sy[0]) - (sx[2] - sx[0]) * (sy[1] - sy[0])
        if abs(area) < 1e-9:
            continue
        if tex and uvs:
            tw, th, trgba, _ = tex
            uz = [uvs[k][0] * iz[k] for k in range(3)]
            vz = [uvs[k][1] * iz[k] for k in range(3)]
        for y in range(y0, y1 + 1):
            py = y + 0.5
            for x in range(x0, x1 + 1):
                px = x + 0.5
                w0 = ((sx[1] - px) * (sy[2] - py) - (sx[2] - px) * (sy[1] - py)) / area
                w1 = ((sx[2] - px) * (sy[0] - py) - (sx[0] - px) * (sy[2] - py)) / area
                w2 = 1 - w0 - w1
                if w0 < 0 or w1 < 0 or w2 < 0:
                    continue
                z = w0 * iz[0] + w1 * iz[1] + w2 * iz[2]
                o = y * W + x
                if 1 / z >= zbuf[o]:
                    continue
                if tex and uvs:
                    u = (w0 * uz[0] + w1 * uz[1] + w2 * uz[2]) / z
                    v = (w0 * vz[0] + w1 * vz[1] + w2 * vz[2]) / z
                    ti = ((int(v * th) % th) * tw + (int(u * tw) % tw)) * 4
                    if trgba[ti + 3] < 128:
                        continue
                    c = trgba[ti:ti + 3]
                else:
                    c = col
                zbuf[o] = 1 / z
                img[o * 3:o * 3 + 3] = bytes(c)
    rgba = bytearray()
    for i in range(W * H):
        rgba += img[i * 3:i * 3 + 3] + b"\xff"
    return ntx.png_bytes(W, H, bytes(rgba))


# ----------------------------------------------------------------- CLI

def main(argv):
    if len(argv) < 3 or argv[0] not in ("glb", "html", "png"):
        print(__doc__)
        return 2
    cmd, src, dst = argv[:3]
    opts = argv[3:]

    def opt(name, default=None, many=False):
        vals = [opts[i + 1] for i, o in enumerate(opts) if o == name and i + 1 < len(opts)]
        return vals if many else (vals[-1] if vals else default)

    here = os.path.dirname(os.path.abspath(src))
    data_root = here
    for _ in range(3):  # find .../data_S60 (folder that holds resources/)
        if os.path.isdir(os.path.join(data_root, "resources")):
            break
        data_root = os.path.dirname(data_root)
    texdir = opt("--textures", os.path.join(data_root, "resources", "textures"))
    libs = opt("--lib", many=True) or [os.path.join(data_root, "resources", "models")]
    scene = hgr.load(src)
    textures = TextureCache(texdir)
    lib = Library(libs)
    if cmd == "png":
        w, h = (int(x) for x in opt("--size", "960x540").split("x"))
        out = render_png(scene, textures, lib, (w, h), opt("--view", "iso"))
    else:
        out = build_glb(scene, textures, lib)
        if cmd == "html":
            out = build_html(scene, out, os.path.basename(src)).encode()
    with open(dst, "wb") as f:
        f.write(out)
    print(f"{dst}: {len(out)} bytes")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
