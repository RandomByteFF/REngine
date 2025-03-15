#include "pipeline.hpp"

#include "loader/shader.hpp"
#include "instance.hpp"
#include "vertex.hpp"

/* TODO: Descriptor sets should be reusable. My idea for this is:
- Instead of SetLayout, create an AddLayout. This can either take a new layout, or a reference to an existing one.
- On binding, we won't try to bind the already bound descriptors.
*/
namespace REngine::Core {
	void Pipeline::Create(const char *vertShader, const char *fragShader, Swapchain &swapchain, vk::RenderPass renderPass) {
		vk::ShaderModule vertShaderModule = Loader::Shader::Get(vertShader);
		vk::ShaderModule fragShaderModule = Loader::Shader::Get(fragShader);
		
		vk::PipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.stage = vk::ShaderStageFlagBits::eVertex;
		vertShaderStageInfo.module = vertShaderModule;
		vertShaderStageInfo.pName = "main";
		
		vk::PipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.stage = vk::ShaderStageFlagBits::eFragment;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName = "main";
		
		vk::PipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};
		
		std::vector<vk::DynamicState> dynamicStates = {
			vk::DynamicState::eViewport,
			vk::DynamicState::eScissor
		};
		
		vk::PipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.dynamicStateCount = uint32_t(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();
		
		auto bindingDescription = Vertex::GetBindingDescription();
		auto attributeDescription = Vertex::GetAttributeDescriptions();

		vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputInfo.vertexAttributeDescriptionCount = uint32_t(attributeDescription.size());
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescription.data();
		
		vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
		inputAssembly.primitiveRestartEnable = VK_FALSE;
		
		vk::Viewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = float(swapchain.Extent().width);
		viewport.height = float(swapchain.Extent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		
		vk::Rect2D scissor{};
		scissor.offset = vk::Offset2D{0, 0};
		scissor.extent = swapchain.Extent();
		
		vk::PipelineViewportStateCreateInfo	viewportState;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;
		
		vk::PipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = vk::PolygonMode::eFill;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = vk::CullModeFlagBits::eBack;
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
		colorBlendAttachment.blendEnable = VK_FALSE;
		
		vk::PipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		
		vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &descriptorLayout;
	
		layout = Instance::GetInfo().device.createPipelineLayout(pipelineLayoutInfo);

		vk::PipelineDepthStencilStateCreateInfo depthStencil{};
		depthStencil.depthTestEnable = VK_TRUE;
		depthStencil.depthWriteEnable = VK_TRUE;
		depthStencil.depthCompareOp = vk::CompareOp::eLess;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.stencilTestEnable = VK_FALSE;
		
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
	vk::DescriptorSetLayout Pipeline::GetLayout() {
		return descriptorLayout;
	}
	void Pipeline::Destroy()
	{
		Instance::GetInfo().device.destroyDescriptorSetLayout(descriptorLayout);
		Instance::GetInfo().device.destroyPipeline(pipeline);
		Instance::GetInfo().device.destroyPipelineLayout(layout);
	}
}
