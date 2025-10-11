#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include "node.hpp"

namespace REngine::Scene {
	class Node3D : public Node {
		glm::vec3 position;
		glm::vec3 scale;
		glm::quat rotation;
		
		glm::mat4 model;
		
		public:
		Node3D(glm::vec3 position = {0.f, 0.f, 0.f}, glm::vec3 scale = {1.f, 1.f, 1.f}, glm::vec3 rotation = {0.f, 0.f, 0.f});
		
		virtual void ApplyTransforms() override;
		virtual const glm::mat4 &GetModel() override;

		void Position(glm::vec3 position);
		void Scale(glm::vec3 scale);
		void Rotation(glm::vec3 rotation);
		void RotationQuat(glm::quat rotation);
		void Rotate(glm::vec3 rotation);

		glm::vec3 Position() const;
		glm::vec3 GlobalPosition() const;
		glm::vec3 Scale() const;
		glm::vec3 Rotation() const;
		glm::quat RotationQuat() const;
		
		glm::vec3 Forward() const;
		glm::vec3 Right() const;
		glm::vec3 Up() const;
		
		VISITOR(Node);
	};
}