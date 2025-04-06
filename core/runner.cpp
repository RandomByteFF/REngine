#include "runner.hpp"
#include "imgui_impl_vulkan.h"
#include "imgui_impl_glfw.h"

namespace REngine::Core {
	void Runner::InitVulkan() {
		Instance::Initialize(window);
		device = Instance::GetInfo().device;
		renderer.Create(window);
		pipeline.SetLayout({
			{vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex},
			{vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment}
		});
		pipeline.Create("vertex", "fragment", renderer.GetSwapchain(), renderer.RenderPass());
		textureImage.CreateImage(REngine::Loader::Image("test_files/viking_room.png"));
		model.Load("test_files/viking_room.obj");
		objects.push_back(Mesh());
		objects[0].Create(pipeline, model.Verticies(), model.Indices());
		objects[0].SetImage(textureImage, renderer.Sampler());
		// objects.push_back(Mesh());
		// objects[1].Create(pipeline, model.Verticies(), model.Indices());
		// objects[1].SetImage(textureImage, renderer.Sampler());
		camera.SetLook(glm::vec3(0.0f, 0.f, 3.f), glm::vec3(0.0f, 0.0f, 0.0f));
		model.Destroy();

	}

	void Runner::MainLoop() {
		objects[0].Rotate(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		Time::Start();
		while(window.Update()) {
			Time::Tick();
			objects[0].Rotate(Time::Delta() * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			// objects[1].Rotate(Time::Delta() * glm::radians(10.0f), glm::vec3(0.0f, 1.0f, 1.0f));
			renderer.Render(objects, camera);
		}

		vkDeviceWaitIdle(device);
	}

	void Runner::Cleanup() {
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

	int Runner::Run() {
		try	{
			ImGui::CreateContext();
			window.CreateWindow();
			InitVulkan();
			MainLoop();
			Cleanup();
		}
		catch(const std::exception& e) {
			std::cerr << e.what() << '\n';
			return EXIT_FAILURE;
		}

		return EXIT_SUCCESS;	
	}
}
