#include "render.h"
#include <tex3ds.h>
#include <stdio.h>
#include <string>
#include <unordered_map>

#include "scene_shbin.h"

namespace Render
{
	u32 statPrims, statTris;

	static DVLB_s* dvlb;
	static shaderProgram_s program;
	static int uProj, uModelView, uPosScale, uPosBias, uUvScale, uUvBias, uColor;
	static C3D_Mtx view;
	static std::unordered_map<std::string, C3D_Tex*> textures;

	// estado aplicado (para no repetir llamadas)
	static int curMode = -1;
	static C3D_Tex* curTex = nullptr;

	void Init()
	{
		dvlb = DVLB_ParseFile((u32*)scene_shbin, scene_shbin_size);
		shaderProgramInit(&program);
		shaderProgramSetVsh(&program, &dvlb->DVLE[0]);
		auto loc = [](const char* n) { return shaderInstanceGetUniformLocation(program.vertexShader, n); };
		uProj = loc("projection");
		uModelView = loc("modelView");
		uPosScale = loc("posScale");
		uPosBias = loc("posBias");
		uUvScale = loc("uvScale");
		uUvBias = loc("uvBias");
		uColor = loc("color");
	}

	void Exit()
	{
		for (auto& t : textures)
			if (t.second) { C3D_TexDelete(t.second); delete t.second; }
		textures.clear();
		shaderProgramFree(&program);
		DVLB_Free(dvlb);
	}

	void Bind()
	{
		C3D_BindProgram(&program);
		C3D_AttrInfo* attr = C3D_GetAttrInfo();
		AttrInfo_Init(attr);
		AttrInfo_AddLoader(attr, 0, GPU_SHORT, 3);   // x y z
		// u v + relleno: los atributos suman exactamente los 12 bytes del vértice (el PICA200
		// real no tolera un stride mayor que los atributos declarados; Azahar sí)
		AttrInfo_AddLoader(attr, 1, GPU_SHORT, 3);
		C3D_AlphaTest(true, GPU_GREATER, 0x7F);      // color key / recortes
		C3D_CullFace(GPU_CULL_NONE);
		curMode = -1;
		curTex = nullptr;
	}

