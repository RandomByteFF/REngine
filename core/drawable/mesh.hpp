#pragma once

#include "core/pipeline.hpp"
#include "core/buffer.hpp"
#include "core/image.hpp"
#include "core/camera.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "core/vertex.hpp"

namespace REngine::Core {
	class Mesh {
		std::vector<vk::DescriptorSet> descriptorSets;
		Pipeline pipeline;
		Buffer vertexBuffer;
		Buffer indexBuffer;
		uint32_t indicesSize = 0;
		std::vector<Buffer> uniformBuffers;
		glm::mat4 model;
		glm::mat4 mvp; 
		
	public:
		void Create(Pipeline pipeline, std::vector<Vertex> &vertices, std::vector<uint32_t> &indices);
		void Bind(vk::CommandBuffer cb);
		void Draw(vk::CommandBuffer cb);
		void SetImage(Image image, vk::Sampler sampler);
		void Rotate(glm::f32 angle, glm::vec3 pivot);
		void Update(Camera &camera);
		void Destroy();
	};
}