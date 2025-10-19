#ifndef Texture2D_H
#define Texture2D_H
#include <string>
#include <glm/glm.hpp>

class Texture2D 
{
public:
	Texture2D(const char* fileName);
	Texture2D();
	~Texture2D();
	void init();
	void bind();
	glm::vec2 getSize() const { return glm::vec2(width, height); }
	int width, height, nrChannels;

private:
	const char* m_fileName;
	unsigned int texture;
};

#endif