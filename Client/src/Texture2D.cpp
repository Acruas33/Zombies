#include "Texture2D.h"
#include "shader.h"
#include "stb_image.h"

Texture2D::Texture2D(const char* fileName)
	: m_fileName(fileName)
{
	init();
}
Texture2D::Texture2D()
{
	m_fileName = nullptr;
	width = 0;
	height = 0;
	nrChannels = 0;
	texture = 0;
}

Texture2D::~Texture2D()
{

}
void Texture2D::init()
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned char* data = stbi_load(m_fileName, &width, &height, &nrChannels, 0);
	if (data)
	{
		if (nrChannels == 3)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		} 
		else if (nrChannels == 4)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		
		std::cout << "Failed to load texture: " << stbi_failure_reason() << std::endl;
	}
	stbi_image_free(data);

	bind();
}

void Texture2D::bind()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
}