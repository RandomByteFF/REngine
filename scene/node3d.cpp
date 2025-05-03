#include "node3d.hpp"

namespace REngine::Scene {
	Node3D::Node3D(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation){
		this->position = position;
		this->scale = scale;
		this->rotation = glm::quat(rotation);
		ApplyTransforms();
	}

	const glm::mat4 &Node3D::GetModel() {
		ApplyTransforms();
		return model;
	}

	void Node3D::Position(glm::vec3 position) {
		this->position = std::move(position);
		dirty = true;
	}

	void Node3D::Scale(glm::vec3 scale) {
		this->scale = std::move(scale);
		dirty = true;
	}

	void Node3D::Rotate(glm::vec3 rotation) {
		this->rotation = glm::normalize(glm::quat(std::move(rotation)) * this->rotation);
		dirty = true;
	}

	glm::vec3 Node3D::Position() {
		return this->position;
	}

	glm::vec3 Node3D::Scale() {
		return scale;
	}

	void Node3D::ApplyTransforms() {
		if (dirty) {
			dirty = false;
			model = glm::mat4(1.f);
			model = glm::scale(glm::mat4(1.f), scale) * glm::toMat4(rotation) * glm::translate(glm::mat4(1.f), position);
		}
	}
}