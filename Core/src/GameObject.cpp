#include "GameObject.h"

//Texture2D sprite,m_sprite(sprite),
GameObject::GameObject(glm::vec2 pos, glm::vec2 size,  glm::vec3 color, float rotation, float scale, glm::vec2 velocity, bool active)
	: m_pos(pos), m_size(size), m_color(color), m_rotation(rotation), m_scale(scale), m_velocity(velocity), m_active(active)
{

}

void GameObject::update()
{
	// Update position based on velocity
	m_pos += m_velocity;
	updated = true;
}