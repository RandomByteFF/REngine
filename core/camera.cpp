#include "camera.hpp"
#include <iostream>
namespace REngine::Core {
	void Camera::resizedCallback(int width, int height) {
		proj = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 10.0f);
		proj[1][1] *= -1;
		dirty = true;
	}

	Camera::Camera() : Camera(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f)){}
	
	Camera::Camera(glm::vec3 position, glm::vec3 lookAt) {
		SetLook(position, lookAt);
		proj = glm::perspective(glm::radians(45.0f), float(800 / 600), 0.1f, 10.0f);
		proj[1][1] *= -1;
		auto cb = std::bind(&Camera::resizedCallback, this, std::placeholders::_1, std::placeholders::_2);
		Instance::OnResize(cb);
	}
	
	const glm::mat4 &Camera::VP()
	{
		if (dirty) {
			vp = proj * view;
			dirty = false;
		}
		return vp;
	}

	void Camera::SetLook(glm::vec3 position, glm::vec3 lookAt) {
		glm::vec3 up(0.f, 1.f, 0.f);
		if (position.y - lookAt.y == 0.f) up = glm::vec3(0.f, 0.99f, 0.f);
		view = glm::lookAt(position, lookAt, glm::vec3(0.f, 1.f, 0.f));
		dirty = true;
	}
}