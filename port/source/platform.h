// Lo mínimo de la plataforma que necesita el núcleo del juego (nivel, bola, guiones, cámara).
// En la 3DS es libctru; en el build host de macOS (BT_HOST) se definen los mismos nombres.
#pragma once

#ifdef BT_HOST
#include <stdint.h>
#include <stdlib.h>
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;

// mismos valores que libctru (los guiones comparan bits de teclas)
enum
{
	KEY_A = 1u << 0, KEY_B = 1u << 1, KEY_SELECT = 1u << 2, KEY_START = 1u << 3,
	KEY_DRIGHT = 1u << 4, KEY_DLEFT = 1u << 5, KEY_DUP = 1u << 6, KEY_DDOWN = 1u << 7,
	KEY_R = 1u << 8, KEY_L = 1u << 9, KEY_X = 1u << 10, KEY_Y = 1u << 11,
	KEY_CPAD_RIGHT = 1u << 28, KEY_CPAD_LEFT = 1u << 29, KEY_CPAD_UP = 1u << 30, KEY_CPAD_DOWN = 1u << 31,
	KEY_UP = KEY_DUP | KEY_CPAD_UP, KEY_DOWN = KEY_DDOWN | KEY_CPAD_DOWN,
	KEY_LEFT = KEY_DLEFT | KEY_CPAD_LEFT, KEY_RIGHT = KEY_DRIGHT | KEY_CPAD_RIGHT,
};

inline void* linearAlloc(size_t n) { return malloc(n); }
inline void linearFree(void* p) { free(p); }
#else
#include <3ds.h>
#endif

// carpeta de datos convertidos: "romfs:/" en la 3DS, out/romfs/ en el host
extern const char* g_dataRoot;
