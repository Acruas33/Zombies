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
	void draw();

	int m_windowWidth, m_windowHeight;

private:
	static inline int rows, cols = 0;
	std::vector<Texture2D> m_tileTextures;
	std::vector<std::vector<int>> world;
};

#endif