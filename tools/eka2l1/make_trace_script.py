#!/usr/bin/env python3
"""Genera tools/eka2l1/bt_trace_v2.lua: traza del juego original en EKA2L1 (solo lee memoria).

Líneas que escribe en EKA2L1.log (además de BTB/BTC de la v1):
  BTT n p2=.. p3=.. toque=(x y) reloj=..  cada llamada al control táctil Game::v10 (0x2fd08)
  BTI n held=HH pressed=HH tap=T raw=..   entradas al empezar Game::v03 (0x2e810) del fotograma n:
                                          máscaras de las acciones 1..7 del original (tabla FUN_00020464)
                                          y códigos virtuales mantenidos/pulsados en crudo
  BTB ... cam=(incl rumbo dist) f232=.. w224=(..)   campos de cámara de la bola (+0x138/+0x13c/+0x140)
  BTK n ojo=(..) obj=(..) vel=(ojo obj) k=.. c=.. reposo=.. fl=abc obs=(..) modo=.. fuente=..
                                          estado de GameCamera al SALIR de v02 (0x35aac)
  BTV n vista=(..) despl=(..)             tabla de setcameraview (0x166a78) y desplazamiento del
                                          objetivo (0x400150) cuando cambian
  BTR n 0x134 a->b en=NOMBRE lr=..        primer punto observado en que cambia la restitución de la bola
                                          (EKA2L1 no tiene watchpoints: se comprueba en cada importación
                                          y en cada método de PlayerBall/Ball)
Uso: make_trace_script.py  (lee re/imports.tsv y escribe tools/eka2l1/bt_trace_v2.lua)
"""
import os

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.join(HERE, "..", "..")

# métodos de PlayerBall (docs/CLASES.md) y de Ball que hereda, más el constructor de PlayerBall
METHODS = {
    0x4e808: "PlayerBall::v02", 0x4ee68: "PlayerBall::v03", 0x4f200: "PlayerBall::v04",
    0x4f2c8: "PlayerBall::v05", 0x50630: "PlayerBall::v06", 0x14ed7c: "PlayerBall::v11",
    0x14ec50: "PlayerBall::v12", 0x14ea68: "PlayerBall::v13", 0x14ed70: "PlayerBall::v17",
    0x14ed4c: "PlayerBall::v18", 0x14d3c0: "Ball::v09", 0x14edb4: "Ball::v10", 0x1f080: "Ball::v14",
    0x1f248: "Ball::v15", 0x14d368: "Ball::v19", 0x50d54: "PlayerBall salto (FUN_00050d54)",
    0x2e810: "Game::v03",
}
SKIP = ("__aeabi_", "sqrt", "sinf", "cosf", "fabsf", "floorf", "atan", "acosf", "tanf", "fmod", "pow", "sin",
        "memcpy", "memset", "memmove", "memcmp", "strlen")

