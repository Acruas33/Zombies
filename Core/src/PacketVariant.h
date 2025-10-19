#ifndef PacketVariant_H
#define PacketVariant_H

#include <variant>
#include "ObjectPacket.h"
#include "ClientPacket.h"
#include "CommandPacket.h"

using PacketVariant = std::variant<ObjectPacket, ClientPacket, CommandPacket>;

#endif


