#ifndef GameObject_H
#define GameObject_H

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "ObjectType.h"

class GameObject {
public:
	glm::vec2 m_pos, m_size, m_velocity;
	glm::vec3 m_color;
	float dx, dy;
	float m_rotation, m_scale;
	bool m_active;
	int networkID; //unique identifier across all clients.
	int clientID; //owner of the object.
	bool updated = false;

	//for interpolation
	glm::vec2 previousPos;
	glm::vec2 targetPos;
	float t;

	bool synced = false; //whether this object has been synced to the server yet. really only used for projectiles.

	ObjectType objectType;
	std::string texture;

	float health;

	GameObject();
	GameObject(glm::vec2 pos, glm::vec2 size, glm::vec3 color = glm::vec3(1.0f), float rotation = 0.0f, float scale = 1.0f, glm::vec2 velocity = glm::vec2(0.0f), bool active = true);

	//virtual void draw(Renderer &renderer);
	virtual void update();
};

#endif
