#ifndef Network_H
#define Network_H

#include <enet/enet.h>
#include <cstdint>
#include <thread>
#include <optional>
#include <vector>
#include <map>
#include "GameObject.h"
#include "PacketBuilder.h"
#include "Snapshot.h"
#include <deque>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

class Network
{
public:
	static uint8_t clientID;
	static ENetHost* client;
	static ENetPeer* peer;
	static ENetAddress address;
	static ENetEvent event;

	static inline bool startGame = false;

	//Interpolation server update stuffs
	static inline float packetTimer = 1.0f / 60.0f;
	
	static inline float clientTime = 0.0f;
	static inline float tickRate = 60.0f;
	static inline float tickInterval = 1.0f / tickRate;

	static inline uint32_t latestServerTick = 0.0f;

	static inline float interpolationDelayInTicks = 6.0f;

	static inline std::map<uint32_t, Snapshot> snapshots;
	static inline std::deque<Snapshot> snapshotBuffer;


	static inline PacketBuilder* pb = new PacketBuilder();

	static bool isInitialized();
	static Network& getInstance();
	static int init(bool isHost);
	static void networkHost();
	static void networkClient();
	static void sendPacket(ENetPeer* peer, ENetPacket* packet);
	static void handlePacket(ENetPacket* packet);

	static void sendServerUpdate();
	static void handleServerUpdates();

	static std::string getLocalIP();

	static float getCurrentTick();
	static bool getSnapshotsForInterpolation(Snapshot& outPrev, Snapshot& outNext, float renderTick);
	static float computeAlpha(float renderTick, uint32_t tickPrev, uint32_t tickNext);

private:
	Network(bool isHost);

	static uint32_t readUInt32(const uint8_t* data, size_t& offset);

	static std::optional<std::thread> networkThread;
	static Network* instance;
	bool isHost;
};

#endif