#pragma once

#include "core/pipeline.hpp"
#include "core/buffer.hpp"
#include "core/image.hpp"
#include "core/camera.hpp"

#include "core/vertex.hpp"
#include "node3d.hpp"
#include "drawable.hpp"

namespace REngine::Scene {
	class Mesh : public Node3D, public Drawable {
		std::vector<vk::DescriptorSet> descriptorSets;
		Core::Pipeline pipeline;
		Core::Buffer vertexBuffer;
		Core::Buffer indexBuffer;
		uint32_t indicesSize = 0;
		std::vector<Core::Buffer> uniformBuffers;
		void Bind(vk::CommandBuffer cb);
		
		glm::mat4 mvp;
		
	public:
		void Create(Core::Pipeline pipeline, std::vector<Vertex> &vertices, std::vector<uint32_t> &indices);
		void Draw(vk::CommandBuffer cb) override;
		void SetImage(Core::Image image, vk::Sampler sampler);
		void Destroy() override;
	};
}