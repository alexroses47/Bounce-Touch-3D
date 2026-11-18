#include "collision.h"
#include <float.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include "world.h"

static bool sameMatrix(const Mat34& a, const Mat34& b)
{
	return memcmp(&a, &b, sizeof(Mat34)) == 0;
}

static void refresh(CollisionMesh& m, const Mat34& w)
{
	size_t n = m.local.size();
	m.world.resize(n);
	m.normals.resize(n / 3);
	m.mn = Vec3(FLT_MAX, FLT_MAX, FLT_MAX);
	m.mx = Vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	for (size_t i = 0; i < n; i++)
	{
		Vec3 p = w.Apply(m.local[i]);
		m.world[i] = p;
		m.mn = Vec3(fminf(m.mn.x, p.x), fminf(m.mn.y, p.y), fminf(m.mn.z, p.z));
		m.mx = Vec3(fmaxf(m.mx.x, p.x), fmaxf(m.mx.y, p.y), fmaxf(m.mx.z, p.z));
	}
	for (size_t t = 0; t < n / 3; t++)
		m.normals[t] = Cross(m.world[t * 3 + 1] - m.world[t * 3], m.world[t * 3 + 2] - m.world[t * 3]).Normalized();
	m.lastWorld = w;
}

void CollisionWorld::Build(const World& w)
{
	meshes.clear();
	for (size_t ni = 0; ni < w.nodes.size(); ni++)
	{
		const RNode& rn = w.nodes[ni];
		const BscNode& nd = rn.Src();
		if (nd.kind != BSC_MESH || nd.role != ROLE_COLLISION) continue;
		CollisionMesh m;
		m.node = ni;
		std::string g;
		m.group = rn.scene->Prop(rn.src, "group", &g) ? strtoul(g.c_str(), nullptr, 10) : 0;
		const Scene& s = *rn.scene;
		for (u32 r = 0; r < nd.refCount; r++)
		{
			const BscPrimitive& p = s.prims[s.refs[nd.firstRef + r]];
			const s16* v = (const s16*)(s.data + p.vboOff);
			const u16* idx = (const u16*)(s.data + p.iboOff);
			for (u32 i = 0; i + 2 < p.indexCount; i += 3)
			{
				Vec3 tri[3];
				for (int k = 0; k < 3; k++)
				{
					const s16* q = v + idx[i + k] * 6;
					tri[k] = Vec3(q[0] * p.posScale + p.posBias[0], q[1] * p.posScale + p.posBias[1], q[2] * p.posScale + p.posBias[2]);
				}
				if (Cross(tri[1] - tri[0], tri[2] - tri[0]).LengthSq() < 1e-6f) continue;   // degenerado
				m.local.insert(m.local.end(), tri, tri + 3);
			}
		}
		if (m.local.empty()) continue;
		refresh(m, rn.world);
		m.active = w.Enabled(ni);
		meshes.push_back(m);
	}
	boundsMin = Vec3(FLT_MAX, FLT_MAX, FLT_MAX);
	boundsMax = Vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	for (auto& m : meshes)
	{
		boundsMin = Vec3(fminf(boundsMin.x, m.mn.x), fminf(boundsMin.y, m.mn.y), fminf(boundsMin.z, m.mn.z));
		boundsMax = Vec3(fmaxf(boundsMax.x, m.mx.x), fmaxf(boundsMax.y, m.mx.y), fmaxf(boundsMax.z, m.mx.z));
	}
}

void CollisionWorld::Update(const World& w, float dt)
{
	for (auto& m : meshes)
	{
		m.active = w.Enabled(m.node);   // setrenderstate también apaga la colisión (CO_bouncecol)
		const Mat34& now = w.nodes[m.node].world;
		if (sameMatrix(now, m.lastWorld)) { m.velocity = Vec3(); continue; }
		Vec3 before = m.lastWorld.Pos();
		refresh(m, now);
		m.velocity = dt > 0 ? (now.Pos() - before) / dt : Vec3();
	}
}

