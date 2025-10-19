#ifndef Game_H
#define Game_H

#include <vector>
#include <queue>
#include <mutex>
#include "GameObject.h"
#include "Player.h"
#include <map>
#include <string>
#include <variant>
#include "ObjectPacket.h"
#include "PacketVariant.h"
#include <chrono>
#include <algorithm>
#include <glm/glm.hpp>

class Game
{
public:
	inline static std::vector<GameObject*> gameObjects;

	//consider making other vectors that point to only players or only projectiles without copying them but just point to the same spot.
	//std::vector<GameObject*> players;
	//std::vector<GameObject*> projectiles;

	inline static std::queue<GameObject*> pendingObjects;
	inline static std::queue<PacketVariant> pendingUpdates;
	inline static std::mutex pendingMutex;

	static void updateGameObjects();
	static glm::vec2 Lerp(const glm::vec2& a, const glm::vec2& b, float t);

	inline static float deltaTime = 0.0f;
	inline static float lastFrametime = 0.0f;
	inline static float currentFrameTime = 0.0f;

	inline static Player* player = nullptr;
	inline static bool spawnEnemies = false; //server controlled enemy spawning toggle

	static Game& getInstance();
	static void init();
	static bool isInitialized();
	static void doDeltaTime(float currentFrameTime);
	static void cleanupInactiveObjects();

	static std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
	static float getTime();

private:
	Game();
	inline static Game* instance = nullptr;
};

#endif