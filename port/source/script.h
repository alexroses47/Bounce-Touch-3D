// Intérprete de los guiones de nivel (triggers y acciones). Comportamiento: docs/GUIONES.md.
#pragma once
#include "platform.h"
#include <string>
#include <unordered_map>
#include <vector>
#include "vec.h"

struct World;
struct CollisionWorld;
struct Ball;

// Lo que los guiones cambian fuera de sí mismos (cámara, bola, partida).
struct GameState
{
	Ball* ball = nullptr;
	int gameMode = 0;
	int guiding = -1;                 // nodo guía de la cámara (-1 = la bola)
	float camPitch = 55, camDist = 555, camStiffness = 27.5f, camDamping = 5.5f;
	// setcamprops (ejecutor FUN_000115d4 caso 0x21): muelles del ojo (+0x10c) y del objetivo (+0x10d) de
	// GameCamera; con ellos apagados la cámara salta al punto deseado
	bool eyeSpring = true, targetSpring = true;
	// FUN_0002e29c(juego, modo, forzar): no cambia el modo si es 3, 4 o 5 y no se fuerza; setgamemode
	// fuerza, el contacto con una superficie no (PlayerBall::v05)
	void SetGameMode(int mode, bool force)
	{
		if ((gameMode == 3 || gameMode == 4 || gameMode == 5) && !force) return;
		gameMode = mode;
	}
	bool hasCheckpoint = false;
	Vec3 checkpointPos;
	float checkpointYaw = 0;
	bool levelFailed = false, levelComplete = false;
	std::string loadLevel;
	std::string text;                 // clave de showtext pendiente de mostrar
	int lives = 3, score = 0;
	u32 downKeys = 0, heldKeys = 0, upKeys = 0;   // (libctru tiene macros keysDown...)
	float idleTime = 0;
	// waitforidle (ejecutor FUN_000115d4 caso 0x38): cuenta solo si han pasado > 0,25 s desde la última
	// entrada (g+0x3cc - g+0x3d0) y la bola está quieta (+0x231 == 0 y |vy| <= 50); si no, vuelve a 0
	float sinceInput = 0;
	bool ballStill = false;
};

struct Action
{
	int verb = -1;
	std::vector<std::string> args;
	// estado de ejecución
	bool started = false;
	float timer = 0;
	int counter = 0;
	int mover = -1;
};

struct Trigger
{
	int node;
	std::string name;
	int instance;
	int state = 1;                    // 0 desactivado, 1 esperando, 2 acciones, 3 salida
	std::vector<Action> actions, exits;
	int index = 0, exitIndex = 0;
	int volume = 0;                   // 0 ninguno, 1 OBB, 2 esfera
	bool onExit = false;              // activatetype = 1: salta al salir del volumen (T_universal_death)
	bool once = false;                // executetype = 0: se ejecuta una sola vez
	float radius = 0;
};

struct Mover
{
	int node;
	int kind;                         // 0 mover, 1 girar, 2 UV
	Vec3 rate;                        // UV: (du, dv) por segundo
	Vec3 axis;                        // mover: dirección unitaria; girar: eje (ambos de Rules::ActionVector)
	float speed = 0;                  // mover: unidades/s; girar: radianes/s (FUN_0003ae40: distancia / tiempo)
	bool worldAxes = false;           // flag `ejes`
	float remaining = 0;
	bool done = false;
	bool waited = false;              // una acción espera a que termine
};

class Script
{
public:
	void Build(World& world, CollisionWorld& collision);
	void Clear();
	// avanza los triggers activos y los movimientos un paso
	void Update(float dt, GameState& gs);
	// contactos de la bola de este paso: volúmenes de triggers y mallas con executetrigger
	void BallContacts(Vec3 ballPos, float radius, const int* meshes, int meshCount);

	std::unordered_map<std::string, float> vars;
	std::vector<Trigger> triggers;
	std::vector<Mover> movers;
	u32 executed = 0;                 // acciones ejecutadas (estadística)

private:
	World* world = nullptr;
	CollisionWorld* collision = nullptr;
	std::vector<int> meshTrigger;     // por malla de colisión: trigger de executetrigger (-1)

	int findTrigger(const std::string& name, int scope) const;
	int resolveNode(const std::string& name, int scope) const;
	void start(int t);
	bool exec(Trigger& t, Action& a, float dt, GameState& gs, bool& jumped);
	float value(const std::string& s) const;
	Vec3 toParentSpace(int node, Vec3 worldVec) const;
	void applyMover(const Mover& mv, float step);
	bool touchesVolume(const Trigger& t, Vec3 p, float r) const;
};
