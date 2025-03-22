#include "instance.hpp"

#include <set>
#include <GLFW/glfw3.h>

#include "queueFamily.hpp"
#include "swapchain.hpp"

namespace REngine::Core {
	void Instance::Initialize(WindowManager manager) {
		InitializeInstance();
		manager.CreateSurface(Instance::Get(), info.surface);
		PickPhysicalDevice();
		CreateLogicalDevice();
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
				info.swapchainSupport = Swapchain::QuerySwapchainSupport(physicalDevice);
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
			SwapchainSupportDetails swapChainSupport = Swapchain::QuerySwapchainSupport(device);
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

	void Instance::CreateLogicalDevice() {
		QueueFamilyIndices indices = Instance::GetInfo().queues;
		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};
		std::array<float, 1> queuePriority = {1.0f};

		for (auto queueFamily : uniqueQueueFamilies) {
			vk::DeviceQueueCreateInfo queueCreateInfo(vk::DeviceQueueCreateFlags(), queueFamily, queuePriority);
			queueCreateInfos.push_back(queueCreateInfo);
		}

		vk::PhysicalDeviceFeatures physicalDeviceFeatures {};
		physicalDeviceFeatures.samplerAnisotropy = true;

		vk::DeviceCreateInfo createInfo {};
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.queueCreateInfoCount = uint32_t(queueCreateInfos.size());
		createInfo.pEnabledFeatures = &physicalDeviceFeatures;
		createInfo.enabledExtensionCount = uint32_t(deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();

		if (enableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		} else {
			createInfo.enabledLayerCount = 0;
		}

		device = physicalDevice.createDevice(createInfo);
		info.device = device;
		graphicsQueue = device.getQueue(indices.graphicsFamily.value(), 0);
		presentQueue = device.getQueue(indices.presentFamily.value(), 0);
		info.graphicsQueue = graphicsQueue;
		info.presentQueue = presentQueue;

		VmaAllocatorCreateInfo allocCreate{};
		allocCreate.device = device;
		allocCreate.instance = instance;
		allocCreate.physicalDevice = physicalDevice;
		vmaCreateAllocator(&allocCreate, &allocator);
		info.allocator = allocator;

		vk::CommandPoolCreateInfo poolInfo;
		poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
		poolInfo.queueFamilyIndex = indices.graphicsFamily.value();
		poolInfo.pNext = nullptr;
		commandPool = device.createCommandPool(poolInfo);
		info.commandPool = commandPool;
	}

	void Instance::SetCurrentFrame(uint32_t frame) {
		info.currentFrame = frame;
	}

	void Instance::Destroy() {
		vmaDestroyAllocator(allocator);
	}

	void Instance::FrameBufferResized(int width, int height) {
		info.fbHeight = height;
		info.fbWidth = width;
	}
}