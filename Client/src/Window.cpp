#include "Window.h"
#include <iostream>
#include "Game.h"
#include <glm/glm.hpp>
#include "Camera.h"

Window::Window()
{
}

Window& Window::getInstance()
{
	if (instance == nullptr)
	{
		throw std::runtime_error("Window not initialized! Call Window::init() first.");
	}
	return *instance;
}

bool Window::isInitialized()
{
	return instance != nullptr;
}

void Window::init(int width, int height)
{
	if (instance != nullptr)
	{
		throw std::runtime_error("Window already initialized!");
	}
	instance = new Window();
	instance->m_width = width;
	instance->m_height = height;
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, "Zombies++", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(0);
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(0);
    }

    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPosCallback(window, mouseCallback);
}

void Window::mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	
    if (Game::player != nullptr)
    {
        glm::vec2 halfScreen = glm::vec2(m_width / 2.0f, m_height / 2.0f);
        //glm::vec2 worldMouse = glm::vec2(xpos, ypos) - halfScreen + Game::player->m_pos;
		Camera& camera = Camera::getInstance();
        glm::vec2 worldMouse = (glm::vec2(xpos, ypos) / camera.zoom) + (camera.cameraPos - halfScreen / camera.zoom);

        float dx = worldMouse.x - (Game::player->m_pos.x);
        float dy = worldMouse.y - (Game::player->m_pos.y);
        float angleRadians = atan2(dy, dx);
        Game::player->m_rotation = angleRadians;
    }
}

void Window::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    m_width = width;
    m_height = height;
    glViewport(0, 0, width, height);
}