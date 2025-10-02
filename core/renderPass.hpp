#pragma once

#include "headers.h"
#include "vulkan/vulkan_enums.hpp"
#include "vulkan/vulkan_structs.hpp"
#include "IViews.hpp"
#include <cstdint>
#include <optional>
#include <vector>
#include "renderTarget.hpp"

namespace REngine::Core {
	class RenderPass {
		vk::RenderPass renderPass;
		std::vector<vk::AttachmentDescription> attachments;
		std::vector<vk::AttachmentReference> colorReferences;
		std::vector<vk::AttachmentReference> resolveReferences;
		vk::AttachmentReference depthStencilReference;
		bool resolveEnable = false;
		bool depthEnable = false;
		vk::Extent2D extent;

		std::vector<std::shared_ptr<RenderTarget>> internalImages;
		std::vector<std::weak_ptr<IViews>> images;
		std::vector<vk::Framebuffer> framebuffers;

		void CreateFramebuffers();
		void AddImage(std::optional<std::shared_ptr<IViews>> image, vk::ImageUsageFlags usage, vk::ImageAspectFlagBits aspect = vk::ImageAspectFlagBits::eColor);

	public:
		vk::AttachmentDescription &AddColorAttachment();
		void AddColorImage(std::optional<std::shared_ptr<IViews>> image = {});
		vk::AttachmentDescription &AddDepthAttachment();
		void AddDepthImage(std::optional<std::shared_ptr<IViews>> image = {});
		vk::AttachmentDescription &AddResolveAttachment();
		void AddResolveImage(std::optional<std::shared_ptr<IViews>> image = {});

		void CreateRenderPass();
		void Recreate();
		void Destroy();
		vk::RenderPass GetRenderPass() const;
		std::vector<vk::Framebuffer> &GetFramebuffer();
		std::weak_ptr<IViews> GetView(uint32_t index);
		vk::Image GetImage(uint32_t slot, uint32_t index);
	};
}