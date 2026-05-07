#pragma once

namespace REngine::Core {
	class IViews {
	public:
		virtual const std::vector<vk::ImageView> &Views() const = 0;
		virtual const vk::Image GetImage(uint32_t index) const = 0;
	};
}