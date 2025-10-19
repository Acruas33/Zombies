#ifndef Snapshot_H
#define Snapshot_H

#include <vector>
#include "ObjectPacket.h"

struct Snapshot {
	uint32_t tickID;
	std::vector<ObjectPacket> entities;
};

#endif
