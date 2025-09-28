#pragma once
#include "core/pipeline.hpp"
#include "mesh.hpp"
#include "scene/node3d.hpp"

namespace REngine::Scene{
	class Portal : public Mesh{
		inline static uint32_t portalCounter = 0;
		inline static std::shared_ptr<Core::Pipeline> pipeline;
	public:
		void Create(vk::RenderPass rp);
		virtual void Destroy() override;
	};
}