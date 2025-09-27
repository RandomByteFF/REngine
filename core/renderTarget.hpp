#pragma once
#include "IViews.hpp"
#include "image.hpp"
#include <vector>

namespace REngine::Core {
	class RenderTarget : public IViews {
		std::vector<Image> images;
		std::vector<vk::ImageView> views;

		vk::SampleCountFlagBits numSample;
		vk::ImageUsageFlags usage;
		vk::ImageAspectFlagBits aspect;
		vk::Format format;
		
		inline static std::vector<RenderTarget *> targets;

	public:
		RenderTarget();
		void CreateTarget(vk::SampleCountFlagBits numSample, vk::Format format, vk::ImageUsageFlags usage, vk::ImageAspectFlagBits aspect = vk::ImageAspectFlagBits::eColor);
		virtual const std::vector<vk::ImageView> &Views() const;
		void Recreate();
		static void RecreateAll();
		void Destroy();
		const vk::Image GetImage(uint32_t index) const;
	};
}