#include "Game.h"

std::chrono::time_point<std::chrono::high_resolution_clock> Game::startTime = std::chrono::high_resolution_clock::now();

Game::Game() 
{

}

Game& Game::getInstance() 
{
	if (instance == nullptr) 
	{
		throw std::runtime_error("Game not initialized! Call Game::init() first.");
	}
	return *instance;
}

bool Game::isInitialized() 
{
	return instance != nullptr;
}

void Game::init() 
{
	if (instance != nullptr) {
		throw std::runtime_error("Game already initialized!");
	}

	startTime = std::chrono::high_resolution_clock::now();

	instance = new Game();
}

void Game::doDeltaTime(float currentFrameTime) 
{
	deltaTime = currentFrameTime - lastFrametime;
	lastFrametime = currentFrameTime;
}

float Game::getTime() 
{
	auto now = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> elapsed = now - startTime;
	return elapsed.count();
}

glm::vec2 Game::Lerp(const glm::vec2& a, const glm::vec2& b, float t)
{
    return a + (b - a) * t;
}

void Game::updateGameObjects()
{
    if (Game::player)
    {
        Game::player->update();
    }

    for (GameObject* go : Game::gameObjects)
    {

        if (Game::player && go->networkID != Game::player->networkID)
        {

            if (go->objectType == ObjectType::PROJECTILE) //  && go->m_active
            {
                //go->update();

                go->t += Game::deltaTime * 5.0f;
                go->t = std::min(go->t, 1.0f);
                go->m_pos = Lerp(go->previousPos, go->targetPos, go->t);

                //std::cout << go->m_pos.x << ", " << go->m_pos.y << std::endl;

                if (go->health <= 0.0f)
                {
                    go->m_active = false;
                }
            }
            else if (go->objectType == ObjectType::ENEMY && go->m_active)
            {
                go->t += Game::deltaTime * 5.0f;
                go->t = std::min(go->t, 1.0f);
                go->m_pos = Lerp(go->previousPos, go->targetPos, go->t);

                if (go->health <= 0.0f)
                {
                    go->m_active = false;
                }
            }
            else if (go->objectType == ObjectType::PLAYER && go->m_active)
            {
                go->t += Game::deltaTime * 5.0f;
                go->t = std::min(go->t, 1.0f);
                go->m_pos = Lerp(go->previousPos, go->targetPos, go->t);
            }

        }
    }
}

void Game::cleanupInactiveObjects()
{
	//handle removal of inactive game objects
	auto newEnd = std::remove_if(Game::gameObjects.begin(), Game::gameObjects.end(),
		[](GameObject* obj) {
			if (!obj->m_active && !obj->updated)
			{
				delete obj;
				return true;
			}
			return false;
		});

	Game::gameObjects.erase(newEnd, Game::gameObjects.end());
}