// Escena BSC1 cargada en memoria lineal (la GPU lee vértices e índices sin copiarlos).
#pragma once
#include <string>
#include <vector>
#include "bsc.h"
#include "platform.h"
#include "render.h"
#include "vec.h"

struct Scene
{
	std::string name;
	u8* data = nullptr;
	const BscHeader* h = nullptr;
	const BscMaterial* materials = nullptr;
	const BscPrimitive* prims = nullptr;
	const BscNode* nodes = nullptr;
	const u32* refs = nullptr;
	std::vector<C3D_Tex*> textures;
	std::vector<std::vector<int>> children;   // hijos de cada nodo
	std::vector<int> roots;
	std::vector<Mat34> local;                 // transformación local de cada nodo
	std::vector<Mat34> world;                 // mundo (escena sin mover)

	bool Load(const char* name);              // romfs:/scenes/<name>.bsc
	void Free();
	~Scene() { Free(); }

	const char* Str(u32 off) const { return (const char*)data + h->stringsOff + off; }
	const char* NodeName(int i) const { return Str(nodes[i].name); }
	int FindNode(const char* name) const;
	// valor de una propiedad de usuario del nodo ("clave = valor"); false si no está
	bool Prop(int node, const char* key, std::string* value) const;
	// todas las propiedades del nodo, en orden ("clave", "valor")
	std::vector<std::pair<std::string, std::string>> Props(int node) const;

	// dibuja la malla de un nodo con una transformación de mundo
#ifndef BT_HOST
	void DrawMesh(int node, const Mat34& world, Render::Pass pass, const float* uvOffset = nullptr) const;
#endif
};

// escenas de librería (resources/models): se cargan una vez
Scene* GetLibraryScene(const char* name);
void FreeLibraryScenes();

// Lista de cosas que dibujar de un nivel: nodos visibles del nivel e instancias de
// `load = archivo.hgr[,nodo]`, ya con su matriz de mundo.
struct DrawItem { const Scene* scene; int node; Mat34 world; bool sky; bool billboard = false; };

// matriz de un billboard (`billboard = true`): misma posición y escala, plano XY local de cara a la cámara
Mat34 BillboardMatrix(const Mat34& world, Vec3 camRight, Vec3 camUp);

// recorre una escena (o el subárbol de `root`) colocada en `base` y añade lo dibujable
// y, si se pasa, las mallas de colisión
struct CollisionWorld;
void CollectScene(const Scene& s, int root, const Mat34& base, bool dropRootTransform, int depth,
	std::vector<DrawItem>& draw, CollisionWorld* collision, bool sky = false);
