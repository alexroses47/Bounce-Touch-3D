-- Traza del juego original en EKA2L1 (proyecto Bounce Touch 3DS). Solo lee memoria; no cambia nada.
-- Copiar a ~/Library/Application Support/EKA2L1/scripts/ y jugar; las líneas "BT..." van a EKA2L1.log.
-- Direcciones de enlace de Bounce.exe (ver docs/CLASES.md):
--   0x4ee68 PlayerBall::v03 (entrada 3, cada cuadro)   0x4f2c8 PlayerBall::v05 (respuesta al choque)
--   0x35470 GameCamera::v02 (cada cuadro)
local common = require('eka2l1.common')
local evt = require('eka2l1.events')
local cpu = require('eka2l1.cpu')
local mem = require('eka2l1.mem')
local ffi = require('ffi')

local UID = 0x2000C4D4
local MAX_LINES = 40000
local lines, frame, base = 0, 0, nil
local buf = ffi.new('uint32_t[1]')

local function u(a) return mem.readDword(a) end
local function f(a) buf[0] = mem.readDword(a); return ffi.cast('float*', buf)[0] end
local function b(a) return mem.readByte(a) end
local function log(s)
	if lines < MAX_LINES then
		lines = lines + 1
		common.log(s)
	end
end
local function v3(a) return string.format('%.3f %.3f %.3f', f(a), f(a + 4), f(a + 8)) end

common.log('BT traza de la bola y la cámara cargada')

-- Cada cuadro: posición del cuerpo, velocidad, fuerzas e impulsos acumulados, suelo, dt
evt.registerBreakpointHook('Bounce.exe', 0x4ee68, 0, UID, function()
	local this = cpu.getReg(0)
	frame = frame + 1
	if base == nil then base = cpu.getPc() - (0x4ee68 - 0x8000) end
	local body = u(u(this + 0x68))
	local game = u(u(base + (0x4edc4 - 0x8000)))
	log(string.format('BTB %d dt=%.5f p=(%.3f %.3f %.3f) v=(%s) F=(%s) J=(%s) suelo=%d forma=%d vmax=%.1f lim=%.1f',
		frame, f(game + 0x3c4), f(body + 0x94), f(body + 0xa4), f(body + 0xb4),
		v3(this + 0x38), v3(this + 0xe0), v3(this + 0xec), b(this + 0x108),
		u(this + 0x1cc), f(this + 0x1dc), f(this + 0x150)))
end)

-- Respuesta al choque: contactos que recibe (punto, normal, desplazamiento de la superficie, grupo)
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

-- Cámara: matriz local de su nodo (posición del ojo y ejes)
evt.registerBreakpointHook('Bounce.exe', 0x35470, 0, UID, function()
	local cam = cpu.getReg(0)
	local node = u(cam + 0x90)
	if node == 0 then return end
	local m = node + 8
	log(string.format('BTK %d ojo=(%.2f %.2f %.2f) x=(%.3f %.3f %.3f) y=(%.3f %.3f %.3f) z=(%.3f %.3f %.3f)', frame,
		f(m + 12), f(m + 28), f(m + 44), f(m), f(m + 16), f(m + 32), f(m + 4), f(m + 20), f(m + 36), f(m + 8), f(m + 24), f(m + 40)))
end)
