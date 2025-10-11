#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include "glm/ext/quaternion_float.hpp"
#include "instance.hpp"
#include "scene/node3d.hpp"

namespace REngine::Core {
	class Camera : public Scene::Node3D {
		Camera(const Camera &) = delete;
		glm::mat4 view;
		glm::mat4 proj;
		glm::mat4 vp;
		float aspect = 1.;
		bool dirty = true;

		void resizedCallback(int width, int height);
	public:
		Camera(float aspect = 1.f, glm::vec3 position = glm::vec3(0.f, 0.f, 0.f), bool ignoreResize = false);
		~Camera();
		const glm::mat4 &VP();
		const glm::mat4 &V();
		const glm::mat4 &P();
		void Aspect(float aspect);
		float Aspect();
		void Orbit(glm::vec3 lookAt, float distance, glm::vec2 rotation);
		
		virtual void ApplyTransforms() override;
	};
}