#ifndef Projectile_H
#define Projectile_H

#include "GameObject.h"
#include "Game.h"

class Projectile : public GameObject
{
    //Texture2D sprite, sprite,
public:
    // Inherit the constructor using GameObject's constructor directly
    Projectile(glm::vec2 pos, glm::vec2 size, glm::vec3 color = glm::vec3(1.0f), float rotation = 0.0f, float scale = 1.5f, glm::vec2 velocity = glm::vec2(0.0f), float health = 100.0f);

    // Override update
    void update();
};

#endif