HEADER = r"""-- GENERADO por tools/eka2l1/make_trace_script.py: no editar a mano.
-- Traza del juego original en EKA2L1 (proyecto Bounce Touch 3DS). Solo lee memoria; no cambia nada.
-- Copiar a ~/Library/Application Support/EKA2L1/scripts/ (y quitar bt_ball_trace.lua); las líneas BT* van a EKA2L1.log.
local common = require('eka2l1.common')
local evt = require('eka2l1.events')
local cpu = require('eka2l1.cpu')
local mem = require('eka2l1.mem')
local ffi = require('ffi')

local UID = 0x2000C4D4
local MAX_LINES = 400000
local lines, frame, base, g = 0, 0, nil, nil
local ball, lastR = nil, nil
local cam = nil
local game = nil
local lastView = ''
local buf = ffi.new('uint32_t[1]')

local function u(a) return mem.readDword(a) end
local function f(a) buf[0] = mem.readDword(a); return ffi.cast('float*', buf)[0] end
local function b(a) return mem.readByte(a) end
local function s16(a) local w = mem.readWord(a); if w >= 0x8000 then w = w - 0x10000 end; return w end
local function log(s)
	if lines < MAX_LINES then
		lines = lines + 1
		common.log(s)
	end
end
local function v3(a) return string.format('%.3f %.3f %.3f', f(a), f(a + 4), f(a + 8)) end

-- direcciones de enlace -> de ejecución (el código se reubica; los datos también)
local function code(a) return base + (a - 0x8000) end
local function setBase(linkAddr)
	if base == nil then
		base = cpu.getPc() - (linkAddr - 0x8000)
		g = u(u(code(0x4edc4)))          -- objeto global (*0x400040): teclas, dt...
	end
end
local function data(a) return u(code(0x4edc4)) - 0x400040 + a end   -- 0x4edc4 guarda &0x400040 reubicado

local function checkR(where)
	if ball == nil then return end
	local r = u(ball + 0x134)
	if r ~= lastR then
		buf[0] = lastR or 0
		local old = lastR and ffi.cast('float*', buf)[0] or -1
		buf[0] = r
		log(string.format('BTR %d 0x134 %.4f->%.4f en=%s lr=%x pc=%x', frame, old, ffi.cast('float*', buf)[0], where,
			cpu.getLr() - base + 0x8000, cpu.getPc() - base + 0x8000))
		lastR = r
	end
end

common.log('BT traza v2 cargada (bola, choques, entradas, cámara, restitución)')

-- constructor de PlayerBall: guarda la bola
evt.registerBreakpointHook('Bounce.exe', 0x4e6b4, 0, UID, function()
	setBase(0x4e6b4)
	ball = cpu.getReg(0)
	lastR = nil
	log(string.format('BTR %d nueva PlayerBall en %x', frame, ball))
end)

-- entradas: al empezar Game::v03, antes de la bola en el mismo fotograma
evt.registerBreakpointHook('Bounce.exe', 0x2e810, 0, UID, function()
	setBase(0x2e810)
	game = cpu.getReg(0)
	checkR('Game::v03')
	local tbl = u(u(g + 0x3dc) + 0x6c)
	local held, pressed = 0, 0
	for n = 1, 7 do
		local k = s16(tbl + 2 * n)
		if k >= 0 and k < 100 then
			if b(g + 0x214 + k) ~= 0 then held = held + 2 ^ n end
			if b(g + 0x278 + k) ~= 0 then pressed = pressed + 2 ^ n end
		end
	end
	local raw = ''
	for k = 0, 99 do
		local h, p = b(g + 0x214 + k), b(g + 0x278 + k)
		if h ~= 0 or p ~= 0 then raw = raw .. string.format('%x:%d/%d,', k, h, p) end
	end
	local function i32(a) local x = u(a); if x >= 0x80000000 then x = x - 0x100000000 end; return x end
	log(string.format('BTI %d held=%x pressed=%x tap=%d raw=%s toque=(%d %d) reloj=%.4f ini=(%.2f %.2f) tini=%.4f ult=(%.2f %.2f) teclado=%d modo=%d',
		frame + 1, held, pressed, b(g + 0x3f4), raw, i32(g + 0x3ec), i32(g + 0x3f0), f(g + 0x3cc),
		f(game + 0x3c), f(game + 0x40), f(game + 0x44), f(game + 0x48), f(game + 0x4c), b(game + 0x158), u(game + 0x128)))
	local view = string.format('vista=(%.2f %.2f %.2f %.2f) despl=(%s)', f(code(0x166a78)), f(code(0x166a7c)),
		f(code(0x166a80)), f(code(0x166a84)), v3(data(0x400150)))
	if view ~= lastView then log(string.format('BTV %d %s', frame + 1, view)); lastView = view end
end)

-- control táctil (Game::v10 0x2fd08): parámetros del gesto (r1, r2 como float)
evt.registerBreakpointHook('Bounce.exe', 0x2fd08, 0, UID, function()
	setBase(0x2fd08)
	buf[0] = cpu.getReg(1); local p2 = ffi.cast('float*', buf)[0]
	buf[0] = cpu.getReg(2); local p3 = ffi.cast('float*', buf)[0]
	log(string.format('BTT %d p2=%.4f p3=%.4f toque=(%d %d) reloj=%.4f', frame + 1, p2, p3, u(g + 0x3ec), u(g + 0x3f0), f(g + 0x3cc)))
end)

-- cada cuadro: posición del cuerpo, velocidad, fuerzas e impulsos acumulados, suelo, dt, cámara de la bola
evt.registerBreakpointHook('Bounce.exe', 0x4ee68, 0, UID, function()
	setBase(0x4ee68)
	local this = cpu.getReg(0)
	frame = frame + 1
	checkR('PlayerBall::v03')
	local body = u(u(this + 0x68))
	log(string.format('BTB %d dt=%.5f p=(%.3f %.3f %.3f) v=(%s) F=(%s) J=(%s) suelo=%d forma=%d vmax=%.1f lim=%.1f cam=(%.3f %.3f %.3f) f232=%d w224=(%s) e=%.4f',
		frame, f(g + 0x3c4), f(body + 0x94), f(body + 0xa4), f(body + 0xb4),
		v3(this + 0x38), v3(this + 0xe0), v3(this + 0xec), b(this + 0x108),
		u(this + 0x1cc), f(this + 0x1dc), f(this + 0x150),
		f(this + 0x138), f(this + 0x13c), f(this + 0x140), b(this + 0x232), v3(this + 0x224), f(this + 0x134)))
end)

-- respuesta al choque: contactos (punto, normal, desplazamiento de la superficie, grupo)
evt.registerBreakpointHook('Bounce.exe', 0x4f2c8, 0, UID, function()
	local this = cpu.getReg(0)
	local n = u(this + 0x60)
	local arr = u(this + 0x5c)
	log(string.format('BTC %d n=%d v=(%s) e=%.3f', frame, n, v3(this + 0x38), f(this + 0x134)))
	for i = 0, math.min(n, 4) - 1 do
		local r = arr + i * 0x44
		local obj = u(r + 0x3c)
		local flags = ''
		if obj ~= 0 then
			for k = 0x150, 0x159 do flags = flags .. b(obj + k) end
			flags = string.format('modo=%d f14c=%d bits8-17=%s', u(obj + 0x148), u(obj + 0x14c), flags)
		end
		log(string.format('BTC   %d p=(%s) n=(%s) d=(%s) w=(%s) f40=%d %s', i, v3(r), v3(r + 0xc), v3(r + 0x18),
			v3(r + 0x30), b(r + 0x40), flags))
	end
end)

-- cámara: se guarda el objeto al entrar en v02 y se lee su estado al salir (0x35aac)
evt.registerBreakpointHook('Bounce.exe', 0x35470, 0, UID, function() cam = cpu.getReg(0) end)
evt.registerBreakpointHook('Bounce.exe', 0x35aac, 0, UID, function()
	if cam == nil then return end
	local node = u(cam + 0xd8)
	if node == 0 then return end
	log(string.format('BTK %d ojo=(%.3f %.3f %.3f) obj=(%s) vel=(%.4f %.4f) k=%.3f c=%.3f reposo=%.3f fl=%d%d%d obs=(%s) modo=%d fuente=%x',
		frame, f(node + 0x14), f(node + 0x24), f(node + 0x34), v3(cam + 0xdc), f(cam + 0xfc), f(cam + 0x100),
		f(cam + 0xf4), f(cam + 0xf8), f(cam + 0x104), b(cam + 0x10c), b(cam + 0x10d), b(cam + 0x10e),
		v3(cam + 0xe8), u(u(cam + 0x94) + 0x128), u(cam + 0xd4)))
end)

-- restitución: se comprueba al empezar Game::v03 y PlayerBall::v03 (y, con --vigilar-importaciones, en cada importación)
"""


