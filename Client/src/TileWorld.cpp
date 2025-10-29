#include "TileWorld.h"

TileWorld::TileWorld(std::vector<Texture2D> tileTextures, int windowWidth, int windowHeight)
	: m_tileTextures(tileTextures), m_windowWidth(windowWidth), m_windowHeight(windowHeight)
{
	init();
}

void TileWorld::init()
{
	rows = (m_windowHeight / 32) + 1; // Assuming each tile is 32 pixels high
	cols = (m_windowWidth / 32) + 1; // Assuming each tile is 32 pixels wide

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
	if (rows != m_windowHeight / 32 || cols != m_windowWidth / 32)
	{
		init(); //resize world if window size changed
	}

	Texture2D tex;
	for (int row = 0; row < world.size(); ++row)
	{
		for (int col = 0; col < world[0].size(); ++col)
		{
			if (world[row][col] == 0)
				tex = m_tileTextures[0]; // Assuming 0 is white tile
			else
				tex = m_tileTextures[1]; // Assuming 1 is gray tile

			Renderer::DrawSprite(tex, glm::vec3(col * 32, row * 32, 0.0f), glm::vec2(32.0f, 32.0f));
		}
	}
}