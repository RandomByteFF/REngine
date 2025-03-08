#pragma once
#include "headers.h"
#include "Info.hpp"
#include "swapchain.hpp"
#include "windowManager.hpp"

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

namespace REngine::Core {
	class Instance {
		Instance() = delete;
		Instance(const Instance &) = delete;
		Instance &operator=(const Instance &) = delete;

		inline static vk::Instance instance;
		inline static vk::PhysicalDevice physicalDevice;
		inline static vk::Device device;
		inline static vk::Queue graphicsQueue;
		inline static vk::Queue presentQueue;
		inline static Info info;
		
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
	};
}

