#pragma once

#include "core/camera.hpp"
#include "mesh.hpp"
#include "core/renderPass.hpp"

namespace REngine::Scene {
	class PortalMesh : public Mesh {
		inline static uint32_t portalCounter = 0;
		inline static std::shared_ptr<Core::Pipeline> pipeline;
		
		Core::RenderPass renderPass;
		vk::ImageMemoryBarrier barrier;
		vk::Sampler sampler;
		Core::Camera *renderCam;

	public:
		void Create(vk::RenderPass rp);
		virtual void PreDraw(Core::CommandBuffer cb) override;
		void SetRenderCam(Core::Camera *camera);
		virtual void Recreate() override;
		virtual void Destroy() override;
		void SetSampler(vk::Sampler sampler);
	};
}