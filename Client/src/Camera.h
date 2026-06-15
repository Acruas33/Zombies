#ifndef Camera_H
#define Camera_H

#include <glm/glm.hpp>

enum class CameraMode
{
	PLAYER,
	MENU
};

class Camera
{
public:
	static Camera& getInstance();
	static bool isInitialized();
	static void init();

	glm::vec2 cameraPos;
	float zoom;
	void update();
	glm::mat4 getViewMatrix();
	CameraMode mode;

private:
	Camera();
	~Camera();
	static inline Camera* instance = nullptr;
};

#endif