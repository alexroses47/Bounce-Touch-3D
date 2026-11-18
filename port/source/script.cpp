// Reimplementación propia del intérprete de guiones (comportamiento en docs/GUIONES.md).
#include "script.h"
#include "platform.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include "ball.h"
#include "collision.h"
#include "rules.h"
#include "world.h"

void logf_(const char* fmt, ...);

// orden de la tabla de verbos del juego
enum Verb
{
	V_ACTIVATERANDOMTRIGGER, V_ACTIVATETRIGGER, V_ADDIMPULSE, V_ADDLIFE, V_ADDSCORE, V_ADDSPEED, V_BREAK,
	V_CHANGEBALL, V_CHECKPOINT, V_COUNTER, V_DELAY, V_DISABLETRIGGER, V_ENABLETRIGGER, V_FINALIZETRIGGER,
	V_JUMPTOACTION, V_JUMPTORANDOMACTION, V_LEVELCOMPLETE, V_LEVELFAILED, V_LINK, V_LOADLEVEL, V_MAXSPEED,
	V_MOVEOBJECT, V_MOVEUV, V_NOP, V_PARTICLEFX, V_PAUSETRIGGER, V_PLAYANIMATION, V_PLAYSOUND, V_PRESSURE,
	V_RANDOMDELAY, V_ROTATEOBJECT, V_SETBALLDIRECTION, V_SETCAMERAVIEW, V_SETCAMPROPS, V_SETGAMEMODE,
	V_SETGRAVITY, V_SETGUIDINGOBJECT, V_SETOBJECTPOS, V_SETOBJECTSTATE, V_SETRENDERSTATE, V_SHOWTEXT,
	V_SPEEDMUL, V_STOPSOUND, V_SWITCHOBJECTSTATE, V_SWITCHRENDERSTATE, V_TIMER, V_UNLINK, V_SET, V_ADD,
	V_SUB, V_MUL, V_DIV, V_LESS, V_EQUAL, V_GREATER, V_NOTEQUAL, V_WAITFORIDLE, V_WAITFORKEY,
	V_WAITFORKEYHOLD, V_WAITFORKEYPRESS, V_WAITFORKEYRELEASE, V_COUNT
};
static const char* VERBS[V_COUNT] = {
	"activaterandomtrigger", "activatetrigger", "addimpulse", "addlife", "addscore", "addspeed", "break",
	"changeball", "checkpoint", "counter", "delay", "disabletrigger", "enabletrigger", "finalizetrigger",
	"jumptoaction", "jumptorandomaction", "levelcomplete", "levelfailed", "link", "loadlevel", "maxspeed",
	"moveobject", "moveuv", "nop", "particlefx", "pausetrigger", "playanimation", "playsound", "pressure",
	"randomdelay", "rotateobject", "setballdirection", "setcameraview", "setcamprops", "setgamemode",
	"setgravity", "setguidingobject", "setobjectpos", "setobjectstate", "setrenderstate", "showtext",
	"speedmul", "stopsound", "switchobjectstate", "switchrenderstate", "timer", "unlink", "set", "add",
	"sub", "mul", "div", "less", "equal", "greater", "notequal", "waitforidle", "waitforkey",
	"waitforkeyhold", "waitforkeypress", "waitforkeyrelease",
};

static int verbIndex(const std::string& v)
{
	for (int i = 0; i < V_COUNT; i++)
		if (strcasecmp(v.c_str(), VERBS[i]) == 0) return i;
	return -1;
}

static std::vector<std::string> split(const std::string& s)
{
	std::vector<std::string> out;
	size_t a = 0;
	while (true)
	{
		size_t b = s.find(',', a);
		std::string t = s.substr(a, b == std::string::npos ? std::string::npos : b - a);
		size_t x = 0, y = t.size();
		while (x < y && isspace((unsigned char)t[x])) x++;
		while (y > x && isspace((unsigned char)t[y - 1])) y--;
		out.push_back(t.substr(x, y - x));
		if (b == std::string::npos) break;
		a = b + 1;
	}
	return out;
}

