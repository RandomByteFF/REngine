#include "viewportRenderer.hpp"

namespace REngine::Core {
	void ViewportRenderer::CreateImages(vk::Extent2D extent, vk::Format colorFormat, vk::Format depthFormat, const std::vector<vk::ImageView> &views) {
		this->views = views;
		colorImage.CreateImage(extent.width, extent.height, 1, Instance::GetInfo().maxMsaa, colorFormat, 
		vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment);
		depthImage.CreateImage(extent.width, extent.height, 1, Instance::GetInfo().maxMsaa, depthFormat, 
		vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::ImageAspectFlagBits::eDepth);
	}

	void ViewportRenderer::CreateRenderPass() {
		vk::AttachmentDescription colorAttachment{};
		colorAttachment.format = colorImage.Format();
		colorAttachment.samples = Instance::GetInfo().maxMsaa;
		colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
		colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
		colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
		colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
		colorAttachment.finalLayout = vk::ImageLayout::eColorAttachmentOptimal;
		
		vk::AttachmentDescription depthAttachment{};
		depthAttachment.format = depthImage.Format();
		depthAttachment.samples = Instance::GetInfo().maxMsaa;
		depthAttachment.loadOp = vk::AttachmentLoadOp::eClear;
		depthAttachment.storeOp = vk::AttachmentStoreOp::eDontCare;
		depthAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
		depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		depthAttachment.initialLayout = vk::ImageLayout::eUndefined;
		depthAttachment.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
		
		vk::AttachmentDescription colorAttachmentResolve{};
		colorAttachmentResolve.format = colorImage.Format(); // If i really mess up, swapchain image might differ from this
		colorAttachmentResolve.samples = vk::SampleCountFlagBits::e1;
		colorAttachmentResolve.loadOp = vk::AttachmentLoadOp::eDontCare;
		colorAttachmentResolve.storeOp = vk::AttachmentStoreOp::eStore;
		colorAttachmentResolve.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
		colorAttachmentResolve.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		colorAttachmentResolve.initialLayout = vk::ImageLayout::eUndefined;
		colorAttachmentResolve.finalLayout = vk::ImageLayout::ePresentSrcKHR;
		
		vk::AttachmentReference colorAttachmentResolveRef{};
		colorAttachmentResolveRef.attachment = 2;
		colorAttachmentResolveRef.layout = vk::ImageLayout::eColorAttachmentOptimal;
		
		vk::AttachmentReference depthAttachmentRef{};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
		
		vk::AttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;
		
		vk::SubpassDescription subpass{};
		subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;
		subpass.pResolveAttachments = &colorAttachmentResolveRef;
		
		std::array<vk::AttachmentDescription, 3> attachments = {colorAttachment, depthAttachment, colorAttachmentResolve};
		
		vk::RenderPassCreateInfo renderPassInfo{};
		renderPassInfo.attachmentCount = uint32_t(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		
		vk::SubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
		dependency.srcAccessMask = vk::AccessFlagBits::eNone;
		dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
		dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;
		
		viewportRenderPass = Instance::GetInfo().device.createRenderPass(renderPassInfo);
	}

	void ViewportRenderer::Render(CommandBuffer cb, vk::Extent2D extent, uint32_t imageIndex, std::vector<std::shared_ptr<Drawable>> &objects, Camera &camera) {
		cb.BeginPass(viewportRenderPass, extent, framebuffers[imageIndex]);

		for (auto i : objects) {
			i->Update(camera);
			i->Bind(cb.GetBuffer());
			i->Draw(cb.GetBuffer());
		}
		
		cb.EndPass();
	}

	vk::RenderPass ViewportRenderer::RenderPass() const {
		return viewportRenderPass;
	}
	
	void ViewportRenderer::CreateFramebuffers() {
		framebuffers.resize(views.size());
		for (size_t i = 0; i < views.size(); i++) {
			std::array<vk::ImageView, 3> attachments = {
				colorImage.View(),
				depthImage.View(),
				views[i]
			};
			
			vk::FramebufferCreateInfo frameBufferInfo{};
			frameBufferInfo.renderPass = viewportRenderPass;
			frameBufferInfo.attachmentCount = uint32_t(attachments.size());
			frameBufferInfo.pAttachments = attachments.data();
			frameBufferInfo.width = colorImage.Width();
			frameBufferInfo.height = colorImage.Height();
			frameBufferInfo.layers = 1;
			
			framebuffers[i] = Instance::GetInfo().device.createFramebuffer(frameBufferInfo);
		}
	}

	void ViewportRenderer::DestroyBuffers() {
		colorImage.Destroy();
		depthImage.Destroy();
		for(auto i : framebuffers) {
			Instance::GetInfo().device.destroyFramebuffer(i);
		}
	}
	
	void ViewportRenderer::Destroy(){
		Instance::GetInfo().device.destroyRenderPass(viewportRenderPass);
	}
}