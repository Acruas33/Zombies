#ifndef CommandPacket_H
#define CommandPacket_H

#include "CommandType.h"

struct CommandPacket 
{
	int clientID;
	CommandType commandType;
};
#endif