static float frand(float a, float b) { return a + (b - a) * (rand() / (float)RAND_MAX); }

// ---------------------------------------------------------------- construcción

void Script::Clear()
{
	vars.clear();
	triggers.clear();
	movers.clear();
	meshTrigger.clear();
	executed = 0;
}

void Script::Build(World& w, CollisionWorld& c)
{
	Clear();
	world = &w;
	collision = &c;
	for (size_t n = 0; n < w.nodes.size(); n++)
	{
		const RNode& rn = w.nodes[n];
		auto props = rn.scene->Props(rn.src);
		Trigger t;
		t.node = n;
		t.name = rn.Name();
		t.instance = rn.instance;
		std::vector<std::pair<int, Action>> acts, exits;
		bool isTrigger = false;
		for (auto& kv : props)
		{
			const std::string& k = kv.first;
			if (k.size() >= 2 && (k[0] == 'a' || k[0] == 'e') && isdigit((unsigned char)k[1]))
			{
				auto parts = split(kv.second);
				Action a;
				a.verb = verbIndex(parts[0]);
				if (a.verb < 0) { logf_("verbo desconocido '%s' en %s\n", parts[0].c_str(), t.name.c_str()); continue; }
				a.args.assign(parts.begin() + 1, parts.end());
				(k[0] == 'a' ? acts : exits).push_back({ atoi(k.c_str() + 1), a });
				isTrigger = true;
			}
			else if (strcasecmp(k.c_str(), "state") == 0) { t.state = atoi(kv.second.c_str()); isTrigger = true; }
			else if (strcasecmp(k.c_str(), "collisiontype") == 0)
			{
				t.volume = strcasecmp(kv.second.c_str(), "SPHERE") == 0 ? 2 : 1;
				isTrigger = true;
			}
			else if (strcasecmp(k.c_str(), "radius") == 0) t.radius = atof(kv.second.c_str());
			else if (strcasecmp(k.c_str(), "activatetype") == 0) t.onExit = atoi(kv.second.c_str()) == 1;
			else if (strcasecmp(k.c_str(), "executetype") == 0) t.once = atoi(kv.second.c_str()) == 0;
		}
		if (!isTrigger) continue;
		auto byNum = [](const std::pair<int, Action>& a, const std::pair<int, Action>& b) { return a.first < b.first; };
		std::stable_sort(acts.begin(), acts.end(), byNum);
		std::stable_sort(exits.begin(), exits.end(), byNum);
		for (auto& a : acts) t.actions.push_back(a.second);
		for (auto& a : exits) t.exits.push_back(a.second);
		if (t.state == 2) t.index = 0;
		triggers.push_back(t);
	}
	// mallas que disparan un trigger al tocarlas
	meshTrigger.assign(c.meshes.size(), -1);
	for (size_t m = 0; m < c.meshes.size(); m++)
	{
		int node = c.meshes[m].node;
		std::string t;
		if (w.Prop(node, "executetrigger", &t) || w.Prop(node, "executelibrarytrigger", &t))
			meshTrigger[m] = findTrigger(t, w.nodes[node].instance);
	}
	int autostart = 0;
	for (auto& t : triggers) autostart += t.state == 2;
	logf_("guiones: %u triggers (%d arrancan solos)\n", (unsigned)triggers.size(), autostart);
}

int Script::findTrigger(const std::string& name, int scope) const
{
	int any = -1;
	for (size_t i = 0; i < triggers.size(); i++)
		if (triggers[i].name == name)
		{
			if (triggers[i].instance == scope) return i;
			if (any < 0 || triggers[i].instance == 0) any = i;
		}
	return any;
}

int Script::resolveNode(const std::string& name, int scope) const
{
	if (name.empty() || name == "this") return -1;
	return world->Find(name, scope);
}