// punto del triángulo abc más cercano a p (Ericson, Real-Time Collision Detection 5.1.5)
static Vec3 closestOnTriangle(Vec3 p, Vec3 a, Vec3 b, Vec3 c, bool* face)
{
	*face = false;
	Vec3 ab = b - a, ac = c - a, ap = p - a;
	float d1 = Dot(ab, ap), d2 = Dot(ac, ap);
	if (d1 <= 0 && d2 <= 0) return a;
	Vec3 bp = p - b;
	float d3 = Dot(ab, bp), d4 = Dot(ac, bp);
	if (d3 >= 0 && d4 <= d3) return b;
	float vc = d1 * d4 - d3 * d2;
	if (vc <= 0 && d1 >= 0 && d3 <= 0) return a + ab * (d1 / (d1 - d3));
	Vec3 cp = p - c;
	float d5 = Dot(ab, cp), d6 = Dot(ac, cp);
	if (d6 >= 0 && d5 <= d6) return c;
	float vb = d5 * d2 - d1 * d6;
	if (vb <= 0 && d2 >= 0 && d6 <= 0) return a + ac * (d2 / (d2 - d6));
	float va = d3 * d6 - d5 * d4;
	if (va <= 0 && (d4 - d3) >= 0 && (d5 - d6) >= 0)
		return b + (c - b) * ((d4 - d3) / ((d4 - d3) + (d5 - d6)));
	*face = true;
	float denom = 1.0f / (va + vb + vc);
	return a + ab * (vb * denom) + ac * (vc * denom);
}

int CollisionWorld::SphereContacts(Vec3 c, float r, Contact* out, int maxContacts) const
{
	int count = 0;
	float r2 = r * r;
	for (size_t mi = 0; mi < meshes.size(); mi++)
	{
		const CollisionMesh& m = meshes[mi];
		if (!m.active) continue;
		if (c.x + r < m.mn.x || c.x - r > m.mx.x || c.y + r < m.mn.y || c.y - r > m.mx.y ||
			c.z + r < m.mn.z || c.z - r > m.mx.z) continue;
		for (size_t t = 0; t < m.normals.size(); t++)
		{
			const Vec3* v = &m.world[t * 3];
			bool face;
			Vec3 q = closestOnTriangle(c, v[0], v[1], v[2], &face);
			Vec3 d = c - q;
			float dist2 = d.LengthSq();
			if (dist2 >= r2) continue;
			float dist = sqrtf(dist2);
			Vec3 n = m.normals[t];
			Contact ct;
			ct.point = q;
			ct.normal = dist > 1e-3f ? d / dist : (Dot(n, d) >= 0 ? n : -n);
			ct.depth = r - dist;
			ct.mesh = mi;
			ct.face = face;
			if (count < maxContacts) out[count++] = ct;
			else
			{
				int worst = 0;
				for (int k = 1; k < count; k++) if (out[k].depth < out[worst].depth) worst = k;
				if (ct.depth > out[worst].depth) out[worst] = ct;
			}
		}
	}
	return count;
}

bool CollisionWorld::RayCast(Vec3 from, Vec3 to, Vec3* hitPoint, Vec3* hitNormal) const
{
	Vec3 dir = to - from;
	Vec3 lo(fminf(from.x, to.x), fminf(from.y, to.y), fminf(from.z, to.z));
	Vec3 hi(fmaxf(from.x, to.x), fmaxf(from.y, to.y), fmaxf(from.z, to.z));
	float best = 2.0f;
	for (const CollisionMesh& m : meshes)
	{
		if (!m.active) continue;
		if (hi.x < m.mn.x || lo.x > m.mx.x || hi.y < m.mn.y || lo.y > m.mx.y || hi.z < m.mn.z || lo.z > m.mx.z) continue;
		for (size_t t = 0; t < m.normals.size(); t++)
		{
			// Möller-Trumbore
			const Vec3* v = &m.world[t * 3];
			Vec3 e1 = v[1] - v[0], e2 = v[2] - v[0];
			Vec3 p = Cross(dir, e2);
			float det = Dot(e1, p);
			if (fabsf(det) < 1e-9f) continue;
			float inv = 1.0f / det;
			Vec3 s = from - v[0];
			float u = Dot(s, p) * inv;
			if (u < 0 || u > 1) continue;
			Vec3 q = Cross(s, e1);
			float w = Dot(dir, q) * inv;
			if (w < 0 || u + w > 1) continue;
			float tt = Dot(e2, q) * inv;
			if (tt < 0 || tt > 1 || tt >= best) continue;
			best = tt;
			*hitPoint = from + dir * tt;
			*hitNormal = Dot(m.normals[t], dir) <= 0 ? m.normals[t] : -m.normals[t];
		}
	}
	return best <= 1.0f;
}
