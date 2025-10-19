#ifndef Enemy_H
#define Enemy_H

#include "GameObject.h"
#include "Game.h"

class Enemy : public GameObject
{
public:
    // Inherit the constructor using GameObject's constructor directly
    Enemy(glm::vec2 pos, glm::vec2 size, glm::vec3 color = glm::vec3(1.0f), float rotation = 0.0f, float scale = 1.5f, glm::vec2 velocity = glm::vec2(0.0f), float health = 100.0f);

	// Override update
    void update();
};

#endif