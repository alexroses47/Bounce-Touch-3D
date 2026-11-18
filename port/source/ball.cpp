// Reimplementación propia del movimiento de la bola (comportamiento descrito en docs/FISICA.md).
#include "ball.h"
#include "rules.h"

void Ball::Reset(Vec3 p, float yawRad)
{
	pos = p;
	vel = Vec3();
	contactForce = Vec3();
	pendingForce = Vec3();
	pendingImpulse = Vec3();
	w224 = Vec3();
	moving = fast = false;
	airFrames = 0;
	yaw = yawRad;
	onGround = false;
	groundNormal = Vec3(0, 1, 0);
	roll = Mat34::Identity();
}

// gira v alrededor del eje Y (misma convención que Mat34::RotY)
static Vec3 rotY(Vec3 v, float a)
{
	float s = sinf(a), c = cosf(a);
	return Vec3(c * v.x + s * v.z, v.y, -s * v.x + c * v.z);
}

void Ball::Step(float dt, const BallInput& input, const CollisionWorld& world)
{
	// PlayerBall::v02: cuadros sin contacto y bola en marcha
	if (++airFrames >= 6) { onGround = false; w224 = Vec3(); fast = false; }
	moving = vel.x * vel.x + vel.z * vel.z > 15.0f;
	surfaceMode = -1;

	Vec3 impulse = pendingImpulse;
	pendingImpulse = Vec3();
	BallInput noInput;
	const BallInput& in = controlEnabled ? input : noInput;
	float push = CONTROL_IMPULSE * (onGround ? GROUND_FACTOR : AIR_FACTOR) * dt;

	// --- control (relativo a la orientación, que es la de la cámara)
	if (in.steer != 0)
	{
		Vec3 flat(vel.x, 0, vel.z);
		float horiz2 = flat.LengthSq();
		float rate = horiz2 > 15.0f ? TURN_FAST : TURN_SLOW;   // girando en marcha, más rápido
		float ang = in.steer * rate * DEG2RAD * dt;
		if (!onGround) ang *= 0.25f;
		ang *= Clamp(1.0f - vel.Length() / (1.5f * maxSpeed), 0, 1);
		vel = rotY(vel, ang);
		yaw += ang * 1.2f;
		Vec3 side = Right() * (in.steer > 0 ? 1.0f : -1.0f);
		if (Dot(vel, side) < formLimit) impulse += side * (push * fabsf(in.steer));
	}
	if (in.forward > 0 && Dot(vel, Forward()) < formLimit)
		impulse += Forward() * (push * in.forward);
	if (in.forward < 0 && Dot(vel, -Forward()) < formLimit * 0.5f)
		impulse += -Forward() * (push * -in.forward);

	// --- salto (forma normal)
	if (in.jumpPressed && onGround && groundNormal.y >= 0.5f)
	{
		vel -= groundNormal * Dot(vel, groundNormal);
		vel *= 0.2f;
		impulse = Vec3();
		float k = Clamp(1.0f - vel.Length() / formLimit, 0, 1);
		Vec3 dir = Vec3(0, 4.2f * (0.25f + 0.75f * k), 0) + Forward() * k;
		impulse = dir.Normalized() * JUMP_IMPULSE;
		onGround = false;
	}

	if (trace) trace->BallState(dt, pos, vel, contactForce + pendingForce, impulse, onGround, 0, maxSpeed, formLimit);

	// --- integración: v += F/m dt + J/m, p += v dt
	vel += (gravity + (contactForce + pendingForce) / MASS) * dt;   // F = g * m + rozamiento + táctil
	contactForce = Vec3();
	pendingForce = Vec3();
	vel += impulse / MASS;
	float speed = vel.Length();
	if (speed > maxSpeed) vel *= maxSpeed / speed;
	pos += vel * dt;

	// --- contactos: separar, rebotar (restitución) y rozamiento
	Contact contacts[8];
	bool anyContact = false;
	Vec3 bestNormal(0, 1, 0);
	touchedCount = 0;
	for (int iter = 0; iter < 3; iter++)
	{
		int n = world.SphereContacts(pos, RADIUS, contacts, 8);
		if (iter == 0) contactsLastStep = n;
		if (n == 0) break;
		// si se toca alguna cara, las aristas y vértices son juntas internas de la malla:
		// se ignoran para que la bola no tropiece al rodar de un triángulo a otro
		bool anyFace = false;
		for (int i = 0; i < n; i++) anyFace |= contacts[i].face;
		if (anyFace)
		{
			int m = 0;
			for (int i = 0; i < n; i++) if (contacts[i].face) contacts[m++] = contacts[i];
			n = m;
		}
		// el más profundo manda en la respuesta de velocidad
		if (iter == 0 && trace) trace->Contacts(vel, restitution, contacts, n, world);
		int best = 0;
		for (int i = 1; i < n; i++) if (contacts[i].depth > contacts[best].depth) best = i;
		for (int i = 0; i < n; i++)
		{
			pos += contacts[i].normal * (contacts[i].depth / n + 0.01f);
			bool known = false;
			for (int k = 0; k < touchedCount; k++) known |= touched[k] == contacts[i].mesh;
			if (!known && touchedCount < 8) touched[touchedCount++] = contacts[i].mesh;
		}
		const Contact& c = contacts[best];
		// respuesta relativa a la superficie (plataformas que se mueven)
		Vec3 surf = world.meshes[c.mesh].velocity;
		if (iter == 0)
		{
			w224 = vel - surf;
			fast = w224.LengthSq() > 50000.0f;
			airFrames = 0;
			surfaceMode = (int)(world.meshes[c.mesh].group & 0xF);
		}
		Vec3 rel = vel - surf;
		float vn = Dot(rel, c.normal);
		if (vn < 0)
		{
			Vec3 vN = c.normal * vn, vT = rel - vN;
			// rebote: Rules::PlayerRestitution (PlayerBall::v05 0x4f2c8 + grupo bit 15)
			rel = vT - vN * Rules::PlayerRestitution(world.meshes[c.mesh].group, restitution);
			vel = rel + surf;
			// rozamiento: fuerza contra la velocidad, se aplica en el cuadro siguiente
			float vlen = vel.Length();
			float normalForce = fmaxf(0.0f, -Dot(gravity, c.normal)) * MASS;
			if (vlen > 0 && dt > 0)
				contactForce = vel * (-fminf(FRICTION * normalForce, MASS * vlen / dt) / vlen);
		}
		anyContact = true;
		bestNormal = c.normal;
	}
	onGround = anyContact;
	if (anyContact) groundNormal = bestNormal;

	// --- giro visual: alrededor de normal x velocidad, ángulo = |v_t| / r * dt
	Vec3 n = onGround ? groundNormal : Vec3(0, 1, 0);
	Vec3 vT = vel - n * Dot(vel, n);
	float vt = vT.Length();
	if (vt > 1.0f)
	{
		Vec3 axis = Cross(n, vT).Normalized();
		roll = Mat34::Rotate(axis, -vt / RADIUS * dt) * roll;
		roll = roll.RotationOnly();
	}
}
