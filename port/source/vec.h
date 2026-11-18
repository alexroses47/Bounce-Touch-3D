// Vectores y matrices afines. Convención del .hgr: vector columna (p' = M * p), matriz 3x4 por
// filas con la traslación en la columna 3. Mundo: Y arriba, mano izquierda (como Direct3D).
#pragma once
#include <math.h>

constexpr float PI = 3.14159265358979f;
constexpr float DEG2RAD = PI / 180.0f;

struct Vec3
{
	float x = 0, y = 0, z = 0;
	constexpr Vec3() = default;
	constexpr Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
	Vec3 operator+(Vec3 o) const { return { x + o.x, y + o.y, z + o.z }; }
	Vec3 operator-(Vec3 o) const { return { x - o.x, y - o.y, z - o.z }; }
	Vec3 operator*(float s) const { return { x * s, y * s, z * s }; }
	Vec3 operator/(float s) const { return { x / s, y / s, z / s }; }
	Vec3 operator-() const { return { -x, -y, -z }; }
	Vec3& operator+=(Vec3 o) { x += o.x; y += o.y; z += o.z; return *this; }
	Vec3& operator-=(Vec3 o) { x -= o.x; y -= o.y; z -= o.z; return *this; }
	Vec3& operator*=(float s) { x *= s; y *= s; z *= s; return *this; }
	float LengthSq() const { return x * x + y * y + z * z; }
	float Length() const { return sqrtf(LengthSq()); }
	Vec3 Normalized() const { float l = Length(); return l > 1e-12f ? *this / l : Vec3(); }
};
inline Vec3 operator*(float s, Vec3 v) { return v * s; }
inline float Dot(Vec3 a, Vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
inline Vec3 Cross(Vec3 a, Vec3 b) { return { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x }; }
inline float Clamp(float v, float lo, float hi) { return v < lo ? lo : (v > hi ? hi : v); }
inline float Approach(float v, float target, float maxStep)
{
	return v < target ? (v + maxStep > target ? target : v + maxStep) : (v - maxStep < target ? target : v - maxStep);
}

struct Mat34
{
	float m[3][4];

	static Mat34 Identity() { return FromRows(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0); }
	static Mat34 FromRows(float a, float b, float c, float d, float e, float f, float g, float h,
		float i, float j, float k, float l)
	{
		Mat34 r;
		float v[12] = { a, b, c, d, e, f, g, h, i, j, k, l };
		for (int n = 0; n < 12; n++) r.m[n / 4][n % 4] = v[n];
		return r;
	}
	static Mat34 FromArray(const float* v) { Mat34 r; for (int n = 0; n < 12; n++) r.m[n / 4][n % 4] = v[n]; return r; }
	static Mat34 Translate(Vec3 t) { return FromRows(1, 0, 0, t.x, 0, 1, 0, t.y, 0, 0, 1, t.z); }
	// giro alrededor de Y: con ángulo a, (0,0,1) pasa a (sin a, 0, cos a)
	static Mat34 RotY(float a)
	{
		float s = sinf(a), c = cosf(a);
		return FromRows(c, 0, s, 0, 0, 1, 0, 0, -s, 0, c, 0);
	}
	// giro de ángulo a alrededor de un eje unitario
	static Mat34 Rotate(Vec3 u, float a)
	{
		float s = sinf(a), c = cosf(a), t = 1 - c;
		return FromRows(t * u.x * u.x + c, t * u.x * u.y - s * u.z, t * u.x * u.z + s * u.y, 0,
			t * u.x * u.y + s * u.z, t * u.y * u.y + c, t * u.y * u.z - s * u.x, 0,
			t * u.x * u.z - s * u.y, t * u.y * u.z + s * u.x, t * u.z * u.z + c, 0);
	}

	Mat34 operator*(const Mat34& b) const
	{
		Mat34 r;
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 4; j++)
				r.m[i][j] = m[i][0] * b.m[0][j] + m[i][1] * b.m[1][j] + m[i][2] * b.m[2][j] + (j == 3 ? m[i][3] : 0);
		return r;
	}
	Vec3 Apply(Vec3 p) const
	{
		return { m[0][0] * p.x + m[0][1] * p.y + m[0][2] * p.z + m[0][3],
				 m[1][0] * p.x + m[1][1] * p.y + m[1][2] * p.z + m[1][3],
				 m[2][0] * p.x + m[2][1] * p.y + m[2][2] * p.z + m[2][3] };
	}
	Vec3 Col(int c) const { return { m[0][c], m[1][c], m[2][c] }; }
	Vec3 Pos() const { return Col(3); }
	// solo la parte de giro, sin escala (ortonormaliza)
	Mat34 RotationOnly() const
	{
		Vec3 x = Col(0).Normalized(), y = Col(1), z;
		y = (y - x * Dot(x, y)).Normalized();
		z = Cross(x, y);
		if (Dot(z, Col(2)) < 0) z = -z;
		return FromRows(x.x, y.x, z.x, 0, x.y, y.y, z.y, 0, x.z, y.z, z.z, 0);
	}
};
