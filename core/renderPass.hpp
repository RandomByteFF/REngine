#pragma once

#include "commandBuffer.hpp"
#include "vulkan/vulkan_enums.hpp"
#include "vulkan/vulkan_structs.hpp"
#include "IViews.hpp"
#include <cstdint>
#include <optional>
#include <vector>
#include "renderTarget.hpp"
#include <memory>

namespace REngine::Core {
	struct ColorLayoutInfo {
		vk::ImageLayout layout = vk::ImageLayout::eUndefined;
		vk::ImageLayout finalLayout = vk::ImageLayout::eColorAttachmentOptimal;
		vk::ImageLayout resolveLayout = vk::ImageLayout::eUndefined;
	};

	struct ColorAttachmentInfo {
		std::optional<vk::Format> format;
		vk::AttachmentLoadOp  loadOp     = vk::AttachmentLoadOp::eClear;
		vk::AttachmentStoreOp storeOp    = vk::AttachmentStoreOp::eStore;
		vk::ClearValue        clearValue = vk::ClearColorValue{0.0f, 0.0f, 0.0f, 1.0f};
		std::optional<std::weak_ptr<IViews>> image = {};

		bool sampled = false;
		vk::ResolveModeFlagBits resolveMode = vk::ResolveModeFlagBits::eAverage;
		std::optional<std::weak_ptr<IViews>> resolveImage = {};

		ColorLayoutInfo layoutInfo;
	};

	struct DepthAttachmentInfo {
		std::optional<vk::Format> format;
		vk::AttachmentLoadOp  loadOp     = vk::AttachmentLoadOp::eClear;
		vk::AttachmentStoreOp storeOp    = vk::AttachmentStoreOp::eDontCare;
		vk::ClearValue        clearValue = vk::ClearDepthStencilValue{1.0f, 0};
		
		std::optional<std::weak_ptr<IViews>> image = {};
	};

	class RenderPass {
		std::vector<vk::RenderingAttachmentInfo> colorAttachments;
		std::vector<vk::Format> colorFormats;
		std::optional<vk::RenderingAttachmentInfo> depthAttachment;
		std::optional<vk::Format> depthFormat;
		
		std::weak_ptr<IViews> AddImage(vk::SampleCountFlagBits samples, vk::Format format, vk::ImageUsageFlags usage, vk::ImageAspectFlagBits aspect = vk::ImageAspectFlagBits::eColor);
		
		std::vector<std::weak_ptr<IViews>> colorImages;
		std::vector<std::weak_ptr<IViews>> resolveImages;
		std::vector<ColorLayoutInfo> layoutInfos;
		std::weak_ptr<IViews> depthImage;
		std::vector<std::shared_ptr<RenderTarget>> internalImages;
		
		vk::SampleCountFlagBits sampleCount = vk::SampleCountFlagBits::e1;
		
		std::optional<CommandBuffer> activeCommandBuffer;

		void TransitionIn();
		void TransitionOut();
		uint32_t currentImageIndex = 0;

	public:
		RenderPass(vk::SampleCountFlagBits sampleCount = vk::SampleCountFlagBits::e1);
		void AddColorAttachment(ColorAttachmentInfo info);
		void SetDepthAttachment(DepthAttachmentInfo info);

		void Destroy();
		void Begin(CommandBuffer cmd, vk::Extent2D extent, uint32_t imageIndex);
		void End();

		vk::PipelineRenderingCreateInfo GetPipelineRenderingInfo() const;
		std::weak_ptr<IViews> GetColorView(uint32_t index);
		std::weak_ptr<IViews> GetDepthView();
		vk::Image GetImage(uint32_t slot, uint32_t index);
	};
}