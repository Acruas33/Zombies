#ifndef ObjectType_H
#define ObjectType_H

#include <cstdint>

enum class ObjectType : uint8_t {
	PLAYER = 1,
	ENEMY = 2,
	PROJECTILE = 3,
	ITEM = 4,
	NPC = 5,
	TERRAIN = 6,
	EFFECT = 7
};

#endif
