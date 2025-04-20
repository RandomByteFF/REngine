#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include "instance.hpp"

namespace REngine::Core {
	class Camera {
		Camera(const Camera &) = delete;
		glm::vec3 position;
		glm::vec3 rotation;
		glm::mat4 view;
		glm::mat4 proj;
		glm::mat4 vp;
		glm::vec3 forward;
		glm::vec3 right;
		glm::vec3 up;
		bool dirty = true;

		void resizedCallback(int width, int height);
	public:
		Camera(float aspect = 1.f, glm::vec3 position = glm::vec3(0.f, 0.f, 0.f));
		void SetPosition(glm::vec3 position);
		glm::vec3 GetPosition();
		glm::vec3 Forward();
		glm::vec3 Right();
		glm::vec3 Up();
		void Rotate(glm::vec3 rotation);
		const glm::mat4 &VP();
	};
}