#include "renderPass.hpp"
#include "image.hpp"
#include "instance.hpp"
#include "renderTarget.hpp"
#include "vulkan/vulkan_enums.hpp"
#include "vulkan/vulkan_structs.hpp"
#include <memory>
#include <stdexcept>

namespace REngine::Core {
	vk::AttachmentDescription &RenderPass::AddColorAttachment() {
		attachments.push_back(vk::AttachmentDescription());
		vk::AttachmentDescription &colorAttachment = attachments.back();
		colorAttachment.format = Instance::GetInfo().imageFormat.format;
		colorAttachment.samples = vk::SampleCountFlagBits::e1;
		colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
		colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
		colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
		colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
		colorAttachment.finalLayout = vk::ImageLayout::eColorAttachmentOptimal;
		colorReferences.push_back(vk::AttachmentReference{uint32_t(attachments.size() - 1), vk::ImageLayout::eColorAttachmentOptimal });

		return colorAttachment;
	}

	void RenderPass::AddColorImage(std::optional<std::shared_ptr<IViews>> image) {
		AddImage(image, vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment);
	}

	vk::AttachmentDescription &RenderPass::AddDepthAttachment() {
		depthEnable = true;
		attachments.push_back(vk::AttachmentDescription());
		vk::AttachmentDescription &depthStencilAttachment = attachments.back();
		depthStencilAttachment.format = Instance::GetInfo().depthFormat;
		depthStencilAttachment.samples = vk::SampleCountFlagBits::e1;
		depthStencilAttachment.loadOp = vk::AttachmentLoadOp::eClear;
		depthStencilAttachment.storeOp = vk::AttachmentStoreOp::eDontCare;
		depthStencilAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
		depthStencilAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		depthStencilAttachment.initialLayout = vk::ImageLayout::eUndefined;
		depthStencilAttachment.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
		depthStencilReference = vk::AttachmentReference{ uint32_t(attachments.size() - 1), vk::ImageLayout::eDepthStencilAttachmentOptimal };

		return depthStencilAttachment;
	}

	void RenderPass::AddDepthImage(std::optional<std::shared_ptr<IViews>> image) {
		AddImage(image, vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::ImageAspectFlagBits::eDepth);
	}

	vk::AttachmentDescription &RenderPass::AddResolveAttachment() {
		resolveEnable = true;
		attachments.push_back(vk::AttachmentDescription());
		vk::AttachmentDescription &colorAttachmentResolve = attachments.back();
		colorAttachmentResolve.format = Instance::GetInfo().imageFormat.format;
		colorAttachmentResolve.samples = vk::SampleCountFlagBits::e1;
		colorAttachmentResolve.loadOp = vk::AttachmentLoadOp::eDontCare;
		colorAttachmentResolve.storeOp = vk::AttachmentStoreOp::eStore;
		colorAttachmentResolve.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
		colorAttachmentResolve.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		colorAttachmentResolve.initialLayout = vk::ImageLayout::eUndefined;
		colorAttachmentResolve.finalLayout = vk::ImageLayout::ePresentSrcKHR;
		resolveReferences.push_back(vk::AttachmentReference{uint32_t(attachments.size() - 1), vk::ImageLayout::eColorAttachmentOptimal });
		return colorAttachmentResolve;
	}

	void RenderPass::AddResolveImage(std::optional<std::shared_ptr<IViews>> image) {
		AddImage(image, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled);
		// FIXME: transition and custom usage bits
	}

	void RenderPass::AddImage(std::optional<std::shared_ptr<IViews>> image, vk::ImageUsageFlags usage, vk::ImageAspectFlagBits aspect) {
		if (image.has_value()) {
			images.push_back(image.value());
		}
		else {
			internalImages.push_back(std::make_shared<RenderTarget>());
			internalImages.back()->CreateTarget(attachments.back().samples, attachments.back().format, usage, aspect);
			images.push_back(internalImages.back());
		}
	}

	void RenderPass::CreateRenderPass() {
		vk::SubpassDescription subpass{};
		subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = colorReferences.data();
		if (depthEnable) subpass.pDepthStencilAttachment = &depthStencilReference;
		if (resolveEnable) subpass.pResolveAttachments = resolveReferences.data();
		
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
		
		renderPass = Instance::GetInfo().device.createRenderPass(renderPassInfo);

		CreateFramebuffers();
	}

	void RenderPass::Recreate() {
		Destroy();
		CreateFramebuffers();
	}

	void RenderPass::Destroy() {
		for (auto i : internalImages) i->Destroy();
		internalImages.clear();
		images.clear();
		for(auto i : framebuffers) {
			Instance::GetInfo().device.destroyFramebuffer(i);
		}

		Instance::GetInfo().device.destroyRenderPass(renderPass);
	}

	void RenderPass::CreateFramebuffers() {
		int count = Instance::GetInfo().swapchainSize;
		if (attachments.size() != images.size()) {
			throw std::runtime_error("Attachments and image sizes differ!");
		}
		framebuffers.resize(count);
		for (size_t i = 0; i < count; i++) {
			std::vector<vk::ImageView> views(attachments.size());
			for (size_t j = 0; j < attachments.size(); j++) {
				views[j] = images[j].lock()->Views()[i]; 
			}
			
			vk::FramebufferCreateInfo frameBufferInfo{};
			frameBufferInfo.renderPass = renderPass;
			frameBufferInfo.attachmentCount = uint32_t(attachments.size());
			frameBufferInfo.pAttachments = views.data();
			frameBufferInfo.width = Instance::GetInfo().swapchainExtent.width;
			frameBufferInfo.height = Instance::GetInfo().swapchainExtent.height;
			frameBufferInfo.layers = 1;
			
			framebuffers[i] = Instance::GetInfo().device.createFramebuffer(frameBufferInfo);
		}
	}

	vk::RenderPass RenderPass::GetRenderPass() const {
		return renderPass;
	}

	std::vector<vk::Framebuffer> &RenderPass::GetFramebuffer() {
		return framebuffers;
	}

	std::weak_ptr<IViews> RenderPass::GetView(uint32_t index) {
		return images[index];
	}

	vk::Image RenderPass::GetImage(uint32_t slot, uint32_t index) {
		auto i = dynamic_cast<RenderTarget&>(*(images[slot].lock()));
		return i.GetImage(index);
	}
}