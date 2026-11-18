// Estado de GPU: shader de escenas, materiales, texturas y matrices del ojo actual.
#pragma once
#include "bsc.h"
#include "vec.h"

void logf_(const char* fmt, ...);

#ifdef BT_HOST
// build host: sin GPU; el núcleo solo necesita los nombres
struct C3D_Tex;
namespace Render
{
	enum Pass { PassNormal, PassSky };
	inline C3D_Tex* GetTexture(const char*) { return nullptr; }
}
#else
#include <3ds.h>
#include <citro3d.h>

namespace Render
{
	void Init();
	void Exit();

	// matrices del ojo que se está dibujando
	void SetCamera(const C3D_Mtx& projection, const C3D_Mtx& view);

	// romfs:/tex/<name>.t3x (se carga una vez; nullptr si no existe)
	C3D_Tex* GetTexture(const char* name);

	// prepara el shader y el formato de vértice (después de dibujar otra cosa)
	void Bind();

	enum Pass { PassNormal, PassSky };
	// dibuja una primitiva BSC con la transformación de mundo dada
	void DrawPrimitive(const u8* fileData, const BscPrimitive& p, const BscMaterial& mat,
		C3D_Tex* tex, const Mat34& world, Pass pass, const float* uvOffset = nullptr);

	// estadísticas del cuadro
	extern u32 statPrims, statTris;
}
#endif
