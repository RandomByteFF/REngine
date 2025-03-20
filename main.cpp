#define GLFW_INCLUDE_VULKAN
#define VMA_IMPLEMENTATION
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "core/windowManager.hpp"
#include "core/instance.hpp"
#include "core/image.hpp"
#include "core/queue.hpp"
#include "core/buffer.hpp"
#include "loader/shader.hpp"
#include "core/pipeline.hpp"
#include "core/vertex.hpp"
#include "core/mesh.hpp"
#include "core/descriptorPool.hpp"
#include "core/renderer.cpp"
#include "loader/obj.hpp"
#include "core/time.hpp"

#include <chrono>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <string.h>
#include <optional>
#include <set>
#include <cstdint>
#include <limits>
#include <algorithm>
#include <fstream>
#include <array>
#include <unordered_map>

using namespace REngine::Core;

const std::string MODEL_PATH = "test_files/viking_room.obj";
const std::string TEXTURE_PATH = "test_files/viking_room.png";
const int MAX_FRAMES_IN_FLIGHT = 2;


class VulkanApp {
public:
	void Run() {
		window.CreateWindow();
		InitVulkan();
		MainLoop();
		Cleanup();
	}
private:
	WindowManager window;
	vk::Device device;
	Pipeline pipeline;
	Renderer renderer;
	uint32_t mipLevels;
	Image textureImage;
	VkSampler textureSampler;
	std::vector<Mesh> objects;
	REngine::Loader::Obj model;


	void InitVulkan() {
		Instance::Initialize(window);
		device = Instance::GetInfo().device;
		renderer.Create(window);	
		pipeline.SetLayout({
			{vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex},
			{vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment}
		});
		pipeline.Create("vert", "frag", renderer.Swapchain(), renderer.RenderPass());
		CreateTextureImage();
		CreateTextureSampler();
		model.Load("test_files/viking_room.obj");
		objects.push_back(Mesh());
		objects[0].Create(pipeline, model.Verticies(), model.Indices());
		objects[0].SetImage(textureImage, textureSampler);
		objects.push_back(Mesh());
		objects[1].Create(pipeline, model.Verticies(), model.Indices());
		objects[1].SetImage(textureImage, textureSampler);
		model.Destroy();
	}



	void CreateTextureImage() {
		int texWidth, texHeight, texChannels;
		stbi_uc *pixels = stbi_load(TEXTURE_PATH.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;
		VkDeviceSize imageSize = texWidth * texHeight * 4;
		if (!pixels) {
			throw std::runtime_error("Failed to load image");
		}

		textureImage.CreateImage(texWidth, texHeight, mipLevels, imageSize, pixels);

		stbi_image_free(pixels);
	}

	bool HasStencilComponent(VkFormat format) {
		return format== VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
	}

	void CreateTextureSampler() {
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_TRUE;

		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(Instance::GetInfo().physicalDevice, &properties);

		samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0;
		samplerInfo.maxLod = VK_LOD_CLAMP_NONE;

		if (vkCreateSampler(device, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create texture sampler!");
		}
	}

	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(Instance::GetInfo().physicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}
		throw std::runtime_error("No memory type found!");
	}

	void MainLoop() {
		int i = 0;
		Time::Start();
		while(window.Update()) {
			i %= 1;
			Time::Tick();
			UpdateUniformBuffer();
			renderer.Render(objects);
		}

		vkDeviceWaitIdle(device);
	}

	void Cleanup() {
		renderer.Destroy();
		vkDestroySampler(device, textureSampler, nullptr);
		textureImage.Destroy();

		DescriptorPool::Cleanup();
		for(auto i : objects) {
			i.Destroy();
		}
		
		REngine::Loader::Shader::Cleanup();
		
		vkDestroyCommandPool(device, Instance::GetInfo().commandPool, nullptr);
		pipeline.Destroy();
		Instance::Destroy();
		vkDestroyDevice(device, nullptr);
		vkDestroySurfaceKHR(Instance::Get(), Instance::GetInfo().surface, nullptr);
		vkDestroyInstance(Instance::Get(), nullptr);
		window.Destroy();
	}
	
	void UpdateUniformBuffer() {
		objects[0].Rotate(Time::Delta() * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		objects[1].Rotate(Time::Delta() * glm::radians(10.0f), glm::vec3(0.0f, 1.0f, 1.0f));
	}
};

int main() {
	VulkanApp app;

	try
	{
		app.Run();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;	
}