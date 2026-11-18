#include "game.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

const char* LEVELS[] = { "skybean_level01", "skybean_level02", "skybean_level03", "skybean_level05",
	"skybean_level07", "skybean_level06", "skybean_level04", "skybean_level08" };   // orden del mapa
const int NUM_LEVELS = sizeof(LEVELS) / sizeof(LEVELS[0]);

bool Game::LoadLevel(const char* name)
{
	level.script.Clear();
	level.collision.Clear();
	level.world.Clear();
	if (!level.scene.Load(name)) return false;
	level.world.Build(level.scene);
	level.collision.Build(level.world);
	level.script.Build(level.world, level.collision);
	level.gs = GameState();
	level.gs.ball = &ball;
	ball.controlEnabled = true;
	ball.maxSpeed = 2000.0f;
	ball.gravity = Vec3(0, -2000.0f, 0);
	ball.trace = trace;

	// el cielo sigue a la cámara: centro de las mallas de cielo
	Vec3 mn(1e9f, 1e9f, 1e9f), mx(-1e9f, -1e9f, -1e9f);
	for (auto& rn : level.world.nodes)
	{
		if (!rn.sky || rn.Src().kind != BSC_MESH) continue;
		const BscNode& nd = rn.Src();
		for (u32 r = 0; r < nd.refCount; r++)
		{
			const BscPrimitive& p = rn.scene->prims[rn.scene->refs[nd.firstRef + r]];
			for (int c = 0; c < 8; c++)
			{
				Vec3 v((c & 1) ? p.bboxMax[0] : p.bboxMin[0], (c & 2) ? p.bboxMax[1] : p.bboxMin[1], (c & 4) ? p.bboxMax[2] : p.bboxMin[2]);
				Vec3 w = rn.world.Apply(v);
				mn = Vec3(fminf(mn.x, w.x), fminf(mn.y, w.y), fminf(mn.z, w.z));
				mx = Vec3(fmaxf(mx.x, w.x), fmaxf(mx.y, w.y), fmaxf(mx.z, w.z));
			}
		}
	}
	level.skyCenter = mn.x < mx.x ? (mn + mx) * 0.5f : Vec3();

	int start = level.world.Find("D_start");
	if (start >= 0)
	{
		const Mat34& w = level.world.nodes[start].world;
		level.spawn = w.Pos();
		Vec3 f = w.Col(1);   // el eje Y local del dummy apunta hacia el camino (ver docs)
		level.spawnYaw = atan2f(f.x, f.z);
		std::string bg;
		if (level.world.Prop(start, "bgcolor", &bg)) level.bgColor = strtoul(bg.c_str(), nullptr, 16);
	}
	logf_("nivel %s: %u nodos, %u mallas de colisión, salida (%.0f %.0f %.0f) rumbo %.0f°\n",
		name, (unsigned)level.world.nodes.size(), (unsigned)level.collision.meshes.size(),
		level.spawn.x, level.spawn.y, level.spawn.z, level.spawnYaw / DEG2RAD);
	// La bola aparece en D_start y cae (traza EKA2L1 del nivel 1: primer cuadro en D_start exacto, cae
	// 260 unidades hasta el suelo). Antes el port la apoyaba en el suelo; era una invención.
	ball.Reset(level.spawn, level.spawnYaw);
	ResetCamera();
	return true;
}

// apoya la bola en el suelo bajo `p` (si hay suelo en 3000 unidades). Solo al reaparecer (Port: sin evidencia)
Vec3 Game::settleOnGround(Vec3 p)
{
	Contact c[2];
	for (float d = 0; d < 3000; d += 8)
	{
		Vec3 q = p - Vec3(0, d, 0);
		if (level.collision.SphereContacts(q, Ball::RADIUS, c, 2) > 0) return q + Vec3(0, 8, 0);
	}
	return p;
}

// Base de la cámara (derecha, arriba, adelante) a partir del ojo y el objetivo, con el arriba del mundo
// como GameCamera (FUN_0008a40c con (0,1,0)).
void Game::CameraBasis(Vec3* right, Vec3* up, Vec3* fwd) const
{
	Vec3 f = (cam.target - cam.eye).Normalized();
	Vec3 r = Cross(Vec3(0, 1, 0), f).Normalized();
	*fwd = f;
	*right = r;
	*up = Cross(f, r);
}

