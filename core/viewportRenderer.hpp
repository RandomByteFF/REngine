#pragma once
#include "headers.h"
#include "drawable/drawable.hpp"
#include "core/image.hpp"
#include "commandBuffer.hpp"
#include <memory>

namespace REngine::Core {
	class ViewportRenderer {
		vk::RenderPass viewportRenderPass;
		Image depthImage;
		Image colorImage;
		std::vector<vk::ImageView> views;
		std::vector<vk::Framebuffer> framebuffers;
	public:
		void CreateImages(vk::Extent2D extent, vk::Format colorFormat, vk::Format depthFormat, const std::vector <vk::ImageView> &views);
		void CreateRenderPass();
		void CreateFramebuffers();
		void Render(CommandBuffer cb, vk::Extent2D extent, uint32_t imageIndex, std::vector<std::shared_ptr<Drawable>> &objects, Camera &camera);

		vk::RenderPass RenderPass() const;
		void DestroyBuffers();
		void Destroy();
	};
}