def main():
    import sys
    out = [HEADER]
    # --vigilar-importaciones: la versión pesada (2 FPS en EKA2L1) que localizó el cambio de restitución
    # (entre el constructor de PlayerBall y la llamada a PlayerBall::v17 desde 0x507fc). Por defecto, ligera.
    if "--vigilar-importaciones" not in sys.argv:
        path = os.path.join(HERE, "bt_trace_v2.lua")
        open(path, "w").write("\n".join(out) + "\n")
        print(f"{path}: versión ligera (sin importaciones vigiladas)")
        return
    for addr, name in sorted(METHODS.items()):
        if addr in (0x2e810,):
            continue
        out.append(f"evt.registerBreakpointHook('Bounce.exe', 0x{addr:x}, 0, UID, function() checkR('{name}') end)")
    n = 0
    for line in open(os.path.join(ROOT, "re", "imports.tsv")):
        parts = line.rstrip("\n").split("\t")
        if len(parts) < 5 or parts[0] != "func":
            continue
        addr, dll, sym = int(parts[1], 16) - 4, parts[2], parts[4]
        if sym.startswith(SKIP):
            continue
        out.append(f"evt.registerBreakpointHook('Bounce.exe', 0x{addr:x}, 0, UID, function() checkR('{dll}:{sym}') end)")
        n += 1
    path = os.path.join(HERE, "bt_trace_v2.lua")
    open(path, "w").write("\n".join(out) + "\n")
    print(f"{path}: {len(METHODS) - 1} métodos y {n} importaciones vigiladas")


if __name__ == "__main__":
    main()
