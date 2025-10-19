#ifndef ResourceManager_H
#define ResourceManager_H

#include <map>
#include <string>
#include "Texture2D.h"

class ResourceManager
{
public:
	inline static std::map<std::string, Texture2D> textures;

	static ResourceManager& getInstance();
	static void init();

private:
	ResourceManager() = default;
	inline static ResourceManager* instance = nullptr;
};

#endif
