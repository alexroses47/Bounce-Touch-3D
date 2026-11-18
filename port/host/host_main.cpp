// Build host (macOS): el núcleo del juego sin gráficos. Reproduce una secuencia de entradas, un paso por
// fotograma con el dt grabado, y escribe una traza con el formato de tools/eka2l1/bt_ball_trace.lua.
//
//   bt_host --data out/romfs/ --level skybean_level01 --trace port.log [--inputs entradas.txt]
//           [--frames N --dt 0.016667]
//
// Entradas (tools/eka2l1/extract_inputs.py): "frame dt held pressed [tap dedo p2 p3 x y reloj iniX iniY tini ultX ultY]" por línea, held/pressed en hex con
// un bit por acción del original (bit k = acción k de Game::v03 0x2e810: 1 y 2 giran, 3 avanza, 4 frena,
// 5 salta). Las líneas que empiezan por # se ignoran.
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include "../source/game.h"

const char* g_dataRoot = "out/romfs/";
static bool verbose = false;

void logf_(const char* fmt, ...)
{
	if (!verbose) return;
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
}

// --camara-aislada: estado de la bola del original por fotograma (líneas BTB de la traza v2)
struct OrigBall { Vec3 p, v; float incl, yaw, dist; int fast; Vec3 w; int mode; };
static std::vector<OrigBall> origBalls;   // índice = fotograma
static void loadOrigBalls(const char* path)
{
	FILE* f = fopen(path, "r");
	if (!f) { fprintf(stderr, "no puedo abrir %s\n", path); exit(2); }
	static char line[4096];
	std::vector<int> modes;
	while (fgets(line, sizeof(line), f))
	{
		const char* s = strstr(line, "BTB ");
		if (s)
		{
			int n; OrigBall b{};
			float dt;
			if (sscanf(s, "BTB %d dt=%f p=(%f %f %f) v=(%f %f %f)", &n, &dt, &b.p.x, &b.p.y, &b.p.z, &b.v.x, &b.v.y, &b.v.z) != 8) continue;
			const char* c = strstr(s, "cam=(");
			if (!c || sscanf(c, "cam=(%f %f %f) f232=%d w224=(%f %f %f)", &b.incl, &b.yaw, &b.dist, &b.fast, &b.w.x, &b.w.y, &b.w.z) != 7) continue;
			if ((int)origBalls.size() <= n) origBalls.resize(n + 1);
			b.mode = -1;
			origBalls[n] = b;
			continue;
		}
		s = strstr(line, "BTI ");
		if (s)
		{
			int n; const char* m = strstr(s, "modo=");
			if (m && sscanf(s, "BTI %d", &n) == 1)
			{
				if ((int)modes.size() <= n) modes.resize(n + 1, -1);
				modes[n] = atoi(m + 5);
			}
		}
	}
	fclose(f);
	for (size_t n = 0; n < origBalls.size() && n < modes.size(); n++) origBalls[n].mode = modes[n];
}
// tras el paso de la bola del fotograma n, el original tiene el estado del BTB n+1
static void imposeOrigBall(Game& g, void*)
{
	int n = g.trace ? g.trace->frame + 1 : 0;
	if (n <= 0 || n >= (int)origBalls.size()) return;
	const OrigBall& b = origBalls[n];
	if (b.dist == 0) return;
	g.ball.pos = b.p;
	g.ball.vel = b.v;
	g.ball.yaw = b.yaw * DEG2RAD;
	g.ball.fast = b.fast;
	g.ball.w224 = b.w;
	g.level.gs.camPitch = b.incl;
	g.level.gs.camDist = b.dist;
	if (b.mode >= 0) g.level.gs.gameMode = b.mode;
}

struct FrameInput { int frame; float dt; unsigned held, pressed; int tap, finger; TouchInput touch; };

static std::vector<FrameInput> readInputs(const char* path)
{
	std::vector<FrameInput> out;
	FILE* f = fopen(path, "r");
	if (!f) { fprintf(stderr, "no puedo abrir %s\n", path); exit(2); }
	char line[256];
	while (fgets(line, sizeof(line), f))
	{
		if (line[0] == '#' || line[0] == '\n') continue;
		FrameInput in{};
		TouchInput& t = in.touch;
		int n = sscanf(line, "%d %f %x %x %d %d %f %f %f %f %f %f %f %f %f %f", &in.frame, &in.dt, &in.held, &in.pressed,
			&in.tap, &in.finger, &t.p2, &t.p3, &t.x, &t.y, &t.clock, &t.iniX, &t.iniY, &t.tini, &t.ultX, &t.ultY);
		t.active = n >= 6 && in.finger;
		if (n >= 2) out.push_back(in);
	}
	fclose(f);
	return out;
}

static GameInput toGameInput(const FrameInput& fi)
{
	auto on = [](unsigned m, int action) { return (m >> action) & 1; };
	GameInput in;
	in.steer = (float)on(fi.held, 2) - (float)on(fi.held, 1);
	in.forward = (float)on(fi.held, 3) - (float)on(fi.held, 4);
	in.jump = on(fi.pressed, 5) || fi.tap;   // Game::v03: acción 5 pulsada o g+0x3f4
	in.back = on(fi.held, 4);
	in.touch = fi.touch;
	// teclas para waitforkey de los guiones (equivalencia del port, sin evidencia)
	if (on(fi.held, 1)) in.held |= KEY_LEFT;
	if (on(fi.held, 2)) in.held |= KEY_RIGHT;
	if (on(fi.held, 3)) in.held |= KEY_UP;
	if (on(fi.held, 4)) in.held |= KEY_DOWN;
	if (on(fi.held, 5)) in.held |= KEY_A;
	if (on(fi.pressed, 5)) in.down |= KEY_A;
	// waitforkey del original también acepta el toque (g+0x3f4, Trigger.c ejecutor): la tecla "action"
	if (fi.tap) in.down |= KEY_A;
	return in;
}

int main(int argc, char** argv)
{
	std::string level = "skybean_level01", tracePath, inputsPath;
	int frames = 600;
	float fixedDt = 1.0f / 60.0f;
	for (int i = 1; i < argc; i++)
	{
		std::string a = argv[i];
		auto next = [&]() { if (i + 1 >= argc) { fprintf(stderr, "falta valor para %s\n", a.c_str()); exit(2); } return std::string(argv[++i]); };
		if (a == "--data") { static std::string d; d = next(); if (d.back() != '/') d += '/'; g_dataRoot = d.c_str(); }
		else if (a == "--level") level = next();
		else if (a == "--trace") tracePath = next();
		else if (a == "--inputs") inputsPath = next();
		else if (a == "--frames") frames = atoi(next().c_str());
		else if (a == "--dt") fixedDt = atof(next().c_str());
		else if (a == "-v") verbose = true;
		else if (a == "--camara-aislada") loadOrigBalls(next().c_str());
		else { fprintf(stderr, "opción desconocida %s\n", a.c_str()); return 2; }
	}

	std::vector<FrameInput> inputs;
	if (!inputsPath.empty()) inputs = readInputs(inputsPath.c_str());
	else for (int i = 0; i < frames; i++) inputs.push_back({ i + 1, fixedDt, 0, 0 });

	static Game game;   // grande: fuera de la pila
	Trace trace;
	if (!tracePath.empty() && !trace.Open(tracePath.c_str())) { fprintf(stderr, "no puedo escribir %s\n", tracePath.c_str()); return 2; }
	game.trace = &trace;
	if (!origBalls.empty()) game.afterBall = imposeOrigBall;
	if (!game.LoadLevel(level.c_str())) { fprintf(stderr, "no puedo cargar %s desde %s\n", level.c_str(), g_dataRoot); return 1; }

	for (const FrameInput& fi : inputs)
	{
		game.Step(fi.dt, toGameInput(fi));
		game.EndFrame();
	}
	trace.Close();
	fprintf(stderr, "bt_host: %s, %d cuadros, bola en (%.1f %.1f %.1f)\n", level.c_str(), (int)inputs.size(),
		game.ball.pos.x, game.ball.pos.y, game.ball.pos.z);
	return 0;
}
