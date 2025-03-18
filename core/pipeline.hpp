#pragma once

#include "headers.h"
#include "swapchain.hpp"

namespace REngine::Core {
	enum Descriptor {
		UniformBuffer,
		Sampler
	};

	// TODO: inherit layout from other pipeline
	class Pipeline {
		public:
		vk::Pipeline pipeline;
		vk::DescriptorSetLayout descriptorLayout;
		vk::PipelineLayout layout;
		void Create(const char *vertShader, const char *fragShader, const Swapchain &swapchain, const vk::RenderPass renderPass);
		void SetLayout(std::vector<std::pair<vk::DescriptorType, vk::ShaderStageFlags>> descriptors);
		vk::DescriptorSetLayout GetLayout();
		void Destroy();
	};
}