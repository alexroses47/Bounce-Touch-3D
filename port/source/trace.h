// Traza del port con el mismo formato que tools/eka2l1/bt_ball_trace.lua (líneas BTB/BTC/BTK), para
// compararla fotograma a fotograma con la del juego original (tools/compare_traces.py).
//   BTB n dt=.. p=(..) v=(..) F=(..) J=(..) suelo=.. forma=.. vmax=.. lim=..   al empezar el paso de la bola
//   BTC n n=.. v=(..) e=..  + una línea "BTC   i p=(..) n=(..) d=(..) w=(..) f40=.. modo=.. f14c=.. bits8-17=.."
//   BTK n ojo=(..) obj=(..) vel=(ojo obj)                                       al terminar la cámara
#pragma once
#include <stdio.h>
#include "collision.h"
#include "vec.h"

struct Trace
{
	FILE* f = nullptr;
	int frame = 0;

	bool Open(const char* path) { f = fopen(path, "w"); return f != nullptr; }
	void Close() { if (f) fclose(f); f = nullptr; }

	void BallState(float dt, Vec3 p, Vec3 v, Vec3 force, Vec3 impulse, bool ground, int form, float vmax, float lim)
	{
		frame++;
		if (!f) return;
		fprintf(f, "BTB %d dt=%.5f p=(%.3f %.3f %.3f) v=(%.3f %.3f %.3f) F=(%.3f %.3f %.3f) J=(%.3f %.3f %.3f) suelo=%d forma=%d vmax=%.1f lim=%.1f\n",
			frame, dt, p.x, p.y, p.z, v.x, v.y, v.z, force.x, force.y, force.z, impulse.x, impulse.y, impulse.z,
			ground ? 1 : 0, form, vmax, lim);
	}

	void Contacts(Vec3 v, float e, const Contact* c, int n, const CollisionWorld& world)
	{
		if (!f) return;
		fprintf(f, "BTC %d n=%d v=(%.3f %.3f %.3f) e=%.3f\n", frame, n, v.x, v.y, v.z, e);
		for (int i = 0; i < n && i < 4; i++)
		{
			u32 g = world.meshes[c[i].mesh].group;
			char bits[11];
			for (int k = 0; k < 10; k++) bits[k] = '0' + ((g >> (8 + k)) & 1);
			bits[10] = 0;
			Vec3 s = world.meshes[c[i].mesh].velocity;
			fprintf(f, "BTC   %d p=(%.3f %.3f %.3f) n=(%.3f %.3f %.3f) d=(%.3f %.3f %.3f) w=(0.000 0.000 0.000) f40=0 modo=%u f14c=%u bits8-17=%s\n",
				i, c[i].point.x, c[i].point.y, c[i].point.z, c[i].normal.x, c[i].normal.y, c[i].normal.z,
				s.x, s.y, s.z, (unsigned)(g & 0xF), (unsigned)((g >> 4) & 0xF), bits);
		}
	}

	void Camera(Vec3 eye, Vec3 target, float eyeSpeed, float targetSpeed)
	{
		if (!f) return;
		fprintf(f, "BTK %d ojo=(%.2f %.2f %.2f) obj=(%.2f %.2f %.2f) vel=(%.3f %.3f)\n", frame,
			eye.x, eye.y, eye.z, target.x, target.y, target.z, eyeSpeed, targetSpeed);
	}
};
