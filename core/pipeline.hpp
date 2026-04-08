#pragma once

#include "swapchain.hpp"
#include "vulkan/vulkan_enums.hpp"
#include "vulkan/vulkan_structs.hpp"

namespace REngine::Core {
	enum Descriptor {
		UniformBuffer,
		Sampler
	};

	// TODO: inherit layout from other pipeline
	class Pipeline {
		vk::Pipeline pipeline;
		vk::DescriptorSetLayout descriptorLayout;
		vk::PipelineLayout layout;

		std::vector<vk::VertexInputBindingDescription> vbinddesc;
		std::vector<vk::VertexInputAttributeDescription> vattribdesc;
		std::string vertEntry = "main";
		std::string fragEntry = "main";
		vk::PrimitiveTopology topology = vk::PrimitiveTopology::eTriangleList;
		vk::CullModeFlagBits cullMode = vk::CullModeFlagBits::eBack;
		bool depthTest = true;
		bool stencilTest = false;
		vk::SampleCountFlagBits sampleCount = vk::SampleCountFlagBits::e1;

	public:
		void Create(const char *vertShader, const char *fragShader, const vk::RenderPass renderPass);
		
		void SetLayout(std::vector<std::pair<vk::DescriptorType, vk::ShaderStageFlags>> descriptors);
		void SetInput(std::vector<vk::VertexInputBindingDescription>, std::vector<vk::VertexInputAttributeDescription>);
		void SetVertexEntry(std::string &entry);
		void SetFragEntry(std::string &entry);
		void SetPrimitiveTopology(vk::PrimitiveTopology topology);
		void SetCullMode(vk::CullModeFlagBits cull) { cullMode = cull; }
		void setDepthTest(bool test) { depthTest = test; }
		void setStencilTest(bool test) { stencilTest = test; }
		void SetSampleCount(vk::SampleCountFlagBits sample) { sampleCount = sample; }

		const vk::Pipeline &GetPipeline() const;
		const vk::DescriptorSetLayout &GetLayout() const;
		const vk::PipelineLayout &GetPipelineLayout() const;
		void Destroy();
	};
}