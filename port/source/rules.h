// Reglas del motor como funciones puras (sin dependencias de la 3DS): las usa el juego y se
// compilan también en el Mac para los tests diferenciales (tests/).
// Cada regla cita su origen en Bounce.exe. "VERIFICADO" = leído en el binario y comprobado con
// test diferencial (tests/test_rules_diff.py) o, si no es lógica pura, con el test indicado;
// "PENDIENTE" = falta evidencia (ver docs/GUIONES.md, lista de reglas).
#pragma once
#include "vec.h"

namespace Rules
{
	// ---- matemáticas de KA3D (para que el resto copie exactamente el original)

	// M(3x3, por filas) * v, sin traslación. VERIFICADO: FUN_0007adcc (0x7adcc), test diferencial.
	Vec3 RotApply(const Mat34& m, Vec3 v);
	// Inversa afín de una 3x4. VERIFICADO: FUN_0007af54 (0x7af54), test diferencial.
	Mat34 AffineInverse(const Mat34& m);
	// Giro de `angle` radianes alrededor del eje unitario `axis` (mano derecha, vector columna).
	// VERIFICADO: FUN_000738ec (0x738ec), test diferencial.
	Mat34 AxisAngle(Vec3 axis, float angle);
	// v_t - e * v_n respecto de la normal n. VERIFICADO: FUN_0014a2fc (0x14a2fc), test diferencial.
	Vec3 Reflect(Vec3 v, Vec3 n, float e);

	// ---- objetos de colisión

	// Bits de la propiedad `group`. VERIFICADO (lectura): FUN_00021fe8 (0x21fe8) los guarda en el
	// objeto de colisión: bits 0-3 -> +0x148, 4-7 -> +0x14c, bit 8 -> +0x150, bits 9..17 -> +0x151..+0x159.
	inline int GroupSurfaceMode(unsigned g) { return g & 0xF; }          // +0x148
	inline bool GroupNoCrush(unsigned g) { return (g >> 11) & 1; }       // +0x153
	inline bool GroupDeadly(unsigned g) { return (g >> 12) & 1; }        // +0x154
	inline bool GroupBouncy(unsigned g) { return (g >> 15) & 1; }        // +0x157

	// Restitución de la bola del jugador. VERIFICADO (lectura + traza EKA2L1): PlayerBall::v05 (0x4f2c8) usa
	// el campo +0x134 de la bola (0,1 en ejecución) solo si el objeto tocado tiene +0x157 == 1 (bit 15 de
	// `group`); si no, 0.001 (DAT_00050274). Traza: 14 impactos con bit 15 -> e = 0,1000; 9 sin él -> 0,0010.
	// Test diferencial de la respuesta completa: PENDIENTE.
	inline float PlayerRestitution(unsigned group, float ballRestitution)
	{
		return GroupBouncy(group) ? ballRestitution : 0.001f;
	}

	// ---- moveobject / rotateobject

	// Vector de los argumentos (coordenadas de 3ds Max) -> vector que guarda la acción.
	// VERIFICADO (lectura): FUN_00013784 (fábrica de acciones, 0x13784), casos 0x15 (moveobject)
	// y 0x1e (rotateobject): sin el flag `ejes` se guarda (x, y, -z); con él, (x, z, y).
	Vec3 ActionVector(Vec3 args, bool worldAxes);

	// rotateobject: ángulo total en radianes = |v| * PI / 180. VERIFICADO (lectura): FUN_00013784
	// caso 0x1e; DAT_000161b8 -> 0x1674a0 = 3.1415927.
	float RotationAngle(Vec3 args);

	// Un paso de moveobject. VERIFICADO (lectura + piezas con test diferencial): FUN_00039698
	// (0x39698), rama tipo 0: Δ = RotApply(base, dir) * (velocidad * dt), con base = matriz local
	// ACTUAL del nodo (sin `ejes`) o AffineInverse(mundo del nodo) (con `ejes`; local si no tiene
	// padre). La velocidad es distancia / tiempo (FUN_0003ae40, campos +0x2c / +0x30).
	Mat34 MoveStep(const Mat34& local, const Mat34& basis, Vec3 dir, float step);

	// Un paso de rotateobject. VERIFICADO (lectura + piezas con test diferencial): FUN_00039698
	// rama tipo 1: local = local(3x3) * AxisAngle(eje, velocidad * dt) (FUN_0007a984, FUN_000738ec,
	// FUN_00074054 = producto A*B); la traslación se conserva (FUN_0008a390).
	Mat34 RotateLocal(const Mat34& local, Vec3 axis, float angle);
}
