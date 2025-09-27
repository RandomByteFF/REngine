#include "renderTarget.hpp"
#include "instance.hpp"
#include "vulkan/vulkan_enums.hpp"

namespace REngine::Core {
	RenderTarget::RenderTarget() {
		targets.push_back(this);
	}

	void RenderTarget::CreateTarget(vk::SampleCountFlagBits numSample, vk::Format format ,vk::ImageUsageFlags usage, vk::ImageAspectFlagBits aspect) {
		this->numSample = numSample;
		this->usage = usage;
		this->aspect = aspect;
		this->format = format;
		auto info = Instance::GetInfo();
		images.resize(info.swapchainSize);
		for(int i = 0; i < images.size(); i++) {
			images[i].CreateImage(info.swapchainExtent.width, info.swapchainExtent.height, 1, numSample, format, vk::ImageTiling::eOptimal, usage, aspect);
		}

		views.resize(images.size());
		for(int i = 0; i < images.size(); i++) {
			views[i] = images[i].View();
		}
	}

	const std::vector<vk::ImageView> &RenderTarget::Views() const {
		return views;
	}

	void RenderTarget::Recreate() {
		for(auto i : images) i.Destroy();
		CreateTarget(numSample, format, usage, aspect);
	}

	void RenderTarget::RecreateAll() {
		for (auto i : targets) i->Recreate();
	}

	void RenderTarget::Destroy(){
		for (auto i : images) i.Destroy();
	}

	const vk::Image RenderTarget::GetImage(uint32_t index) const {
		return images[index].Get();
	}
}	