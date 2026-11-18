#include "scene.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <map>

bool Scene::Load(const char* sceneName)
{
	Free();
	name = sceneName;
	char path[128];
	snprintf(path, sizeof(path), "%sscenes/%s.bsc", g_dataRoot, sceneName);
	FILE* f = fopen(path, "rb");
	if (!f) { logf_("no existe %s\n", path); return false; }
	fseek(f, 0, SEEK_END);
	long size = ftell(f);
	fseek(f, 0, SEEK_SET);
	data = (u8*)linearAlloc(size);
	if (!data) { logf_("sin memoria lineal para %s (%ld bytes)\n", path, size); fclose(f); return false; }
	bool ok = fread(data, 1, size, f) == (size_t)size;
	fclose(f);
	h = (const BscHeader*)data;
	if (!ok || memcmp(h->magic, "BSC1", 4) != 0 || h->version != 1 || h->fileSize != (u32)size)
	{
		logf_("%s no es BSC1 v1\n", path);
		Free();
		return false;
	}
#ifndef BT_HOST
	GSPGPU_FlushDataCache(data, size);
#endif
	materials = (const BscMaterial*)(data + h->matOff);
	prims = (const BscPrimitive*)(data + h->primOff);
	nodes = (const BscNode*)(data + h->nodeOff);
	refs = (const u32*)(data + h->refOff);

	const u32* tex = (const u32*)(data + h->texOff);
	textures.resize(h->texCount);
	for (u32 i = 0; i < h->texCount; i++)
		textures[i] = Render::GetTexture(Str(tex[i]));

	int n = h->nodeCount;
	children.assign(n, {});
	roots.clear();
	local.resize(n);
	world.resize(n);
	for (int i = 0; i < n; i++)
	{
		local[i] = Mat34::FromArray(nodes[i].transform);
		if (nodes[i].parent >= 0 && nodes[i].parent < n) children[nodes[i].parent].push_back(i);
		else roots.push_back(i);
	}
	// mundo: de las raíces hacia abajo
	std::vector<int> stack(roots.begin(), roots.end());
	for (int r : roots) world[r] = local[r];
	while (!stack.empty())
	{
		int i = stack.back();
		stack.pop_back();
		for (int c : children[i]) { world[c] = world[i] * local[c]; stack.push_back(c); }
	}
	logf_("escena %s: %lu nodos, %lu primitivas, %lu texturas, %ld KB\n", sceneName,
		h->nodeCount, h->primCount, h->texCount, size / 1024);
	return true;
}

void Scene::Free()
{
	if (data) linearFree(data);
	data = nullptr;
	h = nullptr;
	textures.clear();
	children.clear();
	roots.clear();
}

int Scene::FindNode(const char* n) const
{
	for (u32 i = 0; i < h->nodeCount; i++)
		if (strcmp(NodeName(i), n) == 0) return i;
	return -1;
}

static std::string trim(const std::string& s)
{
	size_t a = 0, b = s.size();
	while (a < b && isspace((unsigned char)s[a])) a++;
	while (b > a && isspace((unsigned char)s[b - 1])) b--;
	return s.substr(a, b - a);
}

bool Scene::Prop(int node, const char* key, std::string* value) const
{
	const char* p = Str(nodes[node].props);
	while (*p)
	{
		const char* e = p;
		while (*e && *e != '\n' && *e != '\r') e++;
		std::string line(p, e - p);
		size_t eq = line.find('=');
		std::string k = trim(line.substr(0, eq));
		if (strcasecmp(k.c_str(), key) == 0)
		{
			if (value) *value = eq == std::string::npos ? "" : trim(line.substr(eq + 1));
			return true;
		}
		p = e;
		while (*p == '\n' || *p == '\r') p++;
	}
	return false;
}

