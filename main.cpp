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
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "core/windowManager.hpp"
#include "core/instance.hpp"
#include "core/image.hpp"
#include "core/queue.hpp"
#include "core/buffer.hpp"
#include "loader/shader.hpp"
#include "core/pipeline.hpp"
#include "core/vertex.hpp"
#include "core/object.hpp"
#include "core/descriptorPool.hpp"
#include "core/renderer.cpp"

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
	Buffer vertexBuffer;
	Buffer indexBuffer;
	uint32_t mipLevels;
	Image textureImage;
	VkSampler textureSampler;
	std::vector<Object> objects;

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

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
		LoadModel();
		CreateVertexBuffer();
		CreateIndexBuffer();
		objects.push_back(Object());
		objects[0].Create(pipeline);
		objects[0].SetImage(textureImage, textureSampler);
	}

	void LoadModel() {
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, MODEL_PATH.c_str())) {
			throw std::runtime_error(warn + err);
		}

		std::unordered_map<Vertex, uint32_t> uniqueVertices{};

		for (auto &shape : shapes) {
			for (auto &index : shape.mesh.indices) {
				Vertex vertex{};

				vertex.pos = {
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2],
				};

				vertex.texCoord = {
					attrib.texcoords [2 * index.texcoord_index + 0],
					1.0f - attrib.texcoords [2 * index.texcoord_index + 1]
				};

				vertex.color = {1.0f, 1.0f, 1.0f};

				if (uniqueVertices.count(vertex) == 0) {
					uniqueVertices[vertex] = uint32_t(vertices.size());
					vertices.push_back(vertex);
				}

				vertices.push_back(vertex);
				indices.push_back(uniqueVertices[vertex]);
			}
		}
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

	void CreateVertexBuffer() {
		vertexBuffer.Create(sizeof(vertices[0]) * vertices.size(), vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer);
		vertexBuffer.Stage(vertices.data());
	}

	void CreateIndexBuffer() {
		indexBuffer.Create(sizeof(indices[0]) * indices.size(), vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer);
		indexBuffer.Stage(indices.data());
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
		while(window.Update()) {
			i %= 1;
			UpdateUniformBuffer(i++);
			renderer.Render(objects, vertexBuffer, indexBuffer, indices.size());
		}

		vkDeviceWaitIdle(device);
	}

	void Cleanup() {
		renderer.Destroy();
		vkDestroySampler(device, textureSampler, nullptr);
		textureImage.Destroy();

		DescriptorPool::Cleanup();
		objects[0].Destroy();
		indexBuffer.Destroy();
		vertexBuffer.Destroy();
		
		REngine::Loader::Shader::Cleanup();
		
		vkDestroyCommandPool(device, Instance::GetInfo().commandPool, nullptr);
		pipeline.Destroy();
		Instance::Destroy();
		vkDestroyDevice(device, nullptr);
		vkDestroySurfaceKHR(Instance::Get(), Instance::GetInfo().surface, nullptr);
		vkDestroyInstance(Instance::Get(), nullptr);
		window.Destroy();
	}
	
	void UpdateUniformBuffer(uint32_t currentImage) {
		static auto startTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
		UniformBufferObject ubo{};
		ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.proj = glm::perspective(glm::radians(45.0f), renderer.swapchain.Extent().width / (float) renderer.swapchain.Extent().height, 0.1f, 10.0f);
		ubo.proj[1][1] *= -1;
		objects[0].uniformBuffers[currentImage].CopyData(&ubo, sizeof(ubo));
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