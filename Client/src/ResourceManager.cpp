#include "ResourceManager.h"
#include <iostream>

void ResourceManager::init()
{
	if (instance != nullptr) {
		throw std::runtime_error("ResourceManager already initialized!");
	}
	instance = new ResourceManager();

	// Load textures
	textures["zombie"] = Texture2D("resources\\zombie.png");
	textures["soldier"] = Texture2D("resources\\soldier005.png");
	textures["bullet"] = Texture2D("resources\\Shot4.png");
}

ResourceManager& ResourceManager::getInstance()
{
	if (instance == nullptr) {
		throw std::runtime_error("ResourceManager not initialized! Call ResourceManager::init() first.");
	}
	return *instance;
}