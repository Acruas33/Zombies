#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cstdlib>   // rand, srand
#include <ctime>     // time
#include <utility>   // std::pair
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#include "Renderer.h"
#include "Texture2D.h"
#include "TileWorld.h"
#include "GameObject.h"
#include "Projectile.h"
#include "Player.h"
#include "Enemy.h"
#include "Network.h"
#include <thread>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <enet/enet.h>
#include <optional>
#include <mutex>
#include <queue>
#include "Game.h"
#include "ResourceManager.h"
#include "PacketBuilder.h"
#include <algorithm>
#define IMGUI_IMPL_OPENGL_LOADER_GLAD  // if you're using GLAD
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "UIManager.h"
#include "Camera.h"
#include "Window.h"

TileWorld* tileWorld;

bool isHost = false;



void processInput(GLFWwindow* window) 
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        UIManager::toggleSettingsMenu(); //glfwSetWindowShouldClose(window, true);
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        if(Game::player != nullptr)
            Game::player->m_velocity.y = -125.0f;
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        if (Game::player != nullptr)
            Game::player->m_velocity.y = 125.0f;
    }
    else 
    {
        if (Game::player != nullptr)
            Game::player->m_velocity.y = 0.0f;
    }
        
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        if (Game::player != nullptr)
            Game::player->m_velocity.x = -125.0f;
    }
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        if (Game::player != nullptr)
            Game::player->m_velocity.x = 125.0f;
    }
    else
    {
        if (Game::player != nullptr)
    		Game::player->m_velocity.x = 0.0f;
    }
    
    if (Game::player != nullptr) 
    {
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {
            Game::player->shoot();
        }
    }
}

int main(int argc, char* argv[])
{   
    /*if (argc == 2 && std::strcmp(argv[1], "host") == 0)
        isHost = true;*/
    
    //initWindow();
	Window::init(1280, 720);
    Camera::init();
    
    Renderer::init();
    Game::init();
    ResourceManager::init();
    
	tileWorld = new TileWorld({ Texture2D("resources\\whiteTile.png"), Texture2D("resources\\grayTile.png") }, Window::m_width, Window::m_height);

	UIManager::init(Window::getInstance().window);

    while (!glfwWindowShouldClose(Window::getInstance().window))
    {
        glfwPollEvents();

        //should prolly move dis shit inside the renderer somehow.
        //width, height are the second and third values
        glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(Window::m_width), static_cast<float>(Window::m_height), 0.0f, -1.0f, 1.0f);
        Renderer::m_shader->setMatrix4("projection", glm::value_ptr(projection)); //create method for this in the renderer probably.

        Camera::getInstance().update();

        //glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraPosa, 0.0f));
        glm::mat4 view = Camera::getInstance().getViewMatrix();
        Renderer::m_shader->setMatrix4("view", value_ptr(view));

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
		  tileWorld->m_windowHeight = Window::m_height;
		  tileWorld->m_windowWidth = Window::m_width;
        tileWorld->draw();

        if (Network::startGame)
        {
            Game::doDeltaTime(Game::getTime());
            Network::clientTime += Game::deltaTime; //update clientTime for network interpolation stuffs.
            processInput(Window::getInstance().window);

            // Handle incoming network updates
            Network::handleServerUpdates();

            Game::updateGameObjects();

            Renderer::drawFrame();
            
            Network::sendServerUpdate();
            Game::cleanupInactiveObjects();
        }

        UIManager::renderUI();

        glfwSwapBuffers(Window::getInstance().window);
    }

    glfwTerminate();
    return 0;
}

//Messing around with limiting fps
/*float endFrameTime = glfwGetTime();
float frameDuration = endFrameTime - currentFrameTime;

if (frameDuration < (1.0f / 165.0f))
{
    glfwWaitEventsTimeout((1.0f / 165.0f) - frameDuration);
}*/