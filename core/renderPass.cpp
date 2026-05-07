#include "renderPass.hpp"
#include "commandBuffer.hpp"
#include "instance.hpp"
#include "renderTarget.hpp"
#include "vulkan/vulkan_enums.hpp"
#include "vulkan/vulkan_structs.hpp"
#include <cstddef>
#include <memory>
#include <optional>
#include <stdexcept>
#include <iostream>

namespace REngine::Core {
	RenderPass::RenderPass(vk::SampleCountFlagBits sampleCount) {
		this->sampleCount = sampleCount;
	}

	void RenderPass::AddColorAttachment(ColorAttachmentInfo info) {
		if (!info.format) info.format = Instance::GetInfo().imageFormat.format;
		vk::RenderingAttachmentInfo renderInfo{};
		renderInfo.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
		renderInfo.loadOp = info.loadOp;
		renderInfo.storeOp = info.storeOp;
		renderInfo.clearValue = info.clearValue;

		if (!info.image.has_value()) {
			vk::ImageUsageFlags flags = vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment;
			if (info.sampled && sampleCount == vk::SampleCountFlagBits::e1) flags |= vk::ImageUsageFlagBits::eSampled;
			info.image = AddImage(sampleCount, *info.format, flags);
		}
		colorImages.push_back(*info.image);

		if (!info.sampled && info.resolveImage) throw std::runtime_error("Shouldn't resolve but has resolve image.");
		if (info.sampled) {
			if (!info.resolveImage.has_value()) {
				info.resolveImage = AddImage(vk::SampleCountFlagBits::e1, *info.format, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled);
			}
			resolveImages.push_back(*info.resolveImage);
			renderInfo.resolveMode = info.resolveMode;
			renderInfo.resolveImageLayout = vk::ImageLayout::eColorAttachmentOptimal;
		}
		else {
			resolveImages.push_back(std::weak_ptr<IViews>());
		}

		colorAttachments.push_back(std::move(renderInfo));
		colorFormats.push_back(*info.format);
		layoutInfos.push_back(info.layoutInfo);
	}

	void RenderPass::SetDepthAttachment(DepthAttachmentInfo info) {
		if (depthAttachment) std::cout << "DepthAttachment already set. Changing!";
		if (!info.format) info.format = Instance::GetInfo().depthFormat;
		vk::RenderingAttachmentInfo renderInfo;
		renderInfo.imageLayout = vk::ImageLayout::eDepthAttachmentOptimal;
		renderInfo.loadOp = info.loadOp;
		renderInfo.storeOp = info.storeOp;
		renderInfo.clearValue = info.clearValue;

		depthAttachment = renderInfo;
		depthFormat = info.format;

		if (!info.image.has_value()) {
			info.image = AddImage(sampleCount, *info.format, vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::ImageAspectFlagBits::eDepth);
		}
		depthImage = *info.image;

	}

	std::weak_ptr<IViews> RenderPass::AddImage(vk::SampleCountFlagBits samples, vk::Format format, vk::ImageUsageFlags usage, vk::ImageAspectFlagBits aspect) {
		internalImages.push_back(std::make_shared<RenderTarget>());
		internalImages.back()->CreateTarget(samples, format, usage, aspect);
		return internalImages.back();
	}

