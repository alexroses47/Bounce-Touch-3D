#include "world.h"
#include <ctype.h>
#include <string.h>

void World::Clear()
{
	nodes.clear();
	roots.clear();
	instanceRoot.assign(1, -1);
	byName.clear();
}

int World::add(const Scene& s, int src, int parent, int instance, bool dropTransform, int depth, bool sky)
{
	int id = nodes.size();
	nodes.push_back(RNode());
	RNode& n = nodes.back();
	n.scene = &s;
	n.src = src;
	n.parent = parent;
	n.instance = instance;
	n.local = dropTransform ? Mat34::Identity() : s.local[src];
	n.sky = sky || s.nodes[src].role == ROLE_SKYBOX;
	std::string bb;
	n.billboard = s.Prop(src, "billboard", &bb) && (bb == "true" || bb == "1");
	bool isSky = n.sky;
	if (parent >= 0) nodes[parent].children.push_back(id);
	else roots.push_back(id);
	byName[s.NodeName(src)].push_back(id);

	// instancia de librería: `load = archivo.hgr[,nodo]`
	std::string load;
	if (depth < 2 && s.Prop(src, "load", &load) && !load.empty())
	{
		size_t comma = load.find(',');
		std::string file = load.substr(0, comma), sub = comma == std::string::npos ? "" : load.substr(comma + 1);
		size_t dot = file.rfind('.');
		if (dot != std::string::npos) file = file.substr(0, dot);
		for (auto& c : file) c = tolower(c);
		while (!sub.empty() && isspace((unsigned char)sub[0])) sub.erase(0, 1);
		Scene* lib = GetLibraryScene(file.c_str());
		if (lib)
		{
			int inst = instanceRoot.size();
			instanceRoot.push_back(id);
			if (sub.empty())
				for (int r : lib->roots) add(*lib, r, id, inst, false, depth + 1, isSky);
			else
			{
				int r = lib->FindNode(sub.c_str());
				if (r >= 0) add(*lib, r, id, inst, true, depth + 1, isSky);
				else logf_("load: %s no tiene el nodo %s\n", file.c_str(), sub.c_str());
			}
		}
	}
	for (int c : s.children[src]) add(s, c, id, instance, false, depth, isSky);
	return id;
}

void World::Build(const Scene& level)
{
	Clear();
	for (int r : level.roots) add(level, r, -1, 0, false, 0, false);
	UpdateTransforms();
}

void World::UpdateTransforms()
{
	std::vector<int> stack(roots.begin(), roots.end());
	for (int r : roots) nodes[r].world = nodes[r].local;
	while (!stack.empty())
	{
		int i = stack.back();
		stack.pop_back();
		for (int c : nodes[i].children)
		{
			nodes[c].world = nodes[i].world * nodes[c].local;
			stack.push_back(c);
		}
	}
}

bool World::Active(int n) const
{
	for (; n >= 0; n = nodes[n].parent)
		if (!nodes[n].linked) return false;
	return true;
}

bool World::Enabled(int n) const
{
	for (; n >= 0; n = nodes[n].parent)
		if (!nodes[n].linked || !nodes[n].visible) return false;
	return true;
}

int World::Find(const std::string& name, int scope) const
{
	auto it = byName.find(name);
	if (it == byName.end()) return -1;
	if (scope > 0)
		for (int n : it->second)
			if (nodes[n].instance == scope) return n;
	for (int n : it->second)
		if (nodes[n].instance == 0) return n;
	return it->second.front();
}

int World::FindBelow(int root, const std::string& name) const
{
	// "CO_nombre" = el objeto de colisión del nodo "nombre" (setrenderstate,O_node_start,CO_bouncecol,0)
	if (name.size() > 3 && name.compare(0, 3, "CO_") == 0 && byName.find(name) == byName.end())
		return FindBelow(root, name.substr(3));
	if (root < 0) return Find(name);
	if (name == nodes[root].Name()) return root;
	std::vector<int> stack(nodes[root].children.begin(), nodes[root].children.end());
	while (!stack.empty())
	{
		int i = stack.back();
		stack.pop_back();
		if (name == nodes[i].Name()) return i;
		stack.insert(stack.end(), nodes[i].children.begin(), nodes[i].children.end());
	}
	return Find(name, nodes[root].instance);
}

void World::Link(int parent, int child)
{
	RNode& c = nodes[child];
	if (c.parent != parent)
	{
		// conserva la posición en el mundo al cambiar de padre
		auto& sib = c.parent >= 0 ? nodes[c.parent].children : roots;
		for (size_t i = 0; i < sib.size(); i++) if (sib[i] == child) { sib.erase(sib.begin() + i); break; }
		if (parent >= 0)
		{
			nodes[parent].children.push_back(child);
			// local = inversa(mundo del padre) * mundo actual (padres sin escala no uniforme)
			const Mat34& p = nodes[parent].world;
			Vec3 x = p.Col(0), y = p.Col(1), z = p.Col(2);
			float sx = Dot(x, x), sy = Dot(y, y), sz = Dot(z, z);
			Mat34 inv = Mat34::FromRows(x.x / sx, x.y / sx, x.z / sx, 0, y.x / sy, y.y / sy, y.z / sy, 0,
				z.x / sz, z.y / sz, z.z / sz, 0);
			Vec3 t = p.Pos();
			Vec3 it(-(inv.m[0][0] * t.x + inv.m[0][1] * t.y + inv.m[0][2] * t.z),
				-(inv.m[1][0] * t.x + inv.m[1][1] * t.y + inv.m[1][2] * t.z),
				-(inv.m[2][0] * t.x + inv.m[2][1] * t.y + inv.m[2][2] * t.z));
			inv.m[0][3] = it.x; inv.m[1][3] = it.y; inv.m[2][3] = it.z;
			c.local = inv * c.world;
		}
		else
		{
			roots.push_back(child);
			c.local = c.world;
		}
		c.parent = parent;
	}
	c.linked = true;
}
