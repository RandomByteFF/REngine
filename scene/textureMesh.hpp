#pragma once

#include "core/image.hpp"
#include "scene/mesh.hpp"
#include <filesystem>

namespace REngine::Scene {
	class TextureMesh : public Mesh {
		inline static std::shared_ptr<Core::Pipeline> pipeline;
		inline static uint32_t meshCounter = 0;
		inline static vk::Sampler sampler;
		std::optional<Core::Image> image;

	public:
		TextureMesh() = default;
		TextureMesh(vk::RenderPass rp, std::filesystem::path model, std::filesystem::path texture = "");
		virtual void Create(vk::RenderPass rp, std::vector<Vertex> &vertices, std::vector<uint32_t> &indices) override;
		void SetImage(Core::Image image);
		virtual void Destroy() override;
		static void SetSampler(vk::Sampler sampler) { TextureMesh::sampler = sampler; }
	};
}