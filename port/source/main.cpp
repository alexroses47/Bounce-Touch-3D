// Bounce Touch 3DS — frontend de la consola: mandos, dibujo estereoscópico y modo automático.
// El juego (nivel, bola, guiones, cámara) está en game.cpp y se compila también para macOS (host/).
//
// Controles:
//   Pantalla táctil      como el original (Game::v10): deslizar empuja en la dirección del gesto; mantener
//                        quieto en el tercio izquierdo/derecho gira la cámara, en el centro frena; tocar salta
//   Circle Pad / D-pad   control por teclas del original: izquierda-derecha giran; arriba avanza; abajo frena
//   A / B                saltar
//   SELECT               captura a la SD (sdmc:/bt_shot_N.bmp)
//   L / R                nivel anterior / siguiente
//   START                salir
//
// Modo automático (emulador): si existe sdmc:/bt_auto.txt, ejecuta sus líneas en orden:
//   level NOMBRE       carga romfs:/scenes/NOMBRE.bsc
//   wait N             N cuadros sin tocar nada
//   hold TECLAS N      mantiene TECLAS N cuadros (A, B, UP, DOWN, LEFT, RIGHT unidas con +)
//   shot               captura de la pantalla de arriba
//   log                registra posición, velocidad y contactos de la bola
//   view GRADOS DIST ALTO   cámara de prueba alrededor de la bola (view off = la normal)
//   trigger NOMBRE     arranca ese trigger (pruebas)
//   tp NODO            lleva la bola a la posición de un nodo (pruebas)
//   trace              escribe sdmc:/bt_trace.txt (formato de tools/eka2l1, para tools/compare_traces.py)
// Al terminar escribe "FIN" en sdmc:/bt_log.txt y se cierra.

#include <3ds.h>
#include <citro3d.h>
#include <malloc.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>

#include "game.h"
#include "render.h"
#include "scene.h"
#include "vec.h"

#define DISPLAY_TRANSFER_FLAGS \
	(GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) | \
	 GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) | \
	 GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

// Paso de simulación de 1/120 s, dos por cuadro: en EKA2L1 el original se actualiza 120 veces por segundo
// (traza: dt 0,0083) y aplica el empuje táctil como fuerza durante un paso, así que su efecto depende del
// paso; con 1/60 cada empujón pesaba el doble.
constexpr float STEP = 1.0f / 120.0f;
constexpr int STEPS_PER_FRAME = 2;
constexpr int MAX_STEPS = 8;
constexpr float FOV_Y = 55.0f;
constexpr float IOD_MAX = 80.0f;   // como el 8 de celeste64-3ds (foco a 60) con la cámara a 600

// Datos convertidos del .sis de cada usuario: dentro del ejecutable (romfs, build local) o en la SD
// (build público: el .cia/.3dsx no lleva nada del juego).
const char* g_dataRoot = "romfs:/";
static const char* SD_DATA = "sdmc:/3ds/bounce-touch-3d/";

static bool fileExists(const char* path)
{
	FILE* f = fopen(path, "rb");
	if (f) fclose(f);
	return f != nullptr;
}

// ---------------------------------------------------------------- registro
static FILE* logFile;

void logf_(const char* fmt, ...)
{
	char buf[256];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);
	fputs(buf, stderr);   // llega a la Mac por 3dslink
	if (logFile) { fputs(buf, logFile); fflush(logFile); }
}

static Game game;
static Trace autoTrace;

// ---------------------------------------------------------------- táctil
// Pantalla inferior (320x240) -> coordenadas del original (640x360). Anclajes como en la traza EKA2L1:
// al apoyar el dedo, toque inicial = último anclaje = posición; en cada paso p2 = distancia al último
// anclaje y p3 = tiempo desde él; si p2 > 5 (deslizar) el anclaje pasa a la posición actual.
// Salto por toque (g+0x3f4): cuándo lo activa el original no está leído; el port usa soltar antes de
// 0,25 s sin moverse más de 15 px (Port).
struct TouchState { bool down = false; float clock = 0, tUlt = 0; TouchInput t; float moved = 0; };
static TouchState touchState;