// Puntos deseados de la cámara. Con la bola de fuente (PlayerBall::v13 / v12):
//   ojo = bola + R_y(rumbo) * (0, sen(incl)*dist, -h), h = cos(incl)*dist, reducido por
//         (1 - min(|w224|/lim, 1)) si la bola va rápida (+0x232)
//   objetivo = bola + (0, 150, 0) (desplazamiento global 0x400150, traza)
// VERIFICADO con traza EKA2L1: error mediano 0,001-0,005 u por paso. La fuente es el objeto guía
// (setguidingobject) en modo 3 y la bola en los demás (traza: sin excepción en 20260 cuadros); de la guía
// se usan los nodos cameraposition / cameralookat (PENDIENTE: lectura de las fuentes guía).
void Game::desiredView(Vec3* eye, Vec3* target, bool* fromBall) const
{
	const GameState& gs = level.gs;
	if (gs.guiding >= 0 && gs.gameMode == 3)
	{
		int p = level.world.FindBelow(gs.guiding, "cameraposition");
		int l = level.world.FindBelow(gs.guiding, "cameralookat");
		if (p >= 0)
		{
			*eye = level.world.nodes[p].world.Pos();
			*target = l >= 0 ? level.world.nodes[l].world.Pos() : level.world.nodes[gs.guiding].world.Pos();
			*fromBall = false;
			return;
		}
	}
	float incl = (cam.idle ? 55.0f : gs.camPitch) * DEG2RAD, dist = cam.idle ? 350.0f : gs.camDist;
	float h = cosf(incl) * dist;
	if (ball.fast) h *= 1.0f - fminf(ball.w224.Length() / ball.formLimit, 1.0f);
	*eye = ball.pos + Vec3(-h * sinf(ball.yaw), sinf(incl) * dist, -h * cosf(ball.yaw));
	*target = ball.pos + Vec3(0, cam.idle ? 75.0f : 150.0f, 0);
	*fromBall = true;
}

// GameCamera::v02: muelle escalar hacia el punto deseado (o el de obstáculo, sin zona muerta).
// s += (k * exceso - c * s) * dt; posición += dirección * s * dt. Sin muelle (setcamprops 0) salta.
void Game::cameraSprings(float dt)
{
	const GameState& gs = level.gs;
	Vec3 eye, target;
	bool fromBall;
	desiredView(&eye, &target, &fromBall);
	float k = gs.camStiffness, c = gs.camDamping;
	if (!gs.eyeSpring) { cam.eye = eye; cam.eyeSpeed = 0; }
	else
	{
		Vec3 goal = cam.obstructed ? cam.obstructedPos : eye;
		float rest = cam.obstructed ? 0.0f : cam.rest;
		Vec3 d = goal - cam.eye;
		float len = d.Length();
		float excess = len > rest ? len - rest : 0.0f;
		cam.eyeSpeed += (k * excess - c * cam.eyeSpeed) * dt;
		if (len > 0) cam.eye += d * (cam.eyeSpeed * dt / len);
	}
	if (!gs.targetSpring) { cam.target = target; cam.targetSpeed = 0; }
	else
	{
		Vec3 d = target - cam.target;
		float len = d.Length();
		cam.targetSpeed += (k * len - c * cam.targetSpeed) * dt;
		if (len > 0) cam.target += d * (cam.targetSpeed * dt / len);
	}
}

// GameCamera::v04 (fuera de escenas): rayo del objetivo - (0,75,0) al ojo deseado; si choca, el ojo va
// al choque + 35 * normal, y si eso queda a menos de 160 de la bola, a 160 de ella en esa dirección.
// Lectura del binario; orientación de la normal (hacia el origen del rayo): PENDIENTE.
// DESACTIVADO: en la traza EKA2L1 el original no registró ningún obstáculo en 20260 cuadros (+0x10e
// siempre 0) aunque la bola se pegó a paredes; FUN_00026640 debe de probar solo algunos objetos (el nivel
// tiene node03_cameracol). PENDIENTE de leer qué objetos; hasta entonces el rayo del port no se usa.
static constexpr bool CAMERA_OBSTRUCTION = false;

bool Game::cameraObstruction()
{
	cam.obstructed = false;
	if (!CAMERA_OBSTRUCTION || level.gs.gameMode == 3) return false;
	Vec3 eye, target;
	bool fromBall;
	desiredView(&eye, &target, &fromBall);
	Vec3 hit, n;
	if (!level.collision.RayCast(cam.target - Vec3(0, 75.0f, 0), eye, &hit, &n)) return false;
	Vec3 pos = hit + n.Normalized() * 35.0f;
	Vec3 away = hit - ball.pos;
	if (away.LengthSq() < 160.0f * 160.0f) pos = ball.pos + away.Normalized() * 160.0f;
	cam.obstructed = true;
	cam.obstructedPos = pos;
	return true;
}

