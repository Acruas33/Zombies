#include "TileWorld.h"

TileWorld::TileWorld(std::vector<Texture2D> tileTextures, int windowWidth, int windowHeight)
	: m_tileTextures(tileTextures), m_windowWidth(windowWidth), m_windowHeight(windowHeight)
{
	init();
}

void TileWorld::init()
{
	int rows = m_windowHeight / 32; // Assuming each tile is 32 pixels high
	int cols = m_windowWidth / 32; // Assuming each tile is 32 pixels wide

	world = std::vector<std::vector<int>>(rows, std::vector<int>(cols, 0));

	for (int row = 0; row < rows; ++row)
	{
		for (int col = 0; col < cols; ++col)
		{
			world[row][col] = (row + col) % 2;
		}
	}
}

void TileWorld::draw(Renderer &renderer)
{
	Texture2D tex;
	for (int row = 0; row < world.size(); ++row)
	{
		for (int col = 0; col < world[0].size(); ++col)
		{
			if (world[row][col] == 0)
				tex = m_tileTextures[0]; // Assuming 0 is white tile
			else
				tex = m_tileTextures[1]; // Assuming 1 is gray tile

			renderer.DrawSprite(tex, glm::vec3(col * 32, row * 32, 0.0f), glm::vec2(32.0f, 32.0f));
		}
	}
}