// Bola del jugador: física reimplementada según docs/FISICA.md.
#pragma once
#include "collision.h"
#include "trace.h"
#include "vec.h"

struct BallInput
{
	float steer = 0;      // -1 izquierda .. +1 derecha
	float forward = 0;    // -1 atrás .. +1 adelante
	bool jumpPressed = false;
};

struct Ball
{
	// parámetros (unidades del .hgr)
	// Radio de colisión 50: D_bounce radius=50 (bounce.hgr) y traza del original en EKA2L1 (centro a 49,9-50,0 del
	// suelo plano en reposo, re/traces). El 55,0 del constructor de Ball (+0x138) no es el radio de colisión.
	static constexpr float RADIUS = 50.0f;
	static constexpr float MASS = 1.0f;
	static constexpr float CONTROL_IMPULSE = 1750.0f;
	static constexpr float GROUND_FACTOR = 1.4f, AIR_FACTOR = 0.25f;
	static constexpr float TURN_SLOW = 90.0f, TURN_FAST = 135.0f;   // grados/s
	// Rozamiento (PlayerBall::v05 0x4f2c8, líneas del campo this[0x53]): fuerza contra la velocidad tras el
	// choque, |F| = min(mu*m*N, m*|v|/dt) con N = -(g.n), aplicada en la integración del cuadro siguiente.
	// mu = 0,225: traza EKA2L1 (|F| = 450*n.y en 3654 contactos con rozamiento, error mediano 0,8 %).
	// El término de tope (bola casi parada) difiere ~2 % de la traza: PENDIENTE.
	static constexpr float FRICTION = 0.225f;
	static constexpr float JUMP_IMPULSE = 1300.0f;
	// Campo +0x134. El constructor de Ball pone 0,4 (FUN_0001d97c), pero en ejecución vale 0,1 (traza EKA2L1:
	// 8566 choques con +0x134 = 0,1; restitución medida 0,1000 en superficies con bit 15). Quién escribe el 0,1:
	// PENDIENTE de localizar en el binario.
	float restitution = 0.1f;
	float maxSpeed = 2000.0f;
	float formLimit = 800.0f;     // forma normal
	Vec3 gravity = Vec3(0, -2000.0f, 0);

	// estado
	Vec3 pos, vel;
	float yaw = 0;                // orientación (la cámara la sigue)
	bool onGround = false;
	Vec3 groundNormal = Vec3(0, 1, 0);
	Mat34 roll = Mat34::Identity();   // giro visual acumulado
	int contactsLastStep = 0;
	int touched[8];               // mallas de colisión tocadas en el último paso
	int touchedCount = 0;
	bool controlEnabled = true;   // setgamemode
	Trace* trace = nullptr;       // traza para el comparador (opcional)

	void Reset(Vec3 p, float yawRad);
	void Step(float dt, const BallInput& in, const CollisionWorld& world);
	void Impulse(Vec3 j) { pendingImpulse += j; }
	Vec3 pendingImpulse;
	Vec3 contactForce;            // F del rozamiento, para el cuadro siguiente
	Vec3 pendingForce;            // F del control táctil (Game::v10), se integra en este paso
	// Estado que usan la cámara y el rumbo (PlayerBall::v02 0x4e808 y v05 0x4f2c8). VERIFICADO con traza:
	//   moving (+0x231): vx^2 + vz^2 > 15
	//   w224 (+0x224): velocidad (menos la de la superficie: PENDIENTE, sin plataformas en la traza) al
	//                  entrar en la respuesta al choque; fast (+0x232): |w224|^2 > 50000 (20260/20260 cuadros)
	//   6 cuadros sin contacto: sin suelo, w224 = 0, fast = false
	bool moving = false, fast = false;
	Vec3 w224;
	int airFrames = 0;
	int surfaceMode = -1;         // bits 0-3 del grupo de la superficie tocada en este paso (-1: ninguna)
	Vec3 Forward() const { return Vec3(sinf(yaw), 0, cosf(yaw)); }
	Vec3 Right() const { return Vec3(cosf(yaw), 0, -sinf(yaw)); }
};
