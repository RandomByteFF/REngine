#include "camera.hpp"
#include <iostream>
namespace REngine::Core {
	void Camera::resizedCallback(int width, int height) {
		proj = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 10.0f);
		proj[1][1] *= -1;
		dirty = true;
	}

	Camera::Camera() : Camera(glm::vec3(0.f, 0.f, 0.f)){}
	
	Camera::Camera(glm::vec3 position) {
		proj = glm::perspective(glm::radians(45.0f), float(800 / 600), 0.1f, 10.0f); 
		// TODO: here i probably need to query the current extent from someone. 
		// I need this to be the render target's, not the swapchain, not the window.
		proj[1][1] *= -1;
		rotation = glm::vec3();
		auto cb = std::bind(&Camera::resizedCallback, this, std::placeholders::_1, std::placeholders::_2);
		Instance::OnResize(cb);
	}
	
	const glm::mat4 &Camera::VP()
	{
		if (dirty) {
			glm::vec3 direction = glm::vec3(0.f, 0.f, -1.f);
			direction = glm::vec3(glm::rotate(glm::mat4(1.f), rotation.y, glm::vec3(0.f, 1.f, 0.f)) * glm::vec4(direction, 1.f));
			direction = glm::vec3(glm::rotate(glm::mat4(1.f), rotation.x, glm::vec3(1.f, 0.f, 0.f)) * glm::vec4(direction, 1.f));
			glm::vec3 up = glm::vec3(0.f, 1.f, 0.f);
			if (abs(direction.y) < 0.01) up = glm::normalize(glm::vec3(0.f, 0.99f, 0.1f)); // Calculate normally
			view = glm::lookAt(position, position + direction, up);
			vp = proj * view;
			dirty = false;
		}
		return vp;
	}

	void Camera::SetPosition(glm::vec3 position) {
		this->position = position;
		dirty = true;
	}
	
	void Camera::Rotate(glm::vec3 rotation) {
		this->rotation += rotation;
		dirty = true;
	}
}