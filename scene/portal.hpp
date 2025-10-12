#pragma once
#include "core/camera.hpp"
#include "core/pipeline.hpp"
#include "core/renderPass.hpp"
#include "mesh.hpp"
#include "scene/node3d.hpp"
#include "scene/player.hpp"
#include <vulkan/vulkan_structs.hpp>

namespace REngine::Scene{
	class Portal : public Mesh{
		inline static uint32_t portalCounter = 0;
		inline static std::shared_ptr<Core::Pipeline> pipeline;
		Core::RenderPass renderPass;
		vk::ImageMemoryBarrier barrier;
		vk::Sampler sampler;
		std::shared_ptr<Portal> pair;
		std::shared_ptr<Player> player;
		bool wasAhead = false;
		bool wasClose = false;

	public:
		Core::Camera camera;
		Portal();
		virtual void EnteredTree() override;
		void Create(vk::RenderPass rp);
		virtual void Destroy() override;
		virtual void PreDraw(Core::CommandBuffer cb) override;
		void SetSampler(vk::Sampler sampler);
		virtual void Recreate() override;
		void SetPair(std::shared_ptr<Portal>);
		void UpdateCamera();
		virtual void Update() override;
	};
}