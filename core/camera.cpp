#include "camera.hpp"
#include <iostream>
namespace REngine::Core {
	void Camera::resizedCallback(int width, int height) {
		proj = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f);
		proj[1][1] *= -1;
		dirty = true;
	}

	Camera::Camera(float aspect, glm::vec3 position) {
		proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f); 
		// TODO: here i probably need to query the current extent from someone. 
		// I need this to be the render target's, not the swapchain, not the window.
		proj[1][1] *= -1;
		rotation = glm::vec3();
		auto cb = std::bind(&Camera::resizedCallback, this, std::placeholders::_1, std::placeholders::_2);
		Instance::OnResize(this, cb);
	}

	Camera::~Camera() {
		Instance::UnsubscribeResize(this);
	}

	const glm::mat4 &Camera::VP()
	{
		if (dirty) {
			glm::vec3 up_dir = glm::vec3(0.f, 1.f, 0.f);
			// if (abs(forward.y) < 0.01) up_dir = glm::normalize(glm::vec3(0.f, 0.99f, 0.1f)); // Calculate normally
			view = glm::lookAt(position, position + forward, up_dir);
			vp = proj * view;
			dirty = false;
		}
		return vp;
	}

	const glm::mat4 &Camera::V() {
		if (dirty) VP();
		return view;
	}

	const glm::mat4 &Camera::P() {
		return proj;
	}

	void Camera::SetPosition(glm::vec3 position) {
		this->position = position;
		dirty = true;
	}

	glm::vec3 Camera::GetPosition() {
		return position;
	}

	glm::vec3 Camera::Forward() {
		return forward;
	}

	glm::vec3 Camera::Right() {
		return right;
	}

	glm::vec3 Camera::Up() {
		return up;
	}

	void Camera::Rotate(glm::vec3 delta) {
		this->rotation += delta;
		dirty = true;

		forward = glm::vec3(0.f, 0.f, -1.f);
		forward = glm::vec3(glm::rotate(glm::mat4(1.f), rotation.x, glm::vec3(1.f, 0.f, 0.f)) * glm::vec4(forward, 1.f));
		forward = glm::vec3(glm::rotate(glm::mat4(1.f), rotation.y, glm::vec3(0.f, 1.f, 0.f)) * glm::vec4(forward, 1.f));
		glm::vec3 up_dir = glm::vec3(0.f, 1.f, 0.f);
		// if (abs(forward.y) < 0.01) up_dir = glm::normalize(glm::vec3(0.f, 0.99f, 0.1f)); // Calculate normally
		right = glm::cross(forward, up_dir);
		up = glm::cross(right, forward);
	}
}