void Script::start(int t)
{
	if (t < 0) return;
	Trigger& tr = triggers[t];
	tr.state = 2;
	tr.index = 0;
	tr.exitIndex = 0;
	for (auto& a : tr.actions) a.started = false;
	for (auto& a : tr.exits) a.started = false;
}

float Script::value(const std::string& s) const
{
	char* end;
	float v = strtof(s.c_str(), &end);
	if (end != s.c_str() && *end == 0) return v;
	auto it = vars.find(s);
	return it != vars.end() ? it->second : 0.0f;
}

// vector del mundo -> espacio del padre del nodo (padres con ejes ortogonales)
Vec3 Script::toParentSpace(int node, Vec3 v) const
{
	int p = world->nodes[node].parent;
	if (p < 0) return v;
	const Mat34& m = world->nodes[p].world;
	Vec3 x = m.Col(0), y = m.Col(1), z = m.Col(2);
	return Vec3(Dot(v, x) / Dot(x, x), Dot(v, y) / Dot(y, y), Dot(v, z) / Dot(z, z));
}

bool Script::touchesVolume(const Trigger& t, Vec3 p, float r) const
{
	const RNode& rn = world->nodes[t.node];
	const BscNode& nd = rn.Src();
	if (nd.kind != BSC_DUMMY) return false;
	const Mat34& m = rn.world;
	Vec3 mn(nd.params[0], nd.params[1], nd.params[2]), mx(nd.params[3], nd.params[4], nd.params[5]);
	if (t.volume == 2)
	{
		Vec3 c = m.Apply((mn + mx) * 0.5f);
		float rad = t.radius > 0 ? t.radius : (mx - mn).Length() * 0.5f * m.Col(0).Length() / 1.7320508f;
		return (p - c).Length() < rad + r;
	}
	// OBB: punto de la caja más cercano (en local) y distancia en el mundo
	Vec3 d = p - m.Pos();
	Vec3 x = m.Col(0), y = m.Col(1), z = m.Col(2);
	Vec3 l(Dot(d, x) / Dot(x, x), Dot(d, y) / Dot(y, y), Dot(d, z) / Dot(z, z));
	Vec3 q(Clamp(l.x, mn.x, mx.x), Clamp(l.y, mn.y, mx.y), Clamp(l.z, mn.z, mx.z));
	return (m.Apply(q) - p).Length() < r;
}

void Script::BallContacts(Vec3 p, float r, const int* meshes, int meshCount)
{
	for (size_t i = 0; i < triggers.size(); i++)
	{
		Trigger& t = triggers[i];
		if (t.state != 1 || !t.volume || !world->Enabled(t.node)) continue;
		if (touchesVolume(t, p, r) != t.onExit) start(i);
	}
	for (int k = 0; k < meshCount; k++)
	{
		int t = meshTrigger[meshes[k]];
		if (t >= 0 && triggers[t].state == 1) start(t);
	}
}

// ---------------------------------------------------------------- ejecución

static u32 keyMask(const std::string& k)
{
	if (k == "action") return KEY_A | KEY_B;
	if (k == "left") return KEY_LEFT;
	if (k == "right") return KEY_RIGHT;
	if (k == "up") return KEY_UP;
	if (k == "down") return KEY_DOWN;
	if (k == "middle") return KEY_X;
	if (k == "menu1") return KEY_SELECT;
	if (k == "menu2") return KEY_START;
	return 0;
}

