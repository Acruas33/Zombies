#ifndef UIManager_H
#define UIManager_H

//#include <GLFW/glfw3.h>
#include "imgui.h"
#include "Texture2D.h"
#include "Renderer.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Network.h"
#include "Game.h"

struct mainMenuSettings 
{
	bool isVisible;
	char ipAddress[64];
	Texture2D logo;
};

struct menuSettings 
{
	bool isVisible;
	bool spawnEnemies;
};

class UIManager
{
public:
	static void init(GLFWwindow* window);
	static void renderUI();

	static void toggleMainMenu();
	static void toggleSettingsMenu();

	static ImVec2 getCenterPosition(ImVec2 windowSize);

private:
	UIManager() = default;

	inline static GLFWwindow* window = nullptr;

	static void renderMainMenu();
	inline static mainMenuSettings mainMenuData = { true, "" };

	static void renderSettingsMenu();
	inline static menuSettings settingsMenuData = { false, false };
};

#endif