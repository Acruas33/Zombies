#include "Camera.h"
#include <iostream>
#include "Window.h"
#include "Game.h"
#include "TileWorld.h"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera()
{
	mode = CameraMode::MENU;
	cameraPos = glm::vec2(0.0f, 0.0f);
	zoom = 1.0f;
}

Camera& Camera::getInstance()
{
	if (instance == nullptr)
	{
		throw std::runtime_error("Camera not initialized! Call Camera::init() first.");
	}
	return *instance;
}

bool Camera::isInitialized()
{
	return instance != nullptr;
}

void Camera::init()
{
	if (instance != nullptr)
	{
		throw std::runtime_error("Camera already initialized!");
	}
	instance = new Camera();
}

void Camera::update()
{
	switch (mode)
	{
	case CameraMode::PLAYER: {
		if (Game::player)
		{
			cameraPos = Game::player->m_pos;

			float halfW = Window::m_width / 2.0f;
			float halfH = Window::m_height / 2.0f;

			float minX = halfW;
			float minY = halfH;
			float maxX = (TileWorld::cols * 32) - halfW;
			float maxY = (TileWorld::rows * 32) - halfH;

			cameraPos.x = glm::clamp(cameraPos.x, minX, maxX);
			cameraPos.y = glm::clamp(cameraPos.y, minY, maxY);
		}

		break;
	}
	case CameraMode::MENU:
		Window& window = Window::getInstance();
		cameraPos = glm::vec2(window.m_width / 2.0f, window.m_height / 2.0f);
		break;
	}

	// cameraPos.x = Game::player->m_pos.x - windowWidth / 2.0f;
	// cameraPos.y = Game::player->m_pos.y - windowHeight / 2.0f;
}

glm::mat4 Camera::getViewMatrix()
{
	zoom = 1.0f;
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::scale(view, glm::vec3(zoom, zoom, 1.0f));

	Window& window = Window::getInstance();
	glm::vec2 halfScreen = glm::vec2(window.m_width / 2.0f, window.m_height / 2.0f);

	return glm::translate(view, glm::vec3(halfScreen - cameraPos, 0.0f));
}