// Ejecuta una acción. Devuelve true si terminó (se pasa a la siguiente en el mismo cuadro).
// `jumped` = true si cambió el índice del trigger (no se avanza).
bool Script::exec(Trigger& t, Action& a, float dt, GameState& gs, bool& jumped)
{
	const auto& A = a.args;
	auto arg = [&](size_t i) -> std::string { return i < A.size() ? A[i] : std::string(); };
	auto num = [&](size_t i, float def = 0) { return i < A.size() ? value(A[i]) : def; };
	auto trig = [&](size_t i) { return findTrigger(arg(i), t.instance); };
	auto jumpTo = [&](int oneBased) { t.index = oneBased - 1; jumped = true; };
	Ball& ball = *gs.ball;
	executed++;

	switch (a.verb)
	{
	case V_NOP: case V_PRESSURE: case V_PLAYSOUND: case V_STOPSOUND: case V_PARTICLEFX: case V_BREAK:
	case V_ADDSPEED: case V_SETOBJECTSTATE: case V_SWITCHOBJECTSTATE: case V_TIMER: case V_CHANGEBALL:
	case V_SETBALLDIRECTION: case V_PLAYANIMATION:
		return true;   // pendientes en el port (sonido, partículas, animaciones, formas)

	case V_DELAY: case V_RANDOMDELAY:
		if (!a.started)
		{
			a.started = true;
			a.timer = A.size() >= 2 ? frand(num(0), num(1)) : num(0);
		}
		a.timer -= dt;
		if (a.timer > 0) return false;
		a.started = false;
		return true;

	case V_WAITFORIDLE:
		if (gs.sinceInput > 0.25f && gs.ballStill) a.timer += dt;
		else a.timer = 0;
		if (a.timer < num(0)) return false;
		a.timer = 0;
		return true;
	case V_WAITFORKEY: case V_WAITFORKEYPRESS:
		for (auto& k : A) if (gs.downKeys & keyMask(k)) return true;
		return false;
	case V_WAITFORKEYHOLD:
		for (auto& k : A) if (gs.heldKeys & keyMask(k)) return true;
		return false;
	case V_WAITFORKEYRELEASE:
		for (auto& k : A) if (gs.upKeys & keyMask(k)) return true;
		return false;

	case V_JUMPTOACTION:
		if (A.size() >= 2)
		{
			if (a.counter < (int)num(1)) { a.counter++; jumpTo((int)num(0)); return false; }
			a.counter = 0;
			return true;
		}
		jumpTo((int)num(0));
		return false;
	case V_JUMPTORANDOMACTION:
		if (A.empty()) return true;
		jumpTo((int)num(rand() % A.size()));
		return false;

	case V_ACTIVATETRIGGER: { int i = trig(0); if (i >= 0 && &triggers[i] != &t) start(i); else if (i >= 0) { jumpTo(1); return false; } return true; }
	case V_ACTIVATERANDOMTRIGGER: if (!A.empty()) start(trig(rand() % A.size())); return true;
	case V_ENABLETRIGGER: { int i = trig(0); if (i >= 0 && triggers[i].state == 0) triggers[i].state = 1; return true; }
	case V_DISABLETRIGGER: case V_PAUSETRIGGER:
	{
		int i = trig(0);
		if (i < 0) return true;
		triggers[i].state = 0;
		return &triggers[i] != &t;   // si se desactiva a sí mismo, se detiene aquí
	}
	case V_FINALIZETRIGGER: { int i = trig(0); if (i >= 0 && triggers[i].state == 2) { triggers[i].state = 3; triggers[i].exitIndex = 0; } return true; }

	case V_SET: vars[arg(0)] = num(1); return true;
	case V_ADD: vars[arg(0)] = value(arg(0)) + num(1); return true;
	case V_SUB: vars[arg(0)] = value(arg(0)) - num(1); return true;
	case V_MUL: vars[arg(0)] = value(arg(0)) * num(1); return true;
	case V_DIV: { float d = num(1); if (d != 0) vars[arg(0)] = value(arg(0)) / d; return true; }
	case V_LESS: case V_EQUAL: case V_GREATER: case V_NOTEQUAL:
	{
		float x = value(arg(0)), y = num(1);
		bool ok = a.verb == V_LESS ? x < y : a.verb == V_GREATER ? x > y : a.verb == V_EQUAL ? x == y : x != y;
		if (ok) return true;
		if (A.size() >= 3) jumpTo((int)num(2));
		return false;
	}
	case V_COUNTER:
		// solo deja pasar una de cada N activaciones (port)
		if (++a.counter < (int)num(0, 1)) { t.index = t.actions.size(); jumped = true; return false; }
		a.counter = 0;
		return true;

	case V_LINK:
	{
		int parent = arg(0) == "scene" ? -1 : resolveNode(arg(0), t.instance);
		for (size_t i = 1; i < A.size(); i++)
		{
			int c = resolveNode(A[i], t.instance);
			if (c >= 0) world->Link(parent, c);
		}
		world->UpdateTransforms();
		return true;
	}
	case V_UNLINK:
		for (auto& n : A) { int c = resolveNode(n, t.instance); if (c >= 0) world->Unlink(c); }
		return true;

	case V_SETRENDERSTATE:
	{
		int obj = resolveNode(arg(0), t.instance);
		int n = world->FindBelow(obj, arg(1));
		if (n >= 0) world->nodes[n].visible = num(2) != 0;
		return true;
	}
	case V_SWITCHRENDERSTATE:
	{
		// muestra el nodo y oculta los hermanos de la misma serie (nombre sin los dígitos finales)
		int obj = resolveNode(arg(0), t.instance);
		int n = world->FindBelow(obj, arg(1));
		if (n < 0) return true;
		auto stem = [](const char* s) { std::string r(s); while (!r.empty() && isdigit((unsigned char)r.back())) r.pop_back(); return r; };
		std::string st = stem(world->nodes[n].Name());
		int p = world->nodes[n].parent;
		const std::vector<int>& sib = p >= 0 ? world->nodes[p].children : world->roots;
		for (int s : sib) if (stem(world->nodes[s].Name()) == st) world->nodes[s].visible = s == n;
		return true;
	}

	case V_MOVEOBJECT: case V_ROTATEOBJECT: case V_MOVEUV:
	{
		bool uv = a.verb == V_MOVEUV;
		int timeArg = uv ? 4 : 5;
		bool wait = num(timeArg + 1) != 0;
		if (!a.started)
		{
			a.started = true;
			int obj = resolveNode(arg(0), t.instance);
			int n = world->FindBelow(obj, arg(1));
			if (n < 0)
			{
				static std::vector<std::string> reported;
				std::string key = arg(0) + "/" + arg(1);
				if (std::find(reported.begin(), reported.end(), key) == reported.end())
				{
					reported.push_back(key);
					logf_("%s: no encuentro %s (se ignora)\n", VERBS[a.verb], key.c_str());
				}
				a.started = false;
				return true;
			}
			float time = num(timeArg);
			Mover mv;
			mv.node = n;
			mv.remaining = time;
			if (uv)
			{
				mv.kind = 2;
				mv.rate = Vec3(num(2), num(3), 0) * (time > 0 ? 1 / time : 1);
				if (time <= 0) mv.remaining = 1e9f;   // sin tiempo: desplazamiento continuo (port)
			}
			else
			{
				Vec3 v(num(2), num(3), num(4));
				bool worldAxes = num(timeArg + 2) != 0;
				Vec3 av = Rules::ActionVector(v, worldAxes);   // FUN_00013784 casos 0x15 / 0x1e
				// FUN_0003ae40: dirección unitaria + velocidad = cantidad / tiempo; se aplica cada cuadro
				// en FUN_00039698 (Rules::MoveStep / Rules::RotateLocal)
				mv.kind = a.verb == V_MOVEOBJECT ? 0 : 1;
				mv.axis = av.Normalized();
				mv.worldAxes = worldAxes;
				float amount = a.verb == V_MOVEOBJECT ? av.Length() : Rules::RotationAngle(v);
				mv.speed = time > 0 ? amount / time : amount;
				if (time <= 0) mv.remaining = 1.0f;   // instantáneo: un segundo a velocidad total, abajo se corta
			}
			if (time <= 0 && !uv)
			{
				// sin duración: se aplica de golpe
				applyMover(mv, 1.0f);
				a.started = false;
				return true;
			}
			// hueco libre: terminado y sin nadie esperándolo (los triggers en bucle no hacen crecer la lista)
			mv.waited = wait;
			int slot = -1;
			for (size_t i = 0; i < movers.size() && slot < 0; i++)
				if (movers[i].done && !movers[i].waited) slot = i;
			if (slot < 0) { slot = movers.size(); movers.push_back(mv); }
			else movers[slot] = mv;
			a.mover = wait ? slot : -1;
			if (!wait) { a.started = false; return true; }
		}
		if (a.mover >= 0 && a.mover < (int)movers.size())
		{
			if (!movers[a.mover].done) return false;
			movers[a.mover].waited = false;
		}
		a.started = false;
		a.mover = -1;
		return true;
	}

	case V_SETOBJECTPOS:
	{
		int target = resolveNode(arg(1), t.instance);
		if (target < 0) return true;
		const Mat34& tw = world->nodes[target].world;
		if (arg(0) == "G_balls_root")
		{
			ball.pos = tw.Pos();
			ball.vel = Vec3();
			if (num(2) != 0) { Vec3 f = tw.Col(1); ball.yaw = atan2f(f.x, f.z); }
		}
		else
		{
			int n = resolveNode(arg(0), t.instance);
			if (n < 0) return true;
			RNode& rn = world->nodes[n];
			Vec3 d = toParentSpace(n, tw.Pos() - rn.world.Pos());
			rn.local.m[0][3] += d.x; rn.local.m[1][3] += d.y; rn.local.m[2][3] += d.z;
			world->UpdateTransforms();
		}
		return true;
	}

	case V_CHECKPOINT:
	{
		gs.hasCheckpoint = true;
		if (A.size() >= 4)
		{
			gs.checkpointPos = Vec3(num(0), num(2), num(1));   // coordenadas de 3ds Max
			gs.checkpointYaw = num(3) * DEG2RAD;
		}
		else
		{
			int d = resolveNode(arg(0), t.instance);
			if (d < 0) { gs.hasCheckpoint = false; return true; }
			const Mat34& m = world->nodes[d].world;
			gs.checkpointPos = m.Pos();
			Vec3 f = m.Col(1);
			gs.checkpointYaw = atan2f(f.x, f.z);
		}
		return true;
	}
	case V_LEVELFAILED: gs.levelFailed = true; return true;
	case V_LEVELCOMPLETE: gs.levelComplete = true; return true;
	case V_LOADLEVEL: gs.loadLevel = arg(0); return true;
	case V_ADDLIFE: gs.lives += (int)num(0, 1); return true;
	case V_ADDSCORE: gs.score += (int)num(0, 1); return true;
	case V_SHOWTEXT: gs.text = arg(0); return true;

	case V_SETGAMEMODE:
		gs.SetGameMode((int)num(0), true);
		return true;
	case V_SETGUIDINGOBJECT:
		gs.guiding = arg(0) == "G_balls_root" ? -1 : resolveNode(arg(0), t.instance);
		return true;
	case V_SETCAMERAVIEW:
	{
		float p = num(0), d = num(1);
		gs.camPitch = (p >= -90 && p <= 90 && A.size() >= 1) ? p : 55;
		gs.camDist = d > 0 ? d : 555;
		return true;
	}
	case V_SETCAMPROPS:
	{
		// FUN_000115d4 caso 0x21: a/b encienden (!=0) o apagan los muelles; -1 los deja. k/c: >= 0 se
		// usan, -1 deja el valor, otro negativo restaura el de fábrica (27,5 / 5,5)
		auto spring = [&](size_t i, float& v, float def) {
			if (A.size() <= i) return;
			float x = num(i);
			if (x >= 0) v = x;
			else if (x != -1.0f) v = def;
		};
		if (A.size() >= 1 && num(0) != -1.0f) gs.eyeSpring = num(0) != 0;
		if (A.size() >= 2 && num(1) != -1.0f) gs.targetSpring = num(1) != 0;
		spring(2, gs.camStiffness, 27.5f);
		spring(3, gs.camDamping, 5.5f);
	}
		return true;

	case V_SPEEDMUL: ball.vel *= num(1); return true;
	case V_MAXSPEED: ball.maxSpeed = num(0); return true;
	case V_SETGRAVITY: ball.gravity = Vec3(num(0), num(1), num(2)); return true;
	case V_ADDIMPULSE:
	{
		int d = resolveNode(arg(1), t.instance);
		Vec3 dir = d >= 0 ? world->nodes[d].world.Col(1).Normalized() : Vec3(0, 1, 0);
		ball.Impulse(dir * num(0));
		return true;
	}
	}
	return true;
}