	void SetCamera(const C3D_Mtx& projection, const C3D_Mtx& v)
	{
		view = v;
		C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uProj, &projection);
	}

	C3D_Tex* GetTexture(const char* name)
	{
		auto it = textures.find(name);
		if (it != textures.end()) return it->second;
		char path[128];
		snprintf(path, sizeof(path), "%stex/%s.t3x", g_dataRoot, name);
		C3D_Tex* tex = new C3D_Tex();
		FILE* f = fopen(path, "rb");
		Tex3DS_Texture t3x = f ? Tex3DS_TextureImportStdio(f, tex, NULL, false) : NULL;
		if (f) fclose(f);
		if (!t3x)
		{
			logf_("no se pudo cargar %s\n", path);
			delete tex;
			textures[name] = nullptr;
			return nullptr;
		}
		Tex3DS_TextureFree(t3x);
		C3D_TexSetFilter(tex, GPU_LINEAR, GPU_LINEAR);
		C3D_TexSetFilterMipmap(tex, GPU_LINEAR);
		C3D_TexSetWrap(tex, GPU_REPEAT, GPU_REPEAT);
		textures[name] = tex;
		return tex;
	}

	static void toC3D(const Mat34& a, C3D_Mtx* out)
	{
		for (int r = 0; r < 3; r++)
		{
			out->r[r].x = a.m[r][0];
			out->r[r].y = a.m[r][1];
			out->r[r].z = a.m[r][2];
			out->r[r].w = a.m[r][3];
		}
		out->r[3].x = out->r[3].y = out->r[3].z = 0;
		out->r[3].w = 1;
	}

	// modos: bit0 textura, bit1 multiplicar, bit2 sin escritura de Z, bit3 cielo
	static void applyMode(int mode)
	{
		if (mode == curMode) return;
		curMode = mode;
		C3D_TexEnv* env = C3D_GetTexEnv(0);
		C3D_TexEnvInit(env);
		if (mode & 1)
		{
			C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR);
			C3D_TexEnvFunc(env, C3D_Both, GPU_MODULATE);
		}
		else
		{
			C3D_TexEnvSrc(env, C3D_Both, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR);
			C3D_TexEnvFunc(env, C3D_Both, GPU_REPLACE);
		}
		for (int i = 1; i < 6; i++) C3D_TexEnvInit(C3D_GetTexEnv(i));

		if (mode & 2)   // sombras: destino * fuente
			C3D_AlphaBlend(GPU_BLEND_ADD, GPU_BLEND_ADD, GPU_DST_COLOR, GPU_ZERO, GPU_ZERO, GPU_ONE);
		else
			C3D_AlphaBlend(GPU_BLEND_ADD, GPU_BLEND_ADD, GPU_ONE, GPU_ZERO, GPU_ONE, GPU_ZERO);

		if (mode & 8)
			C3D_DepthTest(false, GPU_ALWAYS, GPU_WRITE_COLOR);
		else if (mode & (2 | 4))
			C3D_DepthTest(true, GPU_GREATER, GPU_WRITE_COLOR);
		else
			C3D_DepthTest(true, GPU_GREATER, GPU_WRITE_ALL);
	}

	void DrawPrimitive(const u8* data, const BscPrimitive& p, const BscMaterial& mat,
		C3D_Tex* tex, const Mat34& world, Pass pass, const float* uvOffset)
	{
		int mode = 0;
		if ((mat.flags & BSC_MAT_TEXTURED) && tex) mode |= 1;
		if (mat.flags & BSC_MAT_MULTIPLY) mode |= 2;
		if (mat.flags & BSC_MAT_NO_ZWRITE) mode |= 4;
		if (pass == PassSky) mode |= 8;
		applyMode(mode);
		if ((mode & 1) && tex != curTex) { C3D_TexBind(0, tex); curTex = tex; }

		C3D_Mtx w, mv;
		toC3D(world, &w);
		Mtx_Multiply(&mv, &view, &w);
		C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uModelView, &mv);
		C3D_FVUnifSet(GPU_VERTEX_SHADER, uPosScale, p.posScale, p.posScale, p.posScale, 0);
		C3D_FVUnifSet(GPU_VERTEX_SHADER, uPosBias, p.posBias[0], p.posBias[1], p.posBias[2], 1);
		C3D_FVUnifSet(GPU_VERTEX_SHADER, uUvScale, p.uvScale, -p.uvScale, 0, 0);
		// uv' = (u*s + du, 1 - (v*s + dv)) : moveuv desplaza en el sentido de las UV del .hgr
		float du = uvOffset ? uvOffset[0] : 0, dv = uvOffset ? uvOffset[1] : 0;
		C3D_FVUnifSet(GPU_VERTEX_SHADER, uUvBias, du, 1 - dv, 0, 0);
		if (mode & 1)
			C3D_FVUnifSet(GPU_VERTEX_SHADER, uColor, 1, 1, 1, 1);
		else
			C3D_FVUnifSet(GPU_VERTEX_SHADER, uColor, (mat.rgba & 0xFF) / 255.0f,
				((mat.rgba >> 8) & 0xFF) / 255.0f, ((mat.rgba >> 16) & 0xFF) / 255.0f, 1);

		C3D_BufInfo* buf = C3D_GetBufInfo();
		BufInfo_Init(buf);
		BufInfo_Add(buf, data + p.vboOff, 12, 2, 0x10);
		C3D_DrawElements(GPU_TRIANGLES, p.indexCount, C3D_UNSIGNED_SHORT, data + p.iboOff);
		statPrims++;
		statTris += p.indexCount / 3;
	}
}
