#include "portal.hpp"
#include "core/instance.hpp"
#include "core/vertex.hpp"
#include "scene/sceneTree.hpp"
#include "core/descriptorPool.hpp"
#include "vulkan/vulkan_structs.hpp"
#include <memory>
#include <vulkan/vulkan_enums.hpp>

namespace REngine::Scene {
	void Portal::Create(vk::RenderPass rp) {
		portalCounter++;
		if (!pipeline) {
			pipeline = std::make_shared<Core::Pipeline>();
			pipeline->SetLayout({
				{vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex},
				{vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment}
			});
			pipeline->SetInput({Vertex::GetBindingDescription()}, Vertex::GetAttributeDescriptions());
			pipeline->SetSampleCount(Core::Instance::GetInfo().maxMsaa);
			pipeline->Create("portalVert", "portalFrag", rp);

			renderPass.AddColorAttachment().samples = Core::Instance::GetInfo().maxMsaa;
			renderPass.AddColorImage();
			renderPass.AddDepthAttachment().samples = Core::Instance::GetInfo().maxMsaa;
			renderPass.AddDepthImage();
			renderPass.AddResolveAttachment().finalLayout = vk::ImageLayout::eColorAttachmentOptimal;
			renderPass.AddResolveImage();

			renderPass.CreateRenderPass();
		}
		pPipeline = pipeline;

		std::vector<Vertex> vertices {
			Vertex{glm::vec3(-1, -1, 0), glm::vec3(0, 0, 0), glm::vec2(0, 1)}, 
			Vertex{glm::vec3(1, -1, 0), glm::vec3(0, 0, 0), glm::vec2(1, 1)},
			Vertex{glm::vec3(1, 1, 0), glm::vec3(0, 0, 0), glm::vec2(1, 0)},
			Vertex{glm::vec3(-1, 1, 0), glm::vec3(0, 0, 0), glm::vec2(0, 0)}};
		std::vector<uint32_t> indices = {0, 1, 2, 0, 2, 3};
		
		barrier.oldLayout = vk::ImageLayout::eColorAttachmentOptimal;
		barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
		barrier.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
		barrier.dstQueueFamilyIndex = vk::QueueFamilyIgnored;
		barrier.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
		barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
		barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.subresourceRange.levelCount = 1;

		Mesh::Create(rp, vertices, indices);
	}

	void Portal::PreDraw(Core::CommandBuffer cb) {
		auto info = Core::Instance::GetInfo();
		Core::DescriptorPool::SetImage(descriptorSets[info.currentFrame], 1, renderPass.GetView(2).lock()->Views()[info.currentFb], sampler);

		cb.BeginPass(renderPass.GetRenderPass(), info.swapchainExtent, renderPass.GetFramebuffer()[info.currentFb]);
		SceneTree::Current()->CallDrawlist([&cb](Drawable &j) {
			if (dynamic_cast<Portal*>(&j)) {
				return;
			}
			j.DrawFromView(cb.GetBuffer(), *SceneTree::Current()->ActiveCamera());
		});
		cb.EndPass();

		barrier.image = renderPass.GetImage(2, info.currentFb);
		cb.GetBuffer().pipelineBarrier(vk::PipelineStageFlagBits::eColorAttachmentOutput,
			vk::PipelineStageFlagBits::eFragmentShader, vk::DependencyFlags(), nullptr, nullptr, barrier);
	}

	void Portal::SetSampler(vk::Sampler sampler) {
		this->sampler = sampler;
	}

	void Portal::Destroy() {
		Mesh::Destroy();
		renderPass.Destroy();
		if (--portalCounter == 0) {
			pipeline->Destroy();
			pipeline = nullptr;
		}
	}

	void Portal::Recreate() {
		renderPass.Recreate();
	}
}