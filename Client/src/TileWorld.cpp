#include "TileWorld.h"
#include "Camera.h"
#include "Window.h"

TileWorld::TileWorld(std::vector<Texture2D> tileTextures, int windowWidth, int windowHeight)
    : m_tileTextures(tileTextures), m_windowWidth(windowWidth), m_windowHeight(windowHeight)
{
	init();
}

void TileWorld::init()
{
	// rows = (m_windowHeight / 32) + 1; // Assuming each tile is 32 pixels high
	// cols = (m_windowWidth / 32) + 1; // Assuming each tile is 32 pixels wide
	rows = 34;
	cols = 60;

	world = std::vector<std::vector<int>>(rows, std::vector<int>(cols, 0));

	for (int row = 0; row < rows; ++row)
	{
		for (int col = 0; col < cols; ++col)
		{
			world[row][col] = (row + col) % 2;
		}
	}
}

void TileWorld::draw()
{
	/*if (rows != m_windowHeight / 32 || cols != m_windowWidth / 32)
	{
	    init(); //resize world if window size changed
	}*/

	glm::vec2 cameraPos = Camera::getInstance().cameraPos;
	;

	float left = cameraPos.x - Window::m_width / 2.0f;
	float right = cameraPos.x + Window::m_width / 2.0f;
	float top = cameraPos.y - Window::m_height / 2.0f;
	float bottom = cameraPos.y + Window::m_height / 2.0f;

	Texture2D tex;
	for (int row = 0; row < world.size(); ++row)
	{
		for (int col = 0; col < world[0].size(); ++col)
		{
			glm::vec2 tilePos = glm::vec2(col * 32, row * 32);

			if (tilePos.x + 32 < left || tilePos.x > right || tilePos.y + 32 < top || tilePos.y > bottom)
				continue;

			if (world[row][col] == 0)
				tex = m_tileTextures[0]; // Assuming 0 is white tile
			else
				tex = m_tileTextures[1]; // Assuming 1 is gray tile

			Renderer::DrawSprite(tex, glm::vec3(col * 32, row * 32, 0.0f), glm::vec2(32.0f, 32.0f));
		}
	}
}