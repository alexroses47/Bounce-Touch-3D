// Colisión del nivel: una malla por nodo `collisionmesh = this`. Los triángulos se guardan en
// espacio local y se pasan al mundo solo cuando el nodo se mueve; las mallas desenganchadas
// (unlink) no participan.
#pragma once
#include "platform.h"
#include <vector>
#include "vec.h"

struct Scene;
struct World;

struct Contact
{
	Vec3 point;    // punto más cercano del triángulo
	Vec3 normal;   // de la superficie hacia el centro de la esfera
	float depth;   // penetración (radio - distancia)
	int mesh;      // índice en CollisionWorld::meshes
	bool face;     // el punto cae dentro de la cara (no en una arista o un vértice)
};

struct CollisionMesh
{
	int node;                  // nodo vivo
	u32 group;
	std::vector<Vec3> local;   // 3 vértices por triángulo
	std::vector<Vec3> world;
	std::vector<Vec3> normals; // en mundo
	Vec3 mn, mx;               // caja en mundo
	Mat34 lastWorld;
	bool active = true;
	Vec3 velocity;             // del último cuadro (superficies que se mueven)
};

struct CollisionWorld
{
	std::vector<CollisionMesh> meshes;
	Vec3 boundsMin, boundsMax;

	void Clear() { meshes.clear(); }
	void Build(const World& w);                 // todas las mallas de colisión del mundo
	void Update(const World& w, float dt);      // sigue a los nodos que se mueven o se desenganchan

	// contactos de una esfera (como mucho maxContacts, los más profundos)
	int SphereContacts(Vec3 center, float radius, Contact* out, int maxContacts) const;
	// primer triángulo que corta el segmento from->to (el más cercano a from); false si ninguno.
	// Equivale al rayo de GameCamera::v04 (FUN_00026640): punto de corte y normal de la cara.
	bool RayCast(Vec3 from, Vec3 to, Vec3* hitPoint, Vec3* hitNormal) const;
};
