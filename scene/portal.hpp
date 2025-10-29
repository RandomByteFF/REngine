#pragma once
#include "core/camera.hpp"
#include "core/pipeline.hpp"
#include "core/renderPass.hpp"
#include "mesh.hpp"
#include "portalMesh.hpp"
#include "scene/node3d.hpp"
#include "scene/player.hpp"
#include <vulkan/vulkan_structs.hpp>

namespace REngine::Scene{
	class Portal : public Node3D{
		std::shared_ptr<PortalMesh> portalMesh;
		std::shared_ptr<Portal> pair;
		std::shared_ptr<Player> player;
		bool wasAhead = false;
		bool wasClose = false;

		glm::mat4 modelNoScale = glm::mat4(1.f);
		float zOffset = 0.f;
		bool didTeleport = false;

	public:
		Core::Camera camera;
		Portal();
		virtual void EnteredTree() override;
		void SetPair(std::shared_ptr<Portal>);
		std::shared_ptr<Portal> GetPair();
		void UpdateCamera();
		virtual void Update() override;
		virtual void ApplyTransforms() override;
		virtual void AddChild(std::shared_ptr<Node> child) override;
		bool DidTeleport();

		VISITOR(Node3D);
	};
}