void Game::UpdateCamera(float dt)
{
	const Ball& b = ball;
	if (debugView)
	{
		float a = b.yaw + debugYaw * DEG2RAD;
		cam.target = b.pos;
		cam.eye = b.pos + Vec3(sinf(a), 0, cosf(a)) * debugDist + Vec3(0, debugHeight, 0);
		cam.init = true;
		return;
	}
	if (!cam.init || dt <= 0)
	{
		bool fromBall;
		desiredView(&cam.eye, &cam.target, &fromBall);
		cam.eyeSpeed = cam.targetSpeed = 0;
		cam.obstructed = false;
		cam.init = true;
		return;
	}
	cameraSprings(dt);                          // v02
	if (cameraObstruction()) cameraSprings(dt);  // v04 vuelve a llamar a v02 si hay obstáculo
}

// Game::v10 (0x2fd08): control táctil, pantalla de 640x360 (traza: x 13..639, y 0..359).
void Game::touchControl(const TouchInput& t, float dt)
{
	if (!t.active) return;
	float airScale = ball.onGround ? 1.0f : 0.25f;   // +0x108 suelo (el indicador global 0x40014c: PENDIENTE)
	if (t.p2 > 5.0f)
	{
		// deslizar: dirección del gesto en la base de la cámara, en horizontal; fuerza
		// clamp(p2/p3, 600, 2000) * 6,5 si la bola no va ya a más de +0x150 en esa dirección.
		// VERIFICADO con traza (cuadro 1087: 39,05 frente a 39,08; dirección exacta).
		Vec3 r, u, f;
		CameraBasis(&r, &u, &f);
		Vec3 w = r * (t.x - t.ultX) + f * (-(t.y - t.ultY));
		w.y = 0;
		float len = w.Length();
		if (len <= 0) return;
		w = w / len;
		float speed = t.p3 > 0 ? Clamp(t.p2 / t.p3, 600.0f, 2000.0f) * 6.5f : 2000.0f * 6.5f;
		if (Dot(ball.vel, w) < ball.formLimit) ball.pendingForce += w * (speed * airScale);
		return;
	}
	// mantener quieto (>= 0,1 s desde el toque, a menos de 15 px de él)
	if (t.clock - t.tini < 0.1f) return;
	float dx = t.x - t.iniX, dy = t.y - t.iniY;
	if (dx * dx + dy * dy >= 15.0f * 15.0f) return;
	const float W = 640.0f, third = W / 3.0f;
	float rate = 0;
	if (t.x < third) { if (level.gs.gameMode == 0) rate = -90.0f; }
	else if (t.x > W - third) { if (level.gs.gameMode == 0) rate = 90.0f; }
	else if (ball.onGround)
	{
		// centro: frena (F -= 1200 * v unitaria, 0x166abc)
		float vl = ball.vel.Length();
		if (vl > 0) ball.pendingForce += ball.vel * (-1200.0f / vl);
	}
	if (rate != 0) ball.yaw += rate * dt * airScale * 1.2f * DEG2RAD;
}

// PlayerBall::v02 (0x4e808): reorienta el rumbo de la cámara cuando la bola se mueve (+0x231).
// Modo 1: rumbo = atan2(vx, vz) si vx^2+vz^2 > 60000 (traza: 0,0003° de error).
// Modo 0 con la bola rápida (+0x232): rumbo = el de la cámara real del cuadro anterior (traza: 0,0002°).
// Escribe además la zona muerta de la cámara: h completo - h reducido.
void Game::ballHeading(bool back)
{
	const GameState& gs = level.gs;
	float incl = (cam.idle ? 55.0f : gs.camPitch) * DEG2RAD, dist = cam.idle ? 350.0f : gs.camDist;
	float h = cosf(incl) * dist, hs = h;
	if (ball.fast) hs *= 1.0f - fminf(ball.w224.Length() / ball.formLimit, 1.0f);
	cam.rest = h - hs;
	if (!ball.moving || back) return;
	if (gs.gameMode == 1)
	{
		if (ball.vel.x * ball.vel.x + ball.vel.z * ball.vel.z > 60000.0f) ball.yaw = atan2f(ball.vel.x, ball.vel.z);
	}
	else if (gs.gameMode == 0 && ball.fast && cam.init)
	{
		Vec3 d = cam.target - cam.eye;
		ball.yaw = atan2f(d.x, d.z);
	}
}

