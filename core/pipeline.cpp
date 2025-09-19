#include "pipeline.hpp"

#include "loader/shader.hpp"
#include "instance.hpp"

/* TODO: Descriptor sets should be reusable. My idea for this is:
- Instead of SetLayout, create an AddLayout. This can either take a new layout, or a reference to an existing one.
- On binding, we won't try to bind the already bound descriptors.
*/
namespace REngine::Core {
	void Pipeline::Create(const char *vertShader, const char *fragShader, const vk::RenderPass renderPass) {
		vk::ShaderModule vertShaderModule = Loader::Shader::Get(vertShader);
		vk::ShaderModule fragShaderModule = Loader::Shader::Get(fragShader);
		
		vk::PipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.stage = vk::ShaderStageFlagBits::eVertex;
		vertShaderStageInfo.module = vertShaderModule;
		vertShaderStageInfo.pName = vertEntry.data();
		
		vk::PipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.stage = vk::ShaderStageFlagBits::eFragment;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName = fragEntry.data();
		
		vk::PipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};
		
		std::vector<vk::DynamicState> dynamicStates = {
			vk::DynamicState::eViewport,
			vk::DynamicState::eScissor
		};
		
		vk::PipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.dynamicStateCount = uint32_t(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();
		
		vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.vertexBindingDescriptionCount = uint32_t(vbinddesc.size());
		vertexInputInfo.pVertexBindingDescriptions = vbinddesc.data();
		vertexInputInfo.vertexAttributeDescriptionCount = uint32_t(vattribdesc.size());
		vertexInputInfo.pVertexAttributeDescriptions = vattribdesc.data();
		
		vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.topology = topology;
		inputAssembly.primitiveRestartEnable = VK_FALSE;
		
		vk::PipelineViewportStateCreateInfo	viewportState;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;
		
		vk::PipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = vk::PolygonMode::eFill;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = cullMode; 
		rasterizer.frontFace = vk::FrontFace::eCounterClockwise;
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f;
		rasterizer.depthBiasClamp = 0.0f;
		rasterizer.depthBiasSlopeFactor = 0.0f;
		
		vk::PipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = Instance::GetInfo().maxMsaa;
		
		vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
		colorBlendAttachment.blendEnable = vk::True;
		colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
		colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
		colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;
		colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eSrcAlpha;
		colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
		colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;
		// TODO: figure this one out
		
		vk::PipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.logicOpEnable = vk::False;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		
		vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &descriptorLayout;
	
		layout = Instance::GetInfo().device.createPipelineLayout(pipelineLayoutInfo);

		vk::PipelineDepthStencilStateCreateInfo depthStencil{};
		depthStencil.depthTestEnable = depthTest;
		depthStencil.depthWriteEnable = depthTest;
		depthStencil.depthCompareOp = vk::CompareOp::eLess;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.stencilTestEnable = stencilTest;
		
		vk::GraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = & rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = &depthStencil;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = layout;
		pipelineInfo.renderPass = renderPass;
		pipelineInfo.subpass = 0;

		auto r = Instance::GetInfo().device.createGraphicsPipeline(nullptr, pipelineInfo);
		if (r.result != vk::Result::eSuccess) {
			throw std::runtime_error("Failed to create graphics pipeline");
		}
		pipeline = r.value;
	}

	void Pipeline::SetLayout(std::vector<std::pair<vk::DescriptorType, vk::ShaderStageFlags>> descriptors) {
		std::vector<vk::DescriptorSetLayoutBinding> bindings(descriptors.size());
		for (uint32_t i = 0; i < descriptors.size(); i++) {;
			bindings[i].binding = i;
			bindings[i].descriptorCount = 1;
			bindings[i].descriptorType = descriptors[i].first;
			bindings[i].stageFlags = descriptors[i].second;
			bindings[i].pImmutableSamplers = nullptr;
		}		

		vk::DescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.bindingCount = uint32_t(bindings.size());
		layoutInfo.pBindings = bindings.data();

		descriptorLayout = Instance::GetInfo().device.createDescriptorSetLayout(layoutInfo);

	}

	void Pipeline::SetInput(std::vector<vk::VertexInputBindingDescription> bind, std::vector<vk::VertexInputAttributeDescription> attrib) {
		vbinddesc = bind;
		vattribdesc = attrib;
	}
		
	void Pipeline::SetVertexEntry(std::string &entry) {
		vertEntry = entry;
	}

	void Pipeline::SetFragEntry(std::string &entry) {
		fragEntry = entry;
	}
	
	void Pipeline::SetPrimitiveTopology(vk::PrimitiveTopology topology) {
		this->topology = topology;
	}

	const vk::Pipeline &Pipeline::GetPipeline() const {
		return pipeline;
	}

	const vk::DescriptorSetLayout &Pipeline::GetLayout() const {
		return descriptorLayout;
	}

	const vk::PipelineLayout &Pipeline::GetPipelineLayout() const {
		return layout;
	}

	void Pipeline::Destroy()
	{
		Instance::GetInfo().device.destroyDescriptorSetLayout(descriptorLayout);
		Instance::GetInfo().device.destroyPipeline(pipeline);
		Instance::GetInfo().device.destroyPipelineLayout(layout);
	}
}
