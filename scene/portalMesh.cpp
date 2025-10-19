#include "portalMesh.hpp"
#include "core/descriptorPool.hpp"
#include "sceneTree.hpp"

namespace REngine::Scene {
	void PortalMesh::Create(vk::RenderPass rp) {
		portalCounter++;
		if (!pipeline) {
			pipeline = std::make_shared<Core::Pipeline>();
			pipeline->SetLayout({
				{vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment}
			});
			pipeline->SetInput({Vertex::GetBindingDescription()}, Vertex::GetAttributeDescriptions());
			pipeline->SetSampleCount(Core::Instance::GetInfo().maxMsaa);
			pipeline->SetCullMode(vk::CullModeFlagBits::eNone);
			pipeline->Create("portalVert", "portalFrag", rp);
		}
		pPipeline = pipeline;
		
		renderPass.AddColorAttachment().samples = Core::Instance::GetInfo().maxMsaa;
		renderPass.AddColorImage();
		renderPass.AddDepthAttachment().samples = Core::Instance::GetInfo().maxMsaa;
		renderPass.AddDepthImage();
		renderPass.AddResolveAttachment().finalLayout = vk::ImageLayout::eColorAttachmentOptimal;
		renderPass.AddResolveImage();

		renderPass.CreateRenderPass();

		std::vector<Vertex> vertices {
			Vertex{glm::vec3(-1, -1, 1), glm::vec3(0, 0, 0), glm::vec2(0, 0)}, 
			Vertex{glm::vec3(1, -1, 1), glm::vec3(0, 0, 0), glm::vec2(0, 0)},
			Vertex{glm::vec3(1, 1, 1), glm::vec3(0, 0, 0), glm::vec2(0, 0)},
			Vertex{glm::vec3(-1, 1, 1), glm::vec3(0, 0, 0), glm::vec2(0, 0)},
			Vertex{glm::vec3(-1, -1, -1), glm::vec3(0, 0, 0), glm::vec2(0, 0)}, 
			Vertex{glm::vec3(1, -1, -1), glm::vec3(0, 0, 0), glm::vec2(0, 0)},
			Vertex{glm::vec3(1, 1, -1), glm::vec3(0, 0, 0), glm::vec2(0, 0)},
			Vertex{glm::vec3(-1, 1, -1), glm::vec3(0, 0, 0), glm::vec2(0, 0)}
		};
		std::vector<uint32_t> indices = {0, 1, 2, 0, 2, 3,
										 0, 1, 5, 0, 5, 4,
										 0, 4, 7, 0, 7, 3,
										 1, 5, 6, 1, 6, 2,
										 3, 2, 6, 3, 6, 7,
										 4, 5, 6, 4, 6, 7};
		
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

	void PortalMesh::PreDraw(Core::CommandBuffer cb) {
		auto info = Core::Instance::GetInfo();
		Core::DescriptorPool::SetImage(descriptorSets[info.currentFrame], 0, renderPass.GetView(2).lock()->Views()[info.currentFb], sampler);

		cb.BeginPass(renderPass.GetRenderPass(), info.swapchainExtent, renderPass.GetFramebuffer()[info.currentFb]);
		SceneTree::Current()->CallDrawlist([&cb, this](Drawable &j) {
			if (dynamic_cast<PortalMesh*>(&j)) {
				return;
			}
			j.DrawFromView(cb.GetBuffer(), *renderCam);
		});
		cb.EndPass();

		barrier.image = renderPass.GetImage(2, info.currentFb);
		cb.GetBuffer().pipelineBarrier(vk::PipelineStageFlagBits::eColorAttachmentOutput,
			vk::PipelineStageFlagBits::eFragmentShader, vk::DependencyFlags(), nullptr, nullptr, barrier);
	}


	void PortalMesh::SetRenderCam(Core::Camera *camera) {
		renderCam = camera;
	}
	
	void PortalMesh::Recreate() {
		renderPass.Recreate();
	}

	void PortalMesh::Destroy() {
		Mesh::Destroy();
		renderPass.Destroy();
		if (--portalCounter == 0) {
			pipeline->Destroy();
			pipeline = nullptr;
		}
	}
	
	void PortalMesh::SetSampler(vk::Sampler sampler) {
		this->sampler = sampler;
	}
}