void Game::Step(float dt, const GameInput& input)
{
	GameState& gs = level.gs;
	gs.downKeys = input.down;
	gs.heldKeys = input.held;
	gs.upKeys = input.up;
	BallInput in;
	in.steer = input.steer;
	in.forward = input.forward;
	in.jumpPressed = input.jump;
	bool anyInput = in.steer != 0 || in.forward != 0 || in.jumpPressed || input.touch.active;
	gs.idleTime = anyInput ? 0 : gs.idleTime + dt;
	level.script.Update(dt, gs);
	level.world.UpdateTransforms();
	level.collision.Update(level.world, dt);
	// Orden del original en un cuadro: Game::v03 (teclado), Game::v10 (táctil), PlayerBall::v02 (rumbo),
	// v03 (integración), v05 (choque), GameCamera::v02 y v04 (traza: BTI, BTT, BTB, BTC, BTK).
	// En modo 3 el teclado no actúa (FUN_0002e29c pone Game+0x158 = 0); el salto por toque sí.
	BallInput keys = in;
	if (gs.gameMode == 3) { keys.steer = keys.forward = 0; }
	// Animación de espera de la bola (bounce.hgr: T_idlecontrol "waitforidle,5" -> T_B_idle; T_idleoff
	// "waitforkey,any" la termina). El port aún no ejecuta los guiones de bounce.hgr: se reproduce aquí.
	// Traza: la cámara de espera empieza ~5,7 s después del último toque (5 s + muelle) y acaba al tocar.
	bool anyAction = anyInput || input.held || input.down;
	gs.sinceInput = anyAction ? 0 : gs.sinceInput + dt;
	gs.ballStill = !ball.moving && fabsf(ball.vel.y) <= 50.0f;
	if (anyAction || gs.gameMode == 3) { cam.idleTime = 0; cam.idle = false; }
	else
	{
		if (gs.sinceInput > 0.25f && gs.ballStill) cam.idleTime += dt;   // misma cuenta que waitforidle
		else if (!cam.idle) cam.idleTime = 0;
		if (cam.idleTime >= 5.0f) cam.idle = true;
	}
	touchControl(input.touch, dt);
	ballHeading(input.back);
	// La física de la bola corre en todos los modos: PlayerBall::v03 se ejecuta en cada fotograma
	// (traza EKA2L1: 9930 de 9930, con escenas; la bola cae desde el cuadro 2 durante la intro).
	ball.Step(dt, keys, level.collision);
	if (ball.surfaceMode == 0) gs.SetGameMode(0, false);       // PlayerBall::v05: modo de la superficie
	else if (ball.surfaceMode == 2) gs.SetGameMode(1, false);
	else if (ball.surfaceMode == 3) gs.SetGameMode(2, false);
	level.script.BallContacts(ball.pos, Ball::RADIUS, ball.touched, ball.touchedCount);
	if (afterBall) afterBall(*this, afterBallCtx);
	UpdateCamera(dt);
	if (trace) trace->Camera(cam.eye, cam.target, cam.eyeSpeed, cam.targetSpeed);
	gs.downKeys = gs.upKeys = 0;
}

void Game::EndFrame()
{
	GameState& gs = level.gs;
	if (ball.pos.y < level.collision.boundsMin.y - 1500)   // caída al vacío
	{
		logf_("caída al vacío\n");
		gs.levelFailed = true;
	}
	if (gs.levelFailed)
	{
		gs.levelFailed = false;
		gs.lives--;
		Vec3 p = gs.hasCheckpoint ? gs.checkpointPos : level.spawn;
		float y = gs.hasCheckpoint ? gs.checkpointYaw : level.spawnYaw;
		logf_("nivel fallido: reaparece en %s (%.0f %.0f %.0f)\n", gs.hasCheckpoint ? "checkpoint" : "salida", p.x, p.y, p.z);
		ball.Reset(settleOnGround(p), y);
		gs.guiding = -1;
		gs.gameMode = 0;
		ball.controlEnabled = true;
		ResetCamera();
	}
	if (gs.levelComplete || !gs.loadLevel.empty())
	{
		logf_("nivel completado\n");
		LoadLevelIndex(levelIndex + 1);
	}
	if (!gs.text.empty())
	{
		logf_("texto: %s\n", gs.text.c_str());
		gs.text.clear();
	}
}
