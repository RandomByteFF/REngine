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

	public:
		Core::Camera camera;
		Portal();
		virtual void EnteredTree() override;
		void Create(vk::RenderPass rp);
		void SetSampler(vk::Sampler sampler);
		void SetPair(std::shared_ptr<Portal>);
		void UpdateCamera();
		virtual void Update() override;
		virtual void ApplyTransforms() override;
	};
}