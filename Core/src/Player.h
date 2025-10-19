#ifndef Player_H
#define Player_H

#include "GameObject.h"

class Player : public GameObject
{

public:
    // Inherit the constructor using GameObject's constructor directly
    Player(glm::vec2 pos, glm::vec2 size,  glm::vec3 color = glm::vec3(1.0f), float rotation = 0.0f, float scale = 1.0f, glm::vec2 velocity = glm::vec2(0.0f), float health = 100.0f);

    void shoot();
    void update();

    bool shooting = false;
	float fireCooldown = 0.0f; // Cooldown for shooting
	float fireRate = 0.05f; // Time between shots in seconds
    int bulletID = 0;
};

#endif