#include "instance.hpp"

#include <GLFW/glfw3.h>
#include "queueFamily.hpp"

// namespace {
// }

namespace REngine::Core {
	void Instance::Initialize(WindowManager manager) {
		InitializeInstance();
		manager.CreateSurface(Instance::Get(), info.surface);
		PickPhysicalDevice();
	}

	const vk::Instance &Instance::Get() {
		return instance;
	}

	const Info & Instance::GetInfo() {
		return info;
	}

	bool Instance::CheckValidationLayerSupport() {
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		
		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (auto i : validationLayers) {
			bool has = false;
			for (auto j : availableLayers) {
				if (strcmp(i, j.layerName) == 0) {
					has = true;
					break;
				}
			}
			if (!has) return false;
		}
		return true;
	}

	void Instance::InitializeInstance() {	
		if (enableValidationLayers && !CheckValidationLayerSupport()) {
			throw std::runtime_error("A requested validation layer is not available");
		}
		vk::ApplicationInfo appInfo("REngine", vk::makeApiVersion(0, 0, 0, 1), "No engine", vk::makeApiVersion(0, 1, 0, 0), vk::ApiVersion10);
		
		vk::InstanceCreateInfo createInfo({}, &appInfo);
		
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		
		createInfo.enabledExtensionCount = glfwExtensionCount;
		createInfo.ppEnabledExtensionNames = glfwExtensions;
		if (enableValidationLayers) {
			createInfo.enabledLayerCount = uint32_t(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else {
			createInfo.enabledLayerCount = 0;
		}
		
		instance = vk::createInstance(createInfo);
	}

	void Instance::PickPhysicalDevice() {
		uint32_t deviceCount = 0;
		for (auto i : instance.enumeratePhysicalDevices()) {
			if (IsDeviceSuitable(i)) {
				physicalDevice = i;
				info.maxMsaa = GetMaxUsableSampleCount();
				info.queues = QueueFamilyIndices::FindQueueFamilies(physicalDevice, info.surface);
				info.swapchainSupport = QuerySwapChainSupport(physicalDevice);
				info.physicalDevice = physicalDevice;
				break;
			}
		}

		if (physicalDevice == nullptr) {
			throw std::runtime_error("No suitable GPU found!");
		}
	}


	bool Instance::IsDeviceSuitable(vk::PhysicalDevice device) {
		auto features = device.getFeatures();
		auto properties = device.getProperties();
		QueueFamilyIndices indices = QueueFamilyIndices::FindQueueFamilies(device, info.surface);

		bool extensionsSupported = CheckDeviceExtensionSupport(device, deviceExtensions);
		bool swapChainAdequate = false;
		if (extensionsSupported) {
			SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}

		return properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu 
		&& indices.IsComplete() 
		&& swapChainAdequate
		&& features.samplerAnisotropy;
		// TODO: something a bit more fancy
	}

	vk::SampleCountFlagBits Instance::GetMaxUsableSampleCount() {
		auto physicalDeviceProperties = physicalDevice.getProperties();
		vk::SampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
		if (counts & vk::SampleCountFlagBits::e64) { return vk::SampleCountFlagBits::e64; }
		if (counts & vk::SampleCountFlagBits::e32) { return vk::SampleCountFlagBits::e32; }
		if (counts & vk::SampleCountFlagBits::e16) { return vk::SampleCountFlagBits::e16; }
		if (counts & vk::SampleCountFlagBits::e8) { return vk::SampleCountFlagBits::e8; }
		if (counts & vk::SampleCountFlagBits::e4) { return vk::SampleCountFlagBits::e4; }
		if (counts & vk::SampleCountFlagBits::e2) { return vk::SampleCountFlagBits::e2; }

		return vk::SampleCountFlagBits::e1;	
	}

	SwapChainSupportDetails Instance::QuerySwapChainSupport(vk::PhysicalDevice device) {
		SwapChainSupportDetails details;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, info.surface, &details.capabilities);
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, info.surface, &formatCount, nullptr);
		if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, info.surface, &formatCount, details.formats.data());
		}
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, info.surface, &presentModeCount, nullptr);

		if (presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, info.surface, &presentModeCount, details.presentModes.data());
		}

		return details;
	}

	bool Instance::CheckDeviceExtensionSupport(vk::PhysicalDevice device, const std::vector<const char*> &requiredExtensions) {
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());
		
		for (auto i : requiredExtensions) {
			bool available = false;
			for (auto j : availableExtensions) {
				if (strcmp(i, j.extensionName) == 0) {
					available = true;
					break;
				}
			}
			if (!available) {
				return false;
			}
		}
		return true;
	}

}