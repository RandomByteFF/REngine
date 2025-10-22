#pragma once

#include "core/camera.hpp"
#include "mesh.hpp"
#include "core/renderPass.hpp"
#include "scene/node.hpp"

namespace REngine::Scene {
	class PortalMesh : public Mesh {
		inline static uint32_t portalCounter = 0;
		inline static std::shared_ptr<Core::Pipeline> pipeline;
		
		Core::RenderPass renderPass;
		vk::ImageMemoryBarrier barrier;
		vk::Sampler sampler;
		Core::Camera *renderCam = nullptr;

	public:
		void Create();
		void EnteredTree() override;
		virtual void PreDraw(Core::CommandBuffer cb) override;
		void SetRenderCam(Core::Camera *camera);
		virtual void Recreate() override;
		virtual void Destroy() override;
		VISITOR(Mesh);
	};
}