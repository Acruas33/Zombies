#include "Projectile.h"
#include "Game.h"
#include <iostream>

Player::Player(glm::vec2 pos, glm::vec2 size, glm::vec3 color, float rotation, float scale, glm::vec2 velocity, float health)
	: GameObject(pos, size, color, rotation, scale, velocity, health)
{
	texture = "soldier";
	objectType = ObjectType::PLAYER;
	Game::gameObjects.push_back(this);
}

void Player::shoot()
{
	if (fireCooldown > 0.0f)
		fireCooldown -= Game::deltaTime;

	if (fireCooldown <= 0.0f)
	{

		glm::vec2 direction = glm::vec2(cos(m_rotation), sin(m_rotation));
		float offsetDistance = (m_size.y * m_scale) / 2.0f;
		glm::vec2 bulletPosition = m_pos + direction * offsetDistance;
		Projectile* bullet = new Projectile(bulletPosition, glm::vec2(32.0f, 32.0f), glm::vec3(1.0f), m_rotation, 1.5f, glm::vec2(cos(m_rotation) * 500.0f, sin(m_rotation) * 500.0f));
		bullet->updated = true;
		bullet->networkID = this->bulletID++;
		bullet->clientID = this->clientID;
		bullet->previousPos = bulletPosition;
		bullet->targetPos = bulletPosition + bullet->m_velocity; //not sure if this is the best solution but im trying to stop the stagger that happens when bullets get created.
		bullet->t = 0.0f;
		bullet->m_active = true;
		bullet->health = 100.0f;
		//bullet->m_color = glm::vec3(1.0f, 0.0f, 0.0f); // Set bullet color to red

		fireCooldown = fireRate;
	}
}

void Player::update()
{
	// Update position based on velocity
	m_pos.x += m_velocity.x * Game::deltaTime;
	m_pos.y += m_velocity.y * Game::deltaTime;

	updated = true;
}