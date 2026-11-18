// Núcleo del juego sin gráficos ni mandos: nivel, bola, guiones y cámara. Lo usan el frontend de la
// 3DS (main.cpp) y el build host de macOS (host/host_main.cpp), que reproduce entradas grabadas.
#pragma once
#include <string>
#include "ball.h"
#include "collision.h"
#include "scene.h"
#include "script.h"
#include "trace.h"
#include "vec.h"
#include "world.h"

extern const char* LEVELS[];
extern const int NUM_LEVELS;

struct Level
{
	Scene scene;
	World world;
	CollisionWorld collision;
	Script script;
	GameState gs;
	Vec3 spawn;
	float spawnYaw = 0;
	Vec3 skyCenter;          // centro de la cúpula del cielo: se coloca en la cámara
	u32 bgColor = 0xFFFFFF;
};

// GameCamera (0x35470 v02, 0x3504c v04): ojo y objetivo con muelles escalares
struct Camera
{
	Vec3 eye, target;
	float eyeSpeed = 0, targetSpeed = 0;   // +0xfc, +0x100
	float rest = 0;                        // +0x104: zona muerta del ojo (la escribe PlayerBall::v02)
	bool obstructed = false;               // +0x10e
	Vec3 obstructedPos;                    // +0xe8
	bool init = false;
	// Cámara de espera: mientras corre la animación de espera de la bola (trigger de +0x19c en estado 2),
	// PlayerBall::v13/v12 usan 55° y 350 (0x166a84, 0x166a80) y la mitad del desplazamiento (+75).
	bool idle = false;
	float idleTime = 0;
};

// Control táctil del original (Game::v10 0x2fd08), pantalla de 640x360. Lo rellena el frontend (3DS) o
// la traza grabada (host).
struct TouchInput
{
	bool active = false;     // dedo apoyado: Game::v10 se llama en este cuadro
	float p2 = 0, p3 = 0;    // distancia desde el último anclaje (px) y tiempo desde él (s)
	float x = 0, y = 0;      // posición del dedo (g+0x3ec, g+0x3f0)
	float clock = 0;         // reloj del juego (g+0x3cc)
	float iniX = 0, iniY = 0, tini = 0;   // toque inicial y su momento (Game+0x3c, +0x40, +0x44)
	float ultX = 0, ultY = 0;             // último anclaje (Game+0x48, +0x4c)
};

// Entrada de un paso. Teclado: acciones del original (Game::v03 0x2e810): 1 y 2 giran, 3 avanza, 4 frena,
// 5 salta. steer/forward van de -1 a 1. tap = salto por toque (g+0x3f4).
struct GameInput
{
	float steer = 0, forward = 0;
	bool jump = false;
	bool back = false;       // acción 4 mantenida (PlayerBall::v02 no reorienta el rumbo)
	TouchInput touch;
	u32 down = 0, held = 0, up = 0;   // teclas de la 3DS (waitforkey de los guiones)
};

class Game
{
public:
	Level level;
	Ball ball;
	Camera cam;
	int levelIndex = 0;
	Trace* trace = nullptr;
	// Pruebas (build host): se llama tras el paso de la bola y antes de la cámara; permite imponer el
	// estado de la bola del juego original para comparar la cámara aislada (bt_host --camara-aislada).
	void (*afterBall)(Game& game, void* ctx) = nullptr;
	void* afterBallCtx = nullptr;

	// vista de prueba alrededor de la bola (modo automático "view")
	bool debugView = false;
	float debugYaw = 0, debugDist = 0, debugHeight = 0;

	bool LoadLevel(const char* name);
	bool LoadLevelIndex(int index) { levelIndex = (index % NUM_LEVELS + NUM_LEVELS) % NUM_LEVELS; return LoadLevel(LEVELS[levelIndex]); }
	// un paso de simulación (guiones, colisión, bola, cámara)
	void Step(float dt, const GameInput& in);
	// después de los pasos de un cuadro: caídas, nivel fallido o completado, textos
	void EndFrame();
	void ResetCamera() { cam.init = false; UpdateCamera(0); }
	void UpdateCamera(float dt);
	// base de la cámara: derecha, arriba, adelante (Mtx_LookAt de la 3DS usa la misma)
	void CameraBasis(Vec3* right, Vec3* up, Vec3* fwd) const;

private:
	Vec3 settleOnGround(Vec3 p);
	void touchControl(const TouchInput& t, float dt);
	void ballHeading(bool back);
	void desiredView(Vec3* eye, Vec3* target, bool* fromBall) const;
	void cameraSprings(float dt);
	bool cameraObstruction();
};
