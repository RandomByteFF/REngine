#include "node3d.hpp"

namespace REngine::Scene {
	Node3D::Node3D(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation){
		this->position = position;
		this->scale = scale;
		this->rotation = glm::quat(rotation);
		ApplyTransforms();
	}

	const glm::mat4 &Node3D::GetModel() {
		return model;
	}

	void Node3D::Position(glm::vec3 position) {
		this->position = std::move(position);
		ApplyTransforms();
	}

	void Node3D::Scale(glm::vec3 scale) {
		this->scale = std::move(scale);
		ApplyTransforms();
	}

	void Node3D::Rotation(glm::vec3 rotation) {
		this->rotation = glm::quat(std::move(rotation));
		ApplyTransforms();
	}

	void Node3D::RotationQuat(glm::quat rotation) {
		this->rotation = rotation;
		ApplyTransforms();
	}

	void Node3D::Rotate(glm::vec3 rotation) {
		this->rotation = glm::normalize(glm::quat(std::move(rotation)) * this->rotation);
		ApplyTransforms();
	}

	glm::vec3 Node3D::Position() const {
		return this->position;
	}
	
	glm::vec3 Node3D::GlobalPosition() const {
		return model[3];
	}

	glm::vec3 Node3D::Scale() const {
		return scale;
	}

	glm::vec3 Node3D::Rotation() const {
		return glm::eulerAngles(rotation);
	}

	glm::quat Node3D::RotationQuat() const {
		return rotation;
	}

	void Node3D::ApplyTransforms() {
		glm::mat4 pModel = GetParent() ? GetParent()->GetModel() : glm::mat4(1.f);
		model = pModel * glm::translate(glm::mat4(1.f), position) * glm::toMat4(rotation) * glm::scale(glm::mat4(1.f), scale);
		for(auto i : children) {
			i->ApplyTransforms();
		}
	}

	glm::vec3 Node3D::Forward() const {
		return model[2];
	}

	glm::vec3 Node3D::Right() const {
		return model[0];
	}
	glm::vec3 Node3D::Up() const {
		return model[1];
	}
}