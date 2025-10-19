#ifndef TileWorld_H
#define TileWorld_H

#include <vector>
#include "Texture2D.h"
#include "Renderer.h"

class TileWorld {
public:
	TileWorld(std::vector<Texture2D> tileTextures,int windowWidth, int windowHeight);
	~TileWorld();
	void init();
	void draw(Renderer &renderer);

private:
	std::vector<Texture2D> m_tileTextures;
	int m_windowWidth, m_windowHeight;
	std::vector<std::vector<int>> world;
};

#endif