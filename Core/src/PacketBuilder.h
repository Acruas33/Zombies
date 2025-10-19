#ifndef PacketBuilder_H
#define PacketBuilder_H

#include <vector>
#include "MessageType.h"
#include <enet/enet.h>
#include <variant>
#include "ClientPacket.h"
#include "PacketVariant.h"
#include <iostream>

class PacketBuilder
{
public:
	PacketBuilder() = default;

	template<typename T>
	void write(T packet)
	{
		packets.push_back(packet);
		size++;
	}

	ENetPacket* build() 
	{
		std::vector<uint8_t> buffer;
		writeUInt32(buffer, tickID);
		
		buffer.push_back(size);
		for (auto& p : packets)
		{
			std::visit([&buffer](auto& pkt) {
				using T = std::decay_t<decltype(pkt)>;
				if constexpr (std::is_same_v<T, ClientPacket>) {
					buffer.push_back(static_cast<uint8_t>(MessageType::CONNECT));
					std::vector<uint8_t> serializedData(sizeof(ClientPacket));
					std::memcpy(serializedData.data(), &pkt, sizeof(ClientPacket));
					buffer.insert(buffer.end(), serializedData.begin(), serializedData.end());
				}
				else if constexpr (std::is_same_v<T, ObjectPacket>) {
					buffer.push_back(static_cast<uint8_t>(MessageType::OBJECT));
					std::vector<uint8_t> serializedData(sizeof(ObjectPacket));
					std::memcpy(serializedData.data(), &pkt, sizeof(ObjectPacket));
					buffer.insert(buffer.end(), serializedData.begin(), serializedData.end());
				}
				else if constexpr (std::is_same_v<T, CommandPacket>) {
					buffer.push_back(static_cast<uint8_t>(MessageType::CHAT));
					std::vector<uint8_t> serializedData(sizeof(CommandPacket));
					std::memcpy(serializedData.data(), &pkt, sizeof(CommandPacket));
					buffer.insert(buffer.end(), serializedData.begin(), serializedData.end());
				}
			}, p);
		}

		size = 0;
		packets.clear();
		tickID++;
		//std::cout << "Built packet with tick ID: " << static_cast<int>(tickID - 1) << std::endl;

		return enet_packet_create(buffer.data(), buffer.size(), ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT);
	}

	void writeUInt32(std::vector<uint8_t>& buffer, uint32_t value)
	{
		uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
		buffer.insert(buffer.end(), ptr, ptr + sizeof(uint32_t));
	}
	
	std::vector<PacketVariant> packets; //gets populated by write, contains all the data we want to send before serialization.

	uint8_t size = 0; //number of PacketVariant objects we have written to the packet.
	uint32_t tickID = 0; //tick ID for the packet, used to identify the tick this packet was sent in.

};
#endif
