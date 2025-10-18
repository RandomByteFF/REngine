#include "camera.hpp"
#include "glm/ext/quaternion_geometric.hpp"
#include "glm/geometric.hpp"
#include "scene/node3d.hpp"
#include "common/math.hpp"
#include <glm/gtc/matrix_access.hpp>

namespace REngine::Core {
	void Camera::resizedCallback(int width, int height) {
		Aspect((float) width/height);
	}

	Camera::Camera(float aspect, glm::vec3 position, bool ignoreResize) : Scene::Node3D(position, glm::vec3(1., 1., 1.), glm::vec3()) {
		Aspect(aspect);
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

	// https://www.terathon.com/lengyel/Lengyel-Oblique.pdf
	void Camera::ObliqueMatrix(glm::vec4 plane) {
		Aspect(aspect);

		glm::vec4 C = plane;

		glm::vec4 Cp = glm::transpose(glm::inverse(P())) * C;
		
		glm::vec4 Qp = glm::vec4(Math::sgn(Cp.x), Math::sgn(Cp.y), 1.f, 1.f);
		glm::vec4 Q = glm::inverse(P()) * Qp;
		
		glm::vec4 M3p = ((-2.f * Q.z) / glm::dot(C, Q)) * C + glm::vec4(0.f, 0.f, 1.f, 0.f);
		proj = glm::row(proj, 2, M3p);
		glm::mat4 S(1.0f);
		S[2][2] = 0.5f;
		S[3][2] = 0.5f;
		proj = S * proj;
		
		ApplyTransforms();
	}

	void Camera::ApplyTransforms() {
		Scene::Node3D::ApplyTransforms();
		view = glm::inverse(GetModel());
		vp = proj * view;
		auto xd = vp * GetModel();
	}
}