std::vector<std::pair<std::string, std::string>> Scene::Props(int node) const
{
	std::vector<std::pair<std::string, std::string>> out;
	const char* p = Str(nodes[node].props);
	while (*p)
	{
		const char* e = p;
		while (*e && *e != '\n' && *e != '\r') e++;
		std::string line(p, e - p);
		size_t eq = line.find('=');
		std::string k = trim(line.substr(0, eq));
		if (!k.empty()) out.push_back({ k, eq == std::string::npos ? "" : trim(line.substr(eq + 1)) });
		p = e;
		while (*p == '\n' || *p == '\r') p++;
	}
	return out;
}

#ifndef BT_HOST
void Scene::DrawMesh(int node, const Mat34& w, Render::Pass pass, const float* uvOffset) const
{
	const BscNode& nd = nodes[node];
	for (u32 r = 0; r < nd.refCount; r++)
	{
		const BscPrimitive& p = prims[refs[nd.firstRef + r]];
		const BscMaterial& m = materials[p.material];
		C3D_Tex* t = m.texture >= 0 ? textures[m.texture] : nullptr;
		Render::DrawPrimitive(data, p, m, t, w, pass, uvOffset);
	}
}
#endif

Mat34 BillboardMatrix(const Mat34& w, Vec3 right, Vec3 up)
{
	float sx = w.Col(0).Length(), sy = w.Col(1).Length(), sz = w.Col(2).Length();
	Vec3 fwd = Cross(right, up);   // hacia el fondo (mano izquierda)
	Vec3 p = w.Pos();
	return Mat34::FromRows(right.x * sx, up.x * sy, fwd.x * sz, p.x,
		right.y * sx, up.y * sy, fwd.y * sz, p.y,
		right.z * sx, up.z * sy, fwd.z * sz, p.z);
}

static std::map<std::string, Scene*> library;

Scene* GetLibraryScene(const char* name)
{
	auto it = library.find(name);
	if (it != library.end()) return it->second;
	Scene* s = new Scene();
	if (!s->Load(name)) { delete s; s = nullptr; }
	library[name] = s;
	return s;
}

void FreeLibraryScenes()
{
	for (auto& e : library) delete e.second;
	library.clear();
}

void CollectScene(const Scene& s, int root, const Mat34& base, bool dropRootTransform, int depth,
	std::vector<DrawItem>& draw, CollisionWorld* collision, bool sky)
{
	struct Item { int node; Mat34 parent; bool sky; };
	std::vector<Item> stack;
	if (root >= 0) stack.push_back({ root, base, sky });
	else for (int r : s.roots) stack.push_back({ r, base, sky });

	while (!stack.empty())
	{
		Item it = stack.back();
		stack.pop_back();
		const BscNode& nd = s.nodes[it.node];
		Mat34 w = (dropRootTransform && it.node == root) ? it.parent : it.parent * s.local[it.node];
		bool isSky = it.sky || nd.role == ROLE_SKYBOX;

		if (nd.kind == BSC_MESH && nd.refCount)
		{
			if (nd.role != ROLE_COLLISION)
			{
				std::string bb;
				bool billboard = s.Prop(it.node, "billboard", &bb) && (bb == "true" || bb == "1");
				draw.push_back({ &s, it.node, w, isSky, billboard });
			}
		}

		std::string load;
		if (depth < 2 && s.Prop(it.node, "load", &load) && !load.empty())
		{
			// "archivo.hgr" o "archivo.hgr,nodo"
			size_t comma = load.find(',');
			std::string file = load.substr(0, comma), sub = comma == std::string::npos ? "" : load.substr(comma + 1);
			size_t dot = file.rfind('.');
			if (dot != std::string::npos) file = file.substr(0, dot);
			for (auto& c : file) c = tolower(c);
			while (!sub.empty() && isspace((unsigned char)sub[0])) sub.erase(0, 1);
			Scene* lib = GetLibraryScene(file.c_str());
			if (lib)
			{
				int r = sub.empty() ? -1 : lib->FindNode(sub.c_str());
				if (sub.empty() || r >= 0)
					CollectScene(*lib, r, w, true, depth + 1, draw, collision, isSky);
				else
					logf_("load: %s no tiene el nodo %s\n", file.c_str(), sub.c_str());
			}
		}
		for (int c : s.children[it.node]) stack.push_back({ c, w, isSky });
	}
}
