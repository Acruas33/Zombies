#include "Renderer.h"
#include "shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Game.h"
#include "ResourceManager.h"

Renderer::Renderer(Shader* shader, int windowWidth, int windowHeight)
	: m_shader(shader),
	m_windowWidth(windowWidth),
	m_windowHeight(windowHeight)
{
	init();
}

void Renderer::drawFrame()
{
	for (GameObject* go : Game::gameObjects)
	{
		draw(ResourceManager::textures[go->texture], *go);
	}
}

void Renderer::DrawSprite(Texture2D &texture, glm::vec2 position, glm::vec2 size,float rotate, glm::vec3 color, float scale)
{   
	m_shader->use();

    glm::mat4 model = glm::mat4(1.0f);
    
	// first translate (transformations are: scale happens first, then rotation, and then final translation happens; reversed order)
	model = glm::translate(model, glm::vec3(position, 0.0f));   

    //model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); // move origin of rotation to center of quad
	model = glm::translate(model, glm::vec3(0.5f * size * scale, 0.0f));
    
	model = glm::rotate(model, rotate, glm::vec3(0.0f, 0.0f, 1.0f)); // then rotate
    
	//model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // move origin back
	model = glm::translate(model, glm::vec3(-0.5f * size * scale, 0.0f));

	model = glm::scale(model, glm::vec3(size * scale, 1.0f)); // last scale

	m_shader->setMatrix4("model", glm::value_ptr(model));

	m_shader->setVec3("spriteColor", glm::value_ptr(color));

	m_shader->setInt("image", 0);

	texture.bind();

	glBindVertexArray(m_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6); 
	glBindVertexArray(0);

}
void Renderer::init()
{
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(spriteVertices), spriteVertices, GL_STATIC_DRAW);

	glBindVertexArray(m_VAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Enable blending once (usually during setup)
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void Renderer::draw(Texture2D& texture, GameObject& gameObject) {
	m_shader->use();

	glm::mat4 model = glm::mat4(1.0f);

	//model = glm::translate(model, glm::vec3(position, 0.0f));  // first translate (transformations are: scale happens first, then rotation, and then final translation happens; reversed order)
	model = glm::translate(model, glm::vec3(gameObject.m_pos, 0.0f));

	//model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); // move origin of rotation to center of quad
	model = glm::translate(model, glm::vec3(0.5f * gameObject.m_size * gameObject.m_scale, 0.0f));

	model = glm::rotate(model, gameObject.m_rotation, glm::vec3(0.0f, 0.0f, 1.0f)); // then rotate

	//model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // move origin back
	model = glm::translate(model, glm::vec3(-0.5f * gameObject.m_size * gameObject.m_scale, 0.0f));

	model = glm::scale(model, glm::vec3(gameObject.m_size * gameObject.m_scale, 1.0f)); // last scale

	m_shader->setMatrix4("model", glm::value_ptr(model));

	m_shader->setVec3("spriteColor", glm::value_ptr(gameObject.m_color));

	m_shader->setInt("image", 0);

	texture.bind();

	glBindVertexArray(m_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

}