// Un paso de un movimiento o giro sobre su nodo (FUN_00039698, ramas tipo 0 y 1)
void Script::applyMover(const Mover& mv, float step)
{
	RNode& rn = world->nodes[mv.node];
	if (mv.kind == 0)
	{
		// base: local actual sin `ejes`; inversa del mundo con `ejes` (local si no hay padre)
		Mat34 basis = !mv.worldAxes ? rn.local : Rules::AffineInverse(rn.parent >= 0 ? rn.world : rn.local);
		rn.local = Rules::MoveStep(rn.local, basis, mv.axis, mv.speed * step);
	}
	else
	{
		Vec3 axis = mv.axis;
		if (mv.worldAxes)   // con `ejes`: eje del mundo -> local con la inversa del mundo, sin normalizar (FUN_00039698 tipo 1)
			axis = Rules::RotApply(Rules::AffineInverse(rn.world), axis);
		rn.local = Rules::RotateLocal(rn.local, axis, mv.speed * step);
	}
}

void Script::Update(float dt, GameState& gs)
{
	vars["speed01"] = gs.ball->vel.Length();

	for (size_t ti = 0; ti < triggers.size(); ti++)
	{
		Trigger& t = triggers[ti];
		for (int guard = 0; guard < 64 && (t.state == 2 || t.state == 3); guard++)
		{
			auto& list = t.state == 2 ? t.actions : t.exits;
			int& idx = t.state == 2 ? t.index : t.exitIndex;
			if (idx < 0 || idx >= (int)list.size())
			{
				if (t.state == 2) { t.state = 3; t.exitIndex = 0; continue; }
				t.state = t.once ? 0 : 1;   // executetype = 0: no vuelve a dispararse
				break;
			}
			bool jumped = false;
			int stateBefore = t.state;
			bool done = exec(t, list[idx], dt, gs, jumped);
			if (t.state != stateBefore) break;   // la acción cambió el estado de su propio trigger
			if (jumped) break;
			if (!done) break;
			idx++;
		}
	}

	// movimientos en curso
	for (auto& mv : movers)
	{
		if (mv.done) continue;
		float step = fminf(dt, mv.remaining);
		RNode& rn = world->nodes[mv.node];
		if (mv.kind != 2)
		{
			applyMover(mv, step);
			world->UpdateTransforms();   // el siguiente movimiento del cuadro ve el resultado (FUN_00039698)
		}
		else
		{
			rn.uvOffset[0] = fmodf(rn.uvOffset[0] + mv.rate.x * step, 1.0f);
			rn.uvOffset[1] = fmodf(rn.uvOffset[1] + mv.rate.y * step, 1.0f);
		}
		mv.remaining -= step;
		if (mv.remaining <= 0) mv.done = true;
	}
}