static void updateTouch(u32 held, float dt, TouchInput* out, bool* tap)
{
	TouchState& s = touchState;
	s.clock += dt;
	*tap = false;
	if (held & KEY_TOUCH)
	{
		touchPosition tp;
		hidTouchRead(&tp);
		float x = tp.px * 2.0f, y = tp.py * 1.5f;
		if (!s.down)
		{
			s.down = true;
			s.t.iniX = s.t.ultX = x;
			s.t.iniY = s.t.ultY = y;
			s.t.tini = s.tUlt = s.clock;
			s.moved = 0;
		}
		s.t.x = x;
		s.t.y = y;
		s.t.clock = s.clock;
		s.t.p2 = sqrtf((x - s.t.ultX) * (x - s.t.ultX) + (y - s.t.ultY) * (y - s.t.ultY));
		s.t.p3 = s.clock - s.tUlt;
		s.t.active = true;
		s.moved = fmaxf(s.moved, sqrtf((x - s.t.iniX) * (x - s.t.iniX) + (y - s.t.iniY) * (y - s.t.iniY)));
		*out = s.t;
		if (s.t.p2 > 5.0f) { s.t.ultX = x; s.t.ultY = y; s.tUlt = s.clock; }
	}
	else
	{
		if (s.down && s.clock - s.t.tini < 0.25f && s.moved < 15.0f) *tap = true;
		s.down = false;
		out->active = false;
	}
}

// ---------------------------------------------------------------- bola (modelo)
struct BallModel { const Scene* scene; std::vector<DrawItem> parts; std::vector<bool> body; };
static BallModel ballModel;

// de bounce.hgr solo la forma normal: se saltan las ramas B_* que no son B_basic
static bool excludedBranch(const Scene& s, int node)
{
	for (int n = node; n >= 0; n = s.nodes[n].parent)
	{
		const char* nm = s.NodeName(n);
		if (nm[0] == 'B' && nm[1] == '_' && strcmp(nm, "B_basic") != 0) return true;
	}
	return false;
}

static void loadBallModel()
{
	Scene* s = GetLibraryScene("bounce");
	ballModel.scene = s;
	if (!s) return;
	int root = s->FindNode("G_balls_root");
	std::vector<DrawItem> all;
	// se conserva el giro de la raíz (paso de Z-arriba de 3ds Max a Y-arriba), sin su posición
	Mat34 rootRot = root >= 0 ? s->local[root] : Mat34::Identity();
	rootRot.m[0][3] = rootRot.m[1][3] = rootRot.m[2][3] = 0;
	CollectScene(*s, root, rootRot, true, 0, all, nullptr);
	for (auto& it : all)
	{
		if (excludedBranch(*s, it.node)) continue;
		ballModel.parts.push_back(it);
		ballModel.body.push_back(strcmp(s->NodeName(it.node), "bounce") == 0);
	}
	logf_("bola: %u mallas\n", (unsigned)ballModel.parts.size());
}

// ---------------------------------------------------------------- captura
static int shotCounter;

static void saveScreenshot(C3D_RenderTarget* target)
{
	const int W = 400;
	u8* buf = (u8*)linearAlloc(240 * W * 3);
	if (!buf) return;
	C3D_SyncDisplayTransfer((u32*)target->frameBuf.colorBuf, GX_BUFFER_DIM(240, W),
		(u32*)buf, GX_BUFFER_DIM(240, W), DISPLAY_TRANSFER_FLAGS);
	char path[64];
	snprintf(path, sizeof(path), "sdmc:/bt_shot_%d.bmp", shotCounter++);
	FILE* f = fopen(path, "wb");
	if (f)
	{
		u32 dataSize = W * 240 * 3, v;
		u8 head[54] = { 'B', 'M' };
		v = 54 + dataSize; memcpy(head + 2, &v, 4);
		v = 54; memcpy(head + 10, &v, 4);
		v = 40; memcpy(head + 14, &v, 4);
		v = W; memcpy(head + 18, &v, 4);
		v = 240; memcpy(head + 22, &v, 4);
		u16 s = 1; memcpy(head + 26, &s, 2);
		s = 24; memcpy(head + 28, &s, 2);
		memcpy(head + 34, &dataSize, 4);
		fwrite(head, 54, 1, f);
		static u8 row[400 * 3];
		for (int r = 0; r < 240; r++)
		{
			for (int x = 0; x < W; x++) memcpy(&row[x * 3], &buf[(x * 240 + r) * 3], 3);
			fwrite(row, W * 3, 1, f);
		}
		fclose(f);
		logf_("captura %s\n", path);
	}
	linearFree(buf);
}

