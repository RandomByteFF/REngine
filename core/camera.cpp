#include "camera.hpp"
#include "scene/node3d.hpp"

namespace REngine::Core {
	void Camera::resizedCallback(int width, int height) {
		Aspect((float) width/height);
	}

	Camera::Camera(float aspect, glm::vec3 position, bool ignoreResize) : Scene::Node3D(position, glm::vec3(1., 1., 1.), glm::vec3()) {
		proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f); 
		proj[1][1] *= -1;
		Rotation(glm::vec3());
		if (!ignoreResize) {
			auto cb = std::bind(&Camera::resizedCallback, this, std::placeholders::_1, std::placeholders::_2);
			Instance::OnResize(this, cb);
		}
		Rotate(glm::vec3());
	}

	Camera::~Camera() {
		Instance::UnsubscribeResize(this);
	}

	const glm::mat4 &Camera::VP() {
		return vp;
	}

	void Camera::Orbit(glm::vec3 lookAt, float distance, glm::vec2 rotation) {
		glm::vec3 pos(0.f, 0.f, distance);
		Position(glm::rotate(glm::quat(glm::vec3(rotation.y, rotation.x, 0.f)), pos) + lookAt);
		RotationQuat(glm::quat_cast(glm::inverse(glm::lookAt(Position(), lookAt, glm::vec3(0., 1., 0.)))));
	}

	const glm::mat4 &Camera::V() {
		if (dirty) VP();
		return view;
	}

	const glm::mat4 &Camera::P() {
		return proj;
	}

	void Camera::Aspect(float aspect) {
		proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
		proj[1][1] *= -1;
		dirty = true;
		this->aspect = aspect;
		ApplyTransforms();
	}

	float Camera::Aspect() {
		return aspect;
	}


	void Camera::ApplyTransforms() {
		Scene::Node3D::ApplyTransforms();
		view = glm::inverse(GetModel());
		vp = proj * view;
	}
}