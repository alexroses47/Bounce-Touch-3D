// Escena viva del nivel: nodos del .hgr más las instancias de `load = archivo,nodo`, con
// transformaciones que cambian (moveobject...), nodos enganchados o no (link/unlink) y visibilidad.
#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include "scene.h"
#include "vec.h"

struct RNode
{
	const Scene* scene;
	int src;              // nodo en la escena de origen
	int parent;           // nodo vivo padre (-1 = raíz)
	std::vector<int> children;
	Mat34 local, world;
	bool linked = true;   // enganchado a su padre (unlink lo pone a false)
	bool visible = true;  // setrenderstate / switchrenderstate
	bool sky = false, billboard = false;
	int instance = 0;     // 0 = el nivel; >0 = instancia de librería
	float uvOffset[2] = { 0, 0 };   // moveuv
	const char* Name() const { return scene->NodeName(src); }
	const BscNode& Src() const { return scene->nodes[src]; }
};

struct World
{
	std::vector<RNode> nodes;
	std::vector<int> roots;
	std::vector<int> instanceRoot;   // nodo raíz de cada instancia (índice = id de instancia)
	std::unordered_map<std::string, std::vector<int>> byName;

	void Build(const Scene& level);
	void Clear();

	// recalcula las matrices de mundo (barato: unos cientos de nodos)
	void UpdateTransforms();
	// enganchado hasta la raíz
	bool Active(int n) const;
	// enganchado y con render state encendido hasta la raíz (lo que se ve y lo que colisiona)
	bool Enabled(int n) const;

	// busca por nombre: primero dentro de la instancia `scope` (si >0), luego en todo el nivel
	int Find(const std::string& name, int scope = 0) const;
	// busca `name` dentro del subárbol de `root`
	int FindBelow(int root, const std::string& name) const;
	// propiedad de usuario de un nodo vivo
	bool Prop(int n, const char* key, std::string* value) const { return nodes[n].scene->Prop(nodes[n].src, key, value); }

	// engancha `child` bajo `parent` (-1 = raíz) / lo desengancha
	void Link(int parent, int child);
	void Unlink(int n) { nodes[n].linked = false; }

private:
	int add(const Scene& s, int src, int parent, int instance, bool dropTransform, int depth, bool sky);
};