// ---------------------------------------------------------------- modo automático
struct AutoCmd { std::string op, arg, rest; int frames; u32 keys; };
static std::vector<AutoCmd> autoCmds;
static size_t autoPos = 0;
static int autoLeft = -1;   // cuadros restantes del comando actual

static u32 parseKeys(const std::string& s)
{
	u32 k = 0;
	size_t a = 0;
	while (a < s.size())
	{
		size_t b = s.find('+', a);
		std::string t = s.substr(a, b == std::string::npos ? std::string::npos : b - a);
		if (t == "A") k |= KEY_A;
		else if (t == "B") k |= KEY_B;
		else if (t == "UP") k |= KEY_UP;
		else if (t == "DOWN") k |= KEY_DOWN;
		else if (t == "LEFT") k |= KEY_LEFT;
		else if (t == "RIGHT") k |= KEY_RIGHT;
		if (b == std::string::npos) break;
		a = b + 1;
	}
	return k;
}

static bool loadAutoScript()
{
	FILE* f = fopen("sdmc:/bt_auto.txt", "r");
	if (!f) return false;
	char line[128];
	while (fgets(line, sizeof(line), f))
	{
		char op[32] = "", a1[64] = "", a2[32] = "";
		int n = sscanf(line, "%31s %63s %31s", op, a1, a2);
		if (n < 1) continue;
		const char* rest = strstr(line, a1);
		rest = rest ? rest + strlen(a1) : "";
		AutoCmd c{ op, a1, rest, 0, 0 };
		if (c.op == "wait") c.frames = atoi(a1);
		else if (c.op == "hold") { c.keys = parseKeys(a1); c.frames = atoi(a2); }
		autoCmds.push_back(c);
	}
	fclose(f);
	remove("sdmc:/bt_auto.txt");
	return true;
}

static void logState(float fps)
{
	const Level& level = game.level;
	const Ball& ball = game.ball;
	const Camera& cam = game.cam;
	logf_("cam eye=(%.1f %.1f %.1f) target=(%.1f %.1f %.1f)\n", cam.eye.x, cam.eye.y, cam.eye.z,
		cam.target.x, cam.target.y, cam.target.z);
	logf_("bola p=(%.0f %.0f %.0f) v=(%.0f %.0f %.0f) suelo=%d n=(%.2f %.2f %.2f) contactos=%d | %.1f fps, %lu prims, %lu tris\n",
		ball.pos.x, ball.pos.y, ball.pos.z, ball.vel.x, ball.vel.y, ball.vel.z,
		ball.onGround, ball.groundNormal.x, ball.groundNormal.y, ball.groundNormal.z,
		ball.contactsLastStep, fps, Render::statPrims, Render::statTris);
	int running = 0;
	for (auto& t : level.script.triggers) running += t.state >= 2;
	logf_("guiones: %d triggers en marcha, %lu acciones ejecutadas, modo %d, guía %d, checkpoint %d, vidas %d\n",
		running, level.script.executed, level.gs.gameMode, level.gs.guiding, level.gs.hasCheckpoint, level.gs.lives);
}

