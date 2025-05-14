#include "node3d.hpp"

namespace REngine::Scene {
	Node3D::Node3D(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation){
		this->position = position;
		this->scale = scale;
		this->rotation = glm::quat(rotation);
	}

	const glm::mat4 &Node3D::GetModel() {
		return model;
	}

	void Node3D::Position(glm::vec3 position) {
		this->position = std::move(position);
	}

	void Node3D::Scale(glm::vec3 scale) {
		this->scale = std::move(scale);
	}

	void Node3D::Rotation(glm::vec3 rotation) {
		this->rotation = glm::quat(std::move(rotation));
	}

	void Node3D::RotationQuat(glm::quat rotation) {
		this->rotation = rotation;
	}

	void Node3D::Rotate(glm::vec3 rotation) {
		this->rotation = glm::normalize(glm::quat(std::move(rotation)) * this->rotation);
	}

	glm::vec3 Node3D::Position() {
		return this->position;
	}

	glm::vec3 Node3D::Scale() {
		return scale;
	}

	glm::vec3 Node3D::Rotation() {
		return glm::eulerAngles(rotation);
	}

	glm::quat Node3D::RotationQuat() {
		return rotation;
	}

	void Node3D::ApplyTransforms(glm::mat4 &parentMatrix) {
		// TODO: should ensure that root is not null
		model = parentMatrix * glm::translate(glm::mat4(1.f), position) * glm::toMat4(rotation) * glm::scale(glm::mat4(1.f), scale);
		for(auto i : children) {
			i->ApplyTransforms(model);
		}
	}
}