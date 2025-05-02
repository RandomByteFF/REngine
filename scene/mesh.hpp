#pragma once

#include "core/pipeline.hpp"
#include "core/buffer.hpp"
#include "core/image.hpp"
#include "core/camera.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "core/vertex.hpp"
#include "node3d.hpp"

namespace REngine::Scene {
	class Mesh : public Node3D {
		std::vector<vk::DescriptorSet> descriptorSets;
		Core::Pipeline pipeline;
		Core::Buffer vertexBuffer;
		Core::Buffer indexBuffer;
		uint32_t indicesSize = 0;
		std::vector<Core::Buffer> uniformBuffers;
		glm::mat4 model;
		glm::mat4 mvp; 
		void Bind(vk::CommandBuffer cb);
		
	public:
		void Create(Core::Pipeline pipeline, std::vector<Vertex> &vertices, std::vector<uint32_t> &indices);
		void Draw(vk::CommandBuffer cb);
		void SetImage(Core::Image image, vk::Sampler sampler);
		void Rotate(glm::f32 angle, glm::vec3 pivot);
		void Destroy();
	};
}