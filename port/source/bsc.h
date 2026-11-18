// Formato de escena BSC1 (lo genera tools/bsc.py; descripción en docs/ROMFS.md).
#pragma once
#include "platform.h"

struct BscHeader
{
	char magic[4];
	u32 version, fileSize;
	u32 stringsOff, stringsSize;
	u32 texCount, texOff;
	u32 matCount, matOff;
	u32 primCount, primOff;
	u32 nodeCount, nodeOff;
	u32 animCount, animOff;
	u32 refCount, refOff;
	u32 vboOff, vboSize;
	u32 iboOff, iboSize;
};

enum { BSC_MAT_TEXTURED = 1, BSC_MAT_MULTIPLY = 2, BSC_MAT_NO_ZWRITE = 4 };
struct BscMaterial { u32 flags; s32 texture; u32 rgba; u32 shader; };

struct BscPrimitive
{
	u32 material, vertexCount, indexCount, vboOff, iboOff;
	float posScale, posBias[3];
	float uvScale;
	float bboxMin[3], bboxMax[3];
};

enum BscKind { BSC_MESH, BSC_CAMERA, BSC_LIGHT, BSC_DUMMY, BSC_LINES, BSC_NODE };
enum BscRole { ROLE_VISUAL, ROLE_COLLISION, ROLE_SKYBOX, ROLE_TRIGGER, ROLE_DUMMY, ROLE_OTHER };

struct BscNode
{
	u32 name;
	s32 parent;
	u8 kind, role;
	u16 reserved;
	float transform[12];
	u32 firstRef, refCount;
	float params[6];
	s32 anim;
	u32 props;
};

struct BscAnim
{
	u8 posRate, rotRate, sclRate, end;
	u32 posCount, posOff, rotCount, rotDim, rotOff, sclCount, sclOff;
	float endTime;
};

static_assert(sizeof(BscHeader) == 84, "BscHeader");
static_assert(sizeof(BscMaterial) == 16, "BscMaterial");
static_assert(sizeof(BscPrimitive) == 64, "BscPrimitive");
static_assert(sizeof(BscNode) == 100, "BscNode");
static_assert(sizeof(BscAnim) == 36, "BscAnim");