	void RenderPass::Begin(CommandBuffer cmd, vk::Extent2D extent, uint32_t imageIndex) {
		currentImageIndex = imageIndex;
		activeCommandBuffer = cmd;

		TransitionIn();

		vk::RenderingInfo renderingInfo{};
		renderingInfo.renderArea = vk::Rect2D{{0, 0}, extent};
		renderingInfo.layerCount = 1;

		for (size_t i = 0; i < colorAttachments.size(); i++) {
			colorAttachments[i].imageView = colorImages[i].lock()->Views()[imageIndex];
			if (auto resolve = resolveImages[i].lock()) colorAttachments[i].resolveImageView = resolve->Views()[imageIndex];
			else colorAttachments[i].resolveImageView = nullptr;
		}
		renderingInfo.colorAttachmentCount = uint32_t(colorAttachments.size());
		renderingInfo.pColorAttachments = colorAttachments.data();

		if (depthAttachment) {
			depthAttachment->imageView = depthImage.lock()->Views()[imageIndex];
			renderingInfo.pDepthAttachment = &*depthAttachment;
		}
		else {
			renderingInfo.pDepthAttachment = nullptr;
		}

		activeCommandBuffer->GetBuffer().beginRendering(renderingInfo);
		vk::Viewport viewport{};
		viewport.x        = 0.0f;
		viewport.y        = 0.0f;
		viewport.width    = float(extent.width);
		viewport.height   = float(extent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		activeCommandBuffer->GetBuffer().setViewport(0, viewport);

		vk::Rect2D scissor{};
		scissor.offset = vk::Offset2D{0, 0};
		scissor.extent = extent;
		activeCommandBuffer->GetBuffer().setScissor(0, scissor);
	}

	void RenderPass::End() {
		if (!activeCommandBuffer) throw std::runtime_error("The render pass wasn't active when it stopped.");
		activeCommandBuffer->GetBuffer().endRendering();
		TransitionOut();
		activeCommandBuffer = std::nullopt;
	}

	void RenderPass::Destroy() {
		for (auto i : internalImages) i->Destroy();
		internalImages.clear();
		colorImages.clear();
		resolveImages.clear();
		depthImage = {};
	}

	vk::PipelineRenderingCreateInfo RenderPass::GetPipelineRenderingInfo() const {
		vk::PipelineRenderingCreateInfo info{};
		info.colorAttachmentCount = uint32_t(colorFormats.size());
		info.pColorAttachmentFormats = colorFormats.data();
		if (depthFormat) {
			info.depthAttachmentFormat = *depthFormat;
		}
		return info;
	}

	std::weak_ptr<IViews> RenderPass::GetColorView(uint32_t index) {
		if (!resolveImages[index].expired()) return resolveImages[index];
		else return colorImages[index];
	}
	
	std::weak_ptr<IViews> RenderPass::GetDepthView() {
		return depthImage;
	}

	vk::Image RenderPass::GetImage(uint32_t slot, uint32_t index) {
		RenderTarget ret;
		ret = dynamic_cast<RenderTarget&>(*((resolveImages[index].expired() ? colorImages : resolveImages)[slot].lock()));
		return ret.GetImage(index);
	}

	void RenderPass::TransitionIn() {
		std::vector<vk::ImageMemoryBarrier2> barriers;
		vk::ImageMemoryBarrier2 example;
		example
		.setSrcStageMask(vk::PipelineStageFlagBits2::eTopOfPipe)
		.setSrcAccessMask(vk::AccessFlagBits2::eNone)
		.setOldLayout(vk::ImageLayout::eUndefined)
		.setDstStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput)
		.setDstAccessMask(vk::AccessFlagBits2::eColorAttachmentWrite)
		.setNewLayout(vk::ImageLayout::eColorAttachmentOptimal)
		.setSubresourceRange({vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
		for(auto i : colorImages) {
			barriers.push_back(example);
			barriers.back().setImage(i.lock()->GetImage(currentImageIndex));
		}
		for(auto i : resolveImages) {
			if (i.expired()) continue;
			barriers.push_back(example);
			barriers.back().setImage(i.lock()->GetImage(currentImageIndex));
		}
		if (!depthImage.expired()) {
			example
			.setDstStageMask(vk::PipelineStageFlagBits2::eEarlyFragmentTests)
			.setDstAccessMask(vk::AccessFlagBits2::eDepthStencilAttachmentWrite)
			.setNewLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
			.setSubresourceRange({vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1});
			barriers.push_back(example);
			barriers.back().setImage(depthImage.lock()->GetImage(currentImageIndex));
		}

		vk::DependencyInfo dependencyInfo;
		dependencyInfo.setImageMemoryBarrierCount(uint32_t(barriers.size()))
		.setPImageMemoryBarriers(barriers.data());

		activeCommandBuffer->GetBuffer().pipelineBarrier2(dependencyInfo);
	}
	
	void RenderPass::TransitionOut() {
		std::vector<vk::ImageMemoryBarrier2> barriers;
		vk::ImageMemoryBarrier2 example;
		example
		.setSrcStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput)
		.setSrcAccessMask(vk::AccessFlagBits2::eColorAttachmentWrite)
		.setOldLayout(vk::ImageLayout::eColorAttachmentOptimal)
		.setDstStageMask(vk::PipelineStageFlagBits2::eFragmentShader)
		.setDstAccessMask(vk::AccessFlagBits2::eShaderRead)
		.setSubresourceRange({vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});

		for (int i = 0; i < layoutInfos.size(); i++) {
			if (layoutInfos[i].finalLayout != vk::ImageLayout::eColorAttachmentOptimal) {
				barriers.push_back(example);
				barriers.back().setImage((resolveImages[i].expired() ? colorImages[i] : resolveImages[i]).lock()->GetImage(currentImageIndex));
				barriers.back().setNewLayout(layoutInfos[i].finalLayout);
			}
		}

		vk::DependencyInfo dependencyInfo;
		dependencyInfo.setImageMemoryBarrierCount(uint32_t(barriers.size()))
		.setPImageMemoryBarriers(barriers.data());

		activeCommandBuffer->GetBuffer().pipelineBarrier2(dependencyInfo);
	}
}
