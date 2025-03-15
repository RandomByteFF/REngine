#pragma once

#include "headers.h"
#include "pipeline.hpp"
#include "buffer.hpp"
#include "image.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct UniformBufferObject {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};

namespace REngine::Core {
	class Object {
		std::vector<vk::DescriptorSet> descriptorSets;
		Pipeline pipeline;
		public:
		std::vector<Buffer> uniformBuffers;
		void Create(Pipeline pipeline);
		void Bind(vk::CommandBuffer cb, uint32_t currentFrame);
		void SetImage(Image image, vk::Sampler sampler);
		void Destroy();
	};
}