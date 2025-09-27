#pragma once

namespace REngine::Core {
	class IViews {
	public:
		virtual const std::vector<vk::ImageView> &Views() const = 0;
	};
}