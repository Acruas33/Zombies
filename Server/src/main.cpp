#include<enet/enet.h>
#include <iostream>
#include <vector>
#include "MessageType.h"
#include "Game.h"
#include "Network.h"
//#include <chrono>
#include <thread>
#include "Enemy.h"
#include "PacketBuilder.h"
#include "Projectile.h"

float spawnTimer = 1.0f;
static int windowWidth = 800;
static int windowHeight = 640;

// Generate a random edge position
std::pair<int, int> getRandomEdgePosition()
{
    int edge = rand() % 4; // 0=top, 1=bottom, 2=left, 3=right
    int x = 0, y = 0;

    switch (edge)
    {
    case 0: // Top
        x = rand() % windowWidth;
        y = 0;
        break;
    case 1: // Bottom
        x = rand() % windowWidth;
        y = windowHeight - 1;
        break;
    case 2: // Left
        x = 0;
        y = rand() % windowHeight;
        break;
    case 3: // Right
        x = windowWidth - 1;
        y = rand() % windowHeight;
        break;
    }

    return { x, y };
}

void spawnEnemy()
{
    spawnTimer -= Game::deltaTime;
    if (spawnTimer <= 0.0f)
    {
        auto [x, y] = getRandomEdgePosition();

        Enemy* zombie = new Enemy(glm::vec2(static_cast<float>(x), static_cast<float>(y)), glm::vec2(32.0f, 32.0f));
        zombie->m_scale = 1.5f;
        zombie->networkID = ++Network::networkID;
        zombie->m_active = true;
        zombie->health = 100.0f;
		Network::pendingPackets.push(ObjectPacket{ 0, zombie->networkID, ObjectType::ENEMY, zombie->m_pos.x, zombie->m_pos.y, zombie->m_rotation, zombie->health });
        spawnTimer = 7.5f;
    }

}

int main(int argc, char* argv[])
{

    if (argc > 1) {
        std::cout << argv[3] << std::endl;
    }

    if (Network::init(argv[3]) == 1)
    {
		return EXIT_FAILURE;
    }

    Game::init();

    while (true)
    {
        
        Game::doDeltaTime(Game::getTime());

        if(Game::spawnEnemies) 
            spawnEnemy();

        Network::handleClientUpdates();

        for (GameObject* go : Game::gameObjects)
        {
			Enemy* enemy = dynamic_cast<Enemy*>(go);
			Projectile* projectile = dynamic_cast<Projectile*>(go);
			Player* player = dynamic_cast<Player*>(go);
            if (enemy && enemy->m_active)
            {
				enemy->update();
			}
            else if (player && player->m_active) {
                player->update();
                
            }
            else if (projectile && projectile->m_active) 
            {
                projectile->update();
            }
        }

        Network::packetTimer -= Game::deltaTime;
        Network::sendServerUpdate();

		Game::cleanupInactiveObjects();

    }

    return EXIT_SUCCESS;
}