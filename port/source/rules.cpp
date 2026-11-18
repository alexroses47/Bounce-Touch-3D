#include "rules.h"

namespace Rules
{
	Vec3 RotApply(const Mat34& m, Vec3 v)
	{
		return Vec3(m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z,
			m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z,
			m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z);
	}

	Mat34 AffineInverse(const Mat34& a)
	{
		const float (*m)[4] = a.m;
		float c00 = m[1][1] * m[2][2] - m[1][2] * m[2][1];
		float c01 = m[1][2] * m[2][0] - m[1][0] * m[2][2];
		float c02 = m[1][0] * m[2][1] - m[1][1] * m[2][0];
		float det = m[0][0] * c00 + m[0][1] * c01 + m[0][2] * c02;
		float id = det != 0 ? 1.0f / det : 0.0f;
		Mat34 r;
		r.m[0][0] = c00 * id;
		r.m[0][1] = (m[0][2] * m[2][1] - m[0][1] * m[2][2]) * id;
		r.m[0][2] = (m[0][1] * m[1][2] - m[0][2] * m[1][1]) * id;
		r.m[1][0] = c01 * id;
		r.m[1][1] = (m[0][0] * m[2][2] - m[0][2] * m[2][0]) * id;
		r.m[1][2] = (m[0][2] * m[1][0] - m[0][0] * m[1][2]) * id;
		r.m[2][0] = c02 * id;
		r.m[2][1] = (m[0][1] * m[2][0] - m[0][0] * m[2][1]) * id;
		r.m[2][2] = (m[0][0] * m[1][1] - m[0][1] * m[1][0]) * id;
		for (int i = 0; i < 3; i++)
			r.m[i][3] = -(r.m[i][0] * m[0][3] + r.m[i][1] * m[1][3] + r.m[i][2] * m[2][3]);
		return r;
	}

	Mat34 AxisAngle(Vec3 axis, float angle)
	{
		return Mat34::Rotate(axis, angle);
	}

	Vec3 Reflect(Vec3 v, Vec3 n, float e)
	{
		float nn = Dot(n, n);
		Vec3 vn = nn != 0 ? n * (Dot(v, n) / nn) : Vec3();
		return (v - vn) - vn * e;
	}

	Vec3 ActionVector(Vec3 v, bool worldAxes)
	{
		// FUN_00013784 casos 0x15/0x1e: flag `ejes` = 0 -> (x, y, -z); = 1 -> (x, z, y)
		return worldAxes ? Vec3(v.x, v.z, v.y) : Vec3(v.x, v.y, -v.z);
	}

	float RotationAngle(Vec3 v)
	{
		return v.Length() * PI / 180.0f;   // FUN_00013784 caso 0x1e
	}

	Mat34 MoveStep(const Mat34& local, const Mat34& basis, Vec3 dir, float step)
	{
		Vec3 d = RotApply(basis, dir) * step;   // FUN_00039698 tipo 0
		Mat34 r = local;
		r.m[0][3] += d.x;
		r.m[1][3] += d.y;
		r.m[2][3] += d.z;
		return r;
	}

	Mat34 RotateLocal(const Mat34& local, Vec3 axis, float angle)
	{
		Mat34 r = local * AxisAngle(axis, angle);   // FUN_00074054(local3x3, R) = local * R
		r.m[0][3] = local.m[0][3];                  // FUN_0008a390: solo cambia el giro
		r.m[1][3] = local.m[1][3];
		r.m[2][3] = local.m[2][3];
		return r;
	}
}

#ifdef BT_HOST_TESTS
// Envoltorios en C para los tests en Python (ctypes). Solo en la compilación del Mac.
static Mat34 M(const float* a) { return Mat34::FromArray(a); }
static void out12(const Mat34& r, float* o) { for (int i = 0; i < 12; i++) o[i] = r.m[i / 4][i % 4]; }
extern "C"
{
	void bt_rot_apply(const float m[12], const float v[3], float o[3])
	{
		Vec3 r = Rules::RotApply(M(m), Vec3(v[0], v[1], v[2])); o[0] = r.x; o[1] = r.y; o[2] = r.z;
	}
	void bt_affine_inverse(const float m[12], float o[12]) { out12(Rules::AffineInverse(M(m)), o); }
	void bt_axis_angle(const float a[3], float ang, float o[12]) { out12(Rules::AxisAngle(Vec3(a[0], a[1], a[2]), ang), o); }
	void bt_reflect(const float v[3], const float n[3], float e, float o[3])
	{
		Vec3 r = Rules::Reflect(Vec3(v[0], v[1], v[2]), Vec3(n[0], n[1], n[2]), e); o[0] = r.x; o[1] = r.y; o[2] = r.z;
	}
	void bt_action_vector(const float in[3], int worldAxes, float o[3])
	{
		Vec3 v = Rules::ActionVector(Vec3(in[0], in[1], in[2]), worldAxes != 0); o[0] = v.x; o[1] = v.y; o[2] = v.z;
	}
	float bt_rotation_angle(const float in[3]) { return Rules::RotationAngle(Vec3(in[0], in[1], in[2])); }
	void bt_move_step(const float local[12], const float basis[12], const float dir[3], float step, float o[12])
	{
		out12(Rules::MoveStep(M(local), M(basis), Vec3(dir[0], dir[1], dir[2]), step), o);
	}
	void bt_rotate_local(const float local[12], const float axis[3], float angle, float o[12])
	{
		out12(Rules::RotateLocal(M(local), Vec3(axis[0], axis[1], axis[2]), angle), o);
	}
}
#endif
