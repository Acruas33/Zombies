#ifndef Network_H
#define Network_H

#include <enet/enet.h>
#include <vector>
#include <map>
#include "GameObject.h"
#include <optional>
#include <thread>
#include <queue>
#include "PacketVariant.h"
#include "MessageType.h"
#include "PacketBuilder.h"

class Network
{
public:
	inline static ENetAddress address = {};
	inline static ENetEvent event = {};
	inline static ENetHost* server = nullptr;

	inline static uint8_t clientCount = 0;
	inline static uint8_t networkID = 0;

	inline static std::queue<PacketVariant> pendingPackets;

	static inline float packetTimer = 1.0f / 60.0f;
	static inline PacketBuilder* pb = new PacketBuilder();

	static int init();
	static bool isInitialized();
	
	static void sendPacket(ENetPeer* peer, ENetPacket* packet);
	static void broadcastPacket(ENetPacket* packet);
	
	static void handlePacket(ENetPacket* packet);
	static void handleConnection(ENetEvent event);

	static void handleClientUpdates();
	static void sendServerUpdate();
	
	static void networkHost();

private:
	Network() = default;

	static uint32_t readUInt32(const uint8_t* data, size_t& offset);

	inline static std::optional<std::thread> networkThread;
	inline static Network* instance = nullptr;

};

#endif
