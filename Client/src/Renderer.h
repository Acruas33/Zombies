#ifndef Renderer_H
#define Renderer_H

#include "shader.h"
#include <vector>
#include <glm/glm.hpp>
#include "Texture2D.h"
#include "GameObject.h"

class Renderer {
public:
	
	static void init();
	static void draw(Texture2D& texture, GameObject& gameObject);
	static void drawFrame();
	static void cleanup();
	static void DrawSprite(Texture2D &texture, glm::vec2 position, glm::vec2 size = glm::vec2(10.0f, 10.0f), float rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f), float scale = 1.0f);

	static inline Shader* m_shader = nullptr;

private:
	static Renderer* instance;
	Renderer();
	~Renderer();
	static bool isInitialized();

	static inline unsigned int m_VAO = 0;
	static inline unsigned int m_VBO = 0;
	static inline unsigned int m_EBO = 0;
	static inline unsigned int m_texture = 0;

	static constexpr unsigned int indices[] =
	{
		0,1,3, //top triangle
		1,2,3 //bottom triangle
	};

	static constexpr float spriteVertices[] = {
		// pos      // tex
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};
};

#endif