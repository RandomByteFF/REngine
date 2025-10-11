#pragma once

#include "core/pipeline.hpp"
#include "core/buffer.hpp"
#include "core/image.hpp"
#include "core/camera.hpp"

#include "core/vertex.hpp"
#include "node3d.hpp"
#include "drawable.hpp"
#include <algorithm>
#include <cstdint>

namespace REngine::Scene {
	class Mesh : public Node3D, public Drawable {
		Core::Buffer vertexBuffer;
		Core::Buffer indexBuffer;
		uint32_t indicesSize = 0;
		std::vector<Core::Buffer> uniformBuffers;
		void Bind(vk::CommandBuffer cb, Core::Camera &camera);
		
		glm::mat4 mvp;
		
	protected:
		std::weak_ptr<Core::Pipeline> pPipeline;
		std::vector<vk::DescriptorSet> descriptorSets;

		virtual void EnteredTree() override;

	public:
		virtual void Create(vk::RenderPass rp, std::vector<Vertex> &vertices, std::vector<uint32_t> &indices);
		void Draw(vk::CommandBuffer cb) override;
		void DrawFromView(vk::CommandBuffer cb, Core::Camera &camera) override;
		virtual void Destroy() override;

		VISITOR(Node3D);
	};
}