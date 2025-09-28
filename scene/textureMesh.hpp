#pragma once

#include "scene/mesh.hpp"

namespace REngine::Scene {
	class TextureMesh : public Mesh {
		inline static std::shared_ptr<Core::Pipeline> pipeline;
		inline static uint32_t meshCounter = 0;

	public:
		virtual void Create(vk::RenderPass rp, std::vector<Vertex> &vertices, std::vector<uint32_t> &indices) override;
		void SetImage(Core::Image image, vk::Sampler sampler);
		virtual void Destroy() override;
	};
}