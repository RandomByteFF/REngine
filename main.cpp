#define GLFW_INCLUDE_VULKAN
#define VMA_IMPLEMENTATION
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define STB_IMAGE_IMPLEMENTATION

#include "core/windowManager.hpp"
#include "core/instance.hpp"
#include "core/image.hpp"
#include "loader/shader.hpp"
#include "core/pipeline.hpp"
#include "core/mesh.hpp"
#include "core/descriptorPool.hpp"
#include "core/renderer.cpp"
#include "loader/obj.hpp"
#include "core/time.hpp"

#include <iostream>

using namespace REngine::Core;

const std::string MODEL_PATH = "test_files/viking_room.obj";
const std::string TEXTURE_PATH = "test_files/viking_room.png";


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
	Image textureImage;
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
		pipeline.Create("vert", "frag", renderer.GetSwapchain(), renderer.RenderPass());
		textureImage.CreateImage(REngine::Loader::Image(TEXTURE_PATH));
		model.Load("test_files/viking_room.obj");
		objects.push_back(Mesh());
		objects[0].Create(pipeline, model.Verticies(), model.Indices());
		objects[0].SetImage(textureImage, renderer.Sampler());
		objects.push_back(Mesh());
		objects[1].Create(pipeline, model.Verticies(), model.Indices());
		objects[1].SetImage(textureImage, renderer.Sampler());
		model.Destroy();
	}
	
	void MainLoop() {
		Time::Start();
		while(window.Update()) {
			Time::Tick();
			UpdateUniformBuffer();
			renderer.Render(objects);
		}

		vkDeviceWaitIdle(device);
	}

	void Cleanup() {
		renderer.Destroy();
		textureImage.Destroy();

		DescriptorPool::Cleanup();
		for(auto i : objects) {
			i.Destroy();
		}
		
		REngine::Loader::Shader::Destroy();
		
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