#pragma once

#include "headers.h"
#include "pipeline.hpp"
#include "buffer.hpp"
#include "image.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "vertex.hpp"

struct MVP {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};

namespace REngine::Core {
	class Mesh {
		std::vector<vk::DescriptorSet> descriptorSets;
		Pipeline pipeline;
		Buffer vertexBuffer;
		Buffer indexBuffer;
		uint32_t indicesSize = 0;
		std::vector<Buffer> uniformBuffers;
		MVP mvp; 
		bool changed = true;
		
	public:
		void Create(Pipeline pipeline, std::vector<Vertex> &vertices, std::vector<uint32_t> &indices);
		void Bind(vk::CommandBuffer cb);
		void Draw(vk::CommandBuffer cb);
		void SetImage(Image image, vk::Sampler sampler);
		void Rotate(glm::f32 angle, glm::vec3 pivot);
		void Update();
		void Destroy();
	};
}