#pragma once
#include "core/pipeline.hpp"
#include "core/renderPass.hpp"
#include "mesh.hpp"
#include "scene/node3d.hpp"
#include <vulkan/vulkan_structs.hpp>

namespace REngine::Scene{
	class Portal : public Mesh{
		inline static uint32_t portalCounter = 0;
		inline static std::shared_ptr<Core::Pipeline> pipeline;
		Core::RenderPass renderPass;
		vk::ImageMemoryBarrier barrier;
		vk::Sampler sampler;
	public:
		void Create(vk::RenderPass rp);
		virtual void Destroy() override;
		virtual void PreDraw(Core::CommandBuffer cb) override;
		void SetSampler(vk::Sampler sampler);
		virtual void Recreate() override;
	};
}