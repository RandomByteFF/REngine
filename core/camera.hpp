#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include "instance.hpp"

namespace REngine::Core {
	class Camera {
		Camera(const Camera &) = delete;
		Camera &operator =(const Camera &) = delete;
		glm::mat4 view;
		glm::mat4 proj;
		glm::mat4 vp;
		bool dirty = true;

		void resizedCallback(int width, int height);
	public:
		Camera();
		Camera(glm::vec3 position, glm::vec3 lookAt);
		void SetLook(glm::vec3 position, glm::vec3 lookAt);
		const glm::mat4 &VP();
	};
}