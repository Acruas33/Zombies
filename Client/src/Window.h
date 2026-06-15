#ifndef Window_H
#define Window_H

#include <utility>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Window
{
public:
	static Window& getInstance();
	static bool isInitialized();
	static void init(int width, int height);

	static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
	static void mouseCallback(GLFWwindow* window, double xpos, double ypos);

	std::pair<int, int> getRandomEdgePosition();

	inline static GLFWwindow* window = nullptr;
	inline static int m_width = 0;
	inline static int m_height = 0;

private:
	Window();
	~Window();
	static inline Window* instance = nullptr;
};

#endif