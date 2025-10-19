#include "Enemy.h"
#include <glm/glm.hpp>
#include "Player.h"
#include <iostream>

Enemy::Enemy(glm::vec2 pos, glm::vec2 size, glm::vec3 color, float rotation, float scale, glm::vec2 velocity, float health)
    : GameObject(pos, size, color, rotation, scale, velocity, health)
{
    texture = "zombie";
    objectType = ObjectType::ENEMY;
    Game::gameObjects.push_back(this);
}

// Custom update logic for projectiles
void Enemy::update()
{
    float closest = 10000.0f;
    GameObject* closestPlayer = nullptr;

    //check for collision with player.
    for (GameObject* go : Game::gameObjects)
    {
		Player* player = dynamic_cast<Player*>(go);
        if (player)
        {
            glm::vec2 playerPos = player->m_pos;

            if (glm::distance(playerPos, m_pos) < closest)
            {
                closestPlayer = player;
				closest = glm::distance(playerPos, m_pos);
            }
        }

    }

	if (closestPlayer != nullptr)
	{
        glm::vec2 toPlayer = closestPlayer->m_pos - m_pos;
        float distance = glm::length(toPlayer);
        if (distance > 0.001f) {
            glm::vec2 direction = glm::normalize(closestPlayer->m_pos - m_pos);
            m_velocity = direction * 100.0f; // Adjust speed as needed
        }
        else {
            m_velocity = glm::vec2(0.0f);
        }
        
        float dx = closestPlayer->m_pos.x - m_pos.x;
        float dy = closestPlayer->m_pos.y - m_pos.y;
        m_rotation = atan2(dy, dx);
	}

    m_pos += m_velocity * Game::deltaTime;
    
    if (health < 0.0f)
    {
		m_active = false;
    }

    updated = true;

}