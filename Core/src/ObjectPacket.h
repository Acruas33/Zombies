#ifndef ObjectPacket_H
#define ObjectPacket_H

#include "ObjectType.h"

struct ObjectPacket {
	int clientID;
	int objectID;
	ObjectType objectType;
	float x, y, dx, dy, rotation, health;
};

#endif
