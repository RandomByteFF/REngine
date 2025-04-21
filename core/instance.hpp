#pragma once
#include "headers.h"
#include "Info.hpp"
#include "swapchain.hpp"
#include "windowManager.hpp"
#include "vk_mem_alloc.h"
#include <functional>

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

const std::vector<const char *> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

using ResizedCallback = std::function<void(int, int)>;
using ResizedCallbackVector = std::vector<std::pair<void*, ResizedCallback>>;

namespace REngine::Core {
	class Instance {
		Instance() = delete;
		Instance(const Instance &) = delete;
		Instance &operator=(const Instance &) = delete;

		inline static vk::Instance instance;
		inline static vk::PhysicalDevice physicalDevice;
		inline static vk::Device device;
		inline static vk::CommandPool commandPool;
		inline static vk::Queue graphicsQueue;
		inline static vk::Queue presentQueue;
		inline static VmaAllocator allocator;
		inline static Info info;
		inline static ResizedCallbackVector callbacks;
		
		static bool CheckValidationLayerSupport();
		static void InitializeInstance();
		static void PickPhysicalDevice();
		static bool IsDeviceSuitable(vk::PhysicalDevice device);
		static bool CheckDeviceExtensionSupport(vk::PhysicalDevice device, const std::vector<const char*> &requiredExtensions);
		
	public:
		static void Initialize(WindowManager manager);
		static const vk::Instance &Get();
		static const Info &GetInfo();
		static vk::SampleCountFlagBits GetMaxUsableSampleCount();
		static void FrameBufferResized(int width, int height);
		static void CreateLogicalDevice();
		static void SetCurrentFrame(uint32_t frame);
		static void OnResize(void *caller, ResizedCallback cb);
		static void UnsubscribeResize(void *caller);

		static void Destroy();
	};
}

