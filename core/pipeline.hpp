#pragma once

#include "headers.h"
#include "swapchain.hpp"

namespace REngine::Core {
	enum Descriptor {
		UniformBuffer,
		Sampler
	};

	class Pipeline {
		public:
		vk::PipelineLayout layout;
		vk::Pipeline pipeline;
		vk::DescriptorSetLayout descriptorLayout;
		void Create(const char *vertShader, const char *fragShader, Swapchain &swapchain, vk::RenderPass renderPass);
		void SetLayout(std::vector<std::pair<vk::DescriptorType, vk::ShaderStageFlags>> descriptors);
		void Destroy();
	};
}