// ---------------------------------------------------------------- main
int main()
{
	gfxInitDefault();
	gfxSet3D(true);
	static u32* socBuf = nullptr;
	if (__3dslink_host.s_addr)
	{
		socBuf = (u32*)memalign(0x1000, 0x100000);
		if (socBuf && socInit(socBuf, 0x100000) == 0)
			link3dsConnectToHost(false, true);   // stderr -> Mac
	}
	consoleInit(GFX_BOTTOM, NULL);
	printf("\x1b[8;1H");   // el registro va debajo de las estadísticas
	Result rc = romfsInit();
	bool autoMode = loadAutoScript();
	logFile = fopen("sdmc:/bt_log.txt", "w");   // siempre: diagnóstico en la consola real
	logf_("Bounce Touch 3D - romfs %s (0x%08lX)\n", R_SUCCEEDED(rc) ? "ok" : "ERROR", rc);
	if (!fileExists("romfs:/scenes/skybean_level01.bsc"))
	{
		g_dataRoot = SD_DATA;
		char probe[96];
		snprintf(probe, sizeof(probe), "%sscenes/skybean_level01.bsc", SD_DATA);
		logf_("datos en la SD: %s\n", SD_DATA);
		if (!fileExists(probe))
		{
			printf("\x1b[2J\x1b[1;1HBounce Touch 3D (demo)\n\n"
				"No encuentro los datos del juego.\n\n"
				"Este programa no incluye nada del\njuego original. Convierte tu copia\n"
				"(Bounce Touch v1.00(57).sis) con\ntools/convert_assets.py y copia el\n"
				"resultado en la SD:\n\n  %s\n\n"
				"Instrucciones: github.com/\nalexroses47/Bounce-Touch-3D\n\nPulsa START para salir.", SD_DATA);
			while (aptMainLoop())
			{
				hidScanInput();
				if (hidKeysDown() & KEY_START) break;
				gspWaitForVBlank();
			}
			if (logFile) fclose(logFile);
			romfsExit();
			gfxExit();
			return 0;
		}
	}
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	logf_("citro3d ok, linear libre %lu KB\n", linearSpaceFree() / 1024);

	C3D_RenderTarget* left = C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
	C3D_RenderTarget* right = C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
	C3D_RenderTargetSetOutput(left, GFX_TOP, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);
	C3D_RenderTargetSetOutput(right, GFX_TOP, GFX_RIGHT, DISPLAY_TRANSFER_FLAGS);

	Render::Init();
	logf_("shader ok\n");
	loadBallModel();
	Level& level = game.level;
	Ball& ball = game.ball;
	Camera& cam = game.cam;
	game.LoadLevelIndex(0);

	u64 lastTick = svcGetSystemTick();
	float accum = 0, fps = 0;
	int frame = 0;
	bool wantShot = false;

	while (aptMainLoop())
	{
		hidScanInput();
		u32 down = hidKeysDown(), held = hidKeysHeld();
		circlePosition cp;
		hidCircleRead(&cp);

		// modo automático: sustituye a los mandos
		if (autoMode)
		{
			down = held = 0;
			cp.dx = cp.dy = 0;
			static u32 prevAuto = 0;
			if (autoLeft <= 0)
			{
				while (autoPos < autoCmds.size())
				{
					AutoCmd& c = autoCmds[autoPos++];
					if (c.op == "level") { game.LoadLevel(c.arg.c_str()); continue; }
					if (c.op == "trigger")
					{
						for (size_t i = 0; i < level.script.triggers.size(); i++)
							if (level.script.triggers[i].name == c.arg) { level.script.triggers[i].state = 2; level.script.triggers[i].index = 0; }
						continue;
					}
					if (c.op == "tp")
					{
						int n = level.world.Find(c.arg);
						if (n >= 0) { ball.pos = level.world.nodes[n].world.Pos() + Vec3(0, 100, 0); ball.vel = Vec3(); game.ResetCamera(); }
						continue;
					}
					if (c.op == "trace")
					{
						if (autoTrace.Open("sdmc:/bt_trace.txt")) { game.trace = &autoTrace; ball.trace = &autoTrace; }
						continue;
					}
					if (c.op == "shot") { wantShot = true; break; }   // la captura cierra el cuadro
					if (c.op == "view")
					{
						float y = 0, d = 0, hh = 0;
						game.debugView = sscanf((c.arg + " " + c.rest).c_str(), "%f %f %f", &y, &d, &hh) == 3;
						game.debugYaw = y; game.debugDist = d; game.debugHeight = hh;
						logf_("view %d: %.0f %.0f %.0f\n", game.debugView, y, d, hh);
						game.UpdateCamera(0);
						continue;
					}
					if (c.op == "log") { logState(fps); continue; }
					autoLeft = c.frames;
					break;
				}
				if (autoLeft <= 0 && autoPos >= autoCmds.size() && !wantShot) { logf_("FIN\n"); break; }
			}
			if (autoLeft > 0)
			{
				const AutoCmd& c = autoCmds[autoPos - 1];
				held = c.op == "hold" ? c.keys : 0;
				down = held & ~prevAuto;
				prevAuto = held;
				autoLeft--;
			}
		}

		if (down & KEY_START) break;
		if (down & KEY_SELECT) wantShot = true;
		if (down & (KEY_L | KEY_R)) game.LoadLevelIndex(game.levelIndex + ((down & KEY_R) ? 1 : -1));

		GameInput in;
		in.steer = Clamp(cp.dx / 140.0f, -1, 1);
		in.forward = Clamp(cp.dy / 140.0f, -1, 1);
		if (held & KEY_DLEFT || held & KEY_LEFT) in.steer = -1;
		if (held & KEY_DRIGHT || held & KEY_RIGHT) in.steer = 1;
		if (held & KEY_DUP || held & KEY_UP) in.forward = 1;
		if (held & KEY_DDOWN || held & KEY_DOWN) in.forward = -1;
		if (fabsf(in.steer) < 0.15f) in.steer = 0;
		if (fabsf(in.forward) < 0.15f) in.forward = 0;
		bool jump = down & (KEY_A | KEY_B);
		in.back = (held & KEY_DDOWN) || (held & KEY_DOWN);
		in.held = held;
		in.up = autoMode ? 0 : hidKeysUp();

		// paso fijo de 1/60 s (el modo automático va siempre a paso fijo, para que sea repetible)
		u64 now = svcGetSystemTick();
		float dt = (now - lastTick) / (float)SYSCLOCK_ARM11;
		lastTick = now;
		fps = fps * 0.9f + (dt > 0 ? 0.1f / dt : 0);
		accum += autoMode ? STEP * STEPS_PER_FRAME : fminf(dt, STEP * MAX_STEPS);
		int steps = 0;
		while (accum >= STEP && steps < MAX_STEPS)
		{
			bool tap = false;
			if (!autoMode) updateTouch(held, STEP, &in.touch, &tap);
			in.jump = (jump && steps == 0) || tap;
			in.down = (steps == 0 ? down : 0) | (tap ? KEY_A : 0);   // waitforkey acepta el toque
			game.Step(STEP, in);
			accum -= STEP;
			steps++;
		}
		game.EndFrame();

		// ---- dibujo
		float slider = autoMode ? 0.0f : osGet3DSliderState();   // capturas sin desplazamiento de ojo
		float iod = slider * IOD_MAX;
		C3D_Mtx view, proj;
		Mtx_LookAt(&view, FVec3_New(cam.eye.x, cam.eye.y, cam.eye.z),
			FVec3_New(cam.target.x, cam.target.y, cam.target.z), FVec3_New(0, 1, 0), true);
		Render::statPrims = Render::statTris = 0;
		u32 clear = (level.bgColor << 8) | 0xFF;

		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		for (int eye = 0; eye < (iod > 0 ? 2 : 1); eye++)
		{
			C3D_RenderTarget* t = eye == 0 ? left : right;
			C3D_RenderTargetClear(t, C3D_CLEAR_ALL, clear, 0);
			C3D_FrameDrawOn(t);
			float e = iod == 0 ? 0 : (eye == 0 ? -iod : iod);
			Mtx_PerspStereoTilt(&proj, C3D_AngleFromDegrees(FOV_Y), C3D_AspectRatioTop, 10.0f, 60000.0f,
				e, fmaxf(200.0f, (cam.target - cam.eye).Length()), true);
			Render::Bind();
			Render::SetCamera(proj, view);
			Mat34 skyShift = Mat34::Translate(cam.eye - level.skyCenter);
			Vec3 camFwd = (cam.target - cam.eye).Normalized();
			Vec3 camRight = Cross(Vec3(0, 1, 0), camFwd).Normalized();
			Vec3 camUp = Cross(camFwd, camRight);
			// recorrido de los nodos enganchados y visibles: primero el cielo, luego lo demás
			for (int pass = 0; pass < 2; pass++)
			{
				std::vector<int> stack(level.world.roots.begin(), level.world.roots.end());
				while (!stack.empty())
				{
					int i = stack.back();
					stack.pop_back();
					const RNode& rn = level.world.nodes[i];
					if (!rn.linked || !rn.visible) continue;
					const BscNode& nd = rn.Src();
					if (nd.kind == BSC_MESH && nd.refCount && nd.role != ROLE_COLLISION && rn.sky == (pass == 0))
					{
						Mat34 w = rn.sky ? skyShift * rn.world : (rn.billboard ? BillboardMatrix(rn.world, camRight, camUp) : rn.world);
						rn.scene->DrawMesh(rn.src, w, rn.sky ? Render::PassSky : Render::PassNormal, rn.uvOffset);
					}
					stack.insert(stack.end(), rn.children.begin(), rn.children.end());
				}
			}
			if (ballModel.scene)
			{
				Mat34 face = Mat34::Translate(ball.pos) * Mat34::RotY(ball.yaw);
				Mat34 body = Mat34::Translate(ball.pos) * ball.roll * Mat34::RotY(ball.yaw);
				for (size_t i = 0; i < ballModel.parts.size(); i++)
				{
					const DrawItem& p = ballModel.parts[i];
					p.scene->DrawMesh(p.node, (ballModel.body[i] ? body : face) * p.world, Render::PassNormal);
				}
			}
		}
		if (iod == 0) C3D_RenderTargetClear(right, C3D_CLEAR_ALL, clear, 0);
		C3D_FrameEnd(0);

		if (wantShot) { saveScreenshot(left); wantShot = false; }
		if (frame == 0) logf_("primer cuadro dibujado\n");

		if (++frame % 10 == 0)
		{
			printf("\x1b[1;1H%s\x1b[K\n", LEVELS[game.levelIndex]);
			printf("FPS %4.1f  prims %4lu  tris %5lu\x1b[K\n", fps, Render::statPrims, Render::statTris);
			printf("pos %6.0f %6.0f %6.0f\x1b[K\n", ball.pos.x, ball.pos.y, ball.pos.z);
			printf("vel %6.0f %6.0f %6.0f\x1b[K\n", ball.vel.x, ball.vel.y, ball.vel.z);
			printf("suelo %d  contactos %d  rumbo %4.0f\x1b[K\n", ball.onGround, ball.contactsLastStep, ball.yaw / DEG2RAD);
			int running = 0;
			for (auto& t : level.script.triggers) running += t.state >= 2;
			printf("guiones %2d activos  modo %d  vidas %d\x1b[K\n", running, level.gs.gameMode, level.gs.lives);
			printf("linear libre %lu KB\x1b[K\n", linearSpaceFree() / 1024);
		}
	}

	autoTrace.Close();
	if (logFile) fclose(logFile);
	level.scene.Free();
	FreeLibraryScenes();
	Render::Exit();
	C3D_Fini();
	romfsExit();
	if (socBuf) socExit();
	gfxExit();
	return 0;
}
