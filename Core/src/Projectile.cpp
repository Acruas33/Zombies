#include "Projectile.h"
#include <glm/glm.hpp>
#include "Enemy.h"
#include "Player.h"

Projectile::Projectile(glm::vec2 pos, glm::vec2 size, glm::vec3 color, float rotation, float scale, glm::vec2 velocity, float health)
    : GameObject(pos, size, color, rotation, scale, velocity)
{
    texture = "bullet";
    objectType = ObjectType::PROJECTILE;
    Game::gameObjects.push_back(this);
}

// Custom update logic for projectiles
void Projectile::update()
{
    
    m_pos += m_velocity * Game::deltaTime;
    //check for collision with player.
    for(GameObject* obj : Game::gameObjects)
    {
		Enemy* enemy = dynamic_cast<Enemy*>(obj);
        if (enemy) 
        {
            glm::vec2 enemyPos = enemy->m_pos;
            if (glm::distance(enemyPos, m_pos) < 15.0f)
            {
                enemy->m_color = glm::vec3(1.0f, 0.0f, 0.0f); // Change player color to red on hit
                
                enemy->health -= 10;
                enemy->updated = true;

                health = 0.0f;
                m_active = false;

                return;
            }
        }
    }

    // Optional: destroy or mark inactive if out of bounds
    if (m_pos.x < -100 || m_pos.y < -100 || m_pos.x > 900 || m_pos.y > 700) {
		m_active = false; // Mark as inactive if out of bounds
        health = 0.0f;
    }
    updated = true;
}