#include "runner.hpp"
#include "imgui_impl_vulkan.h"
#include "imgui_impl_glfw.h"
#include "input/mouse.hpp"
#include "input/keyboard.hpp"

namespace REngine::Core {
	void Runner::InitVulkan() {
		Instance::Initialize(window);
		device = Instance::GetInfo().device;
		tree.SetCurrent();
		tree.SetRoot(std::shared_ptr<Scene::Node>(new Scene::Node()));

		renderer.Create(window);
		camera = std::shared_ptr<Camera>(new Camera(renderer.AspectRatio()));
		pipeline.SetLayout({
			{vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex},
			{vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment}
		});
		whitePipeline.SetLayout({
			{vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex}
		});
		pipeline.Create("vertex", "fragment", renderer.GetSwapchain(), renderer.RenderPass());
		whitePipeline.Create("vertex", "whiteFrag", renderer.GetSwapchain(), renderer.RenderPass());
		textureImage.CreateImage(REngine::Loader::Image("test_files/viking_room.png"));
		
		model.Load("test_files/viking_room.obj");
		testMesh = std::shared_ptr<Scene::Mesh>(new Scene::Mesh());
		testMesh->Create(pipeline, model.Verticies(), model.Indices());
		testMesh->SetImage(textureImage, renderer.Sampler());
		testMesh->name = "TestMesh";
		tree.SetActiveCamera(camera);
		tree.GetRoot()->AddChild(testMesh);

		Loader::Obj arrow;
		arrow.Load("test_files/arrow.obj");
		arrowMesh = std::shared_ptr<Scene::Mesh>(new Scene::Mesh());
		arrowMesh->Create(whitePipeline, arrow.Verticies(), arrow.Indices());
		arrowMesh->name = "Arrow";
		testMesh->AddChild(arrowMesh);

		// objects.push_back(Mesh());
		// objects[1].Create(pipeline, model.Verticies(), model.Indices());
		// objects[1].SetImage(textureImage, renderer.Sampler());
		camera->SetPosition(glm::vec3(0.f, 0.f, 6.f));
		model.Destroy();
		arrow.Destroy();
	}

	void Runner::MainLoop() {
		// testMesh->Rotate(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		Time::Start();
		while(window.Update()) {
			Time::Tick();
			Input::Mouse::RecordDelta();
			if (Input::Keyboard::IsDown(GLFW_KEY_1)) {
				Input::Mouse::Lock();
				locked = true;
			}
			if (Input::Keyboard::IsDown(GLFW_KEY_ESCAPE)) {
				Input::Mouse::Unlock();
				locked = false;
			}
			if (locked) {
				camera->Rotate(glm::vec3(-Input::Mouse::Delta().y, -Input::Mouse::Delta().x, 0.f) * Time::Delta());
				if (Input::Keyboard::IsDown(GLFW_KEY_W)) camera->SetPosition(camera->GetPosition() + camera->Forward() * Time::Delta());
				if (Input::Keyboard::IsDown(GLFW_KEY_S)) camera->SetPosition(camera->GetPosition() - camera->Forward() * Time::Delta());
				if (Input::Keyboard::IsDown(GLFW_KEY_D)) camera->SetPosition(camera->GetPosition() + camera->Right() * Time::Delta());
				if (Input::Keyboard::IsDown(GLFW_KEY_A)) camera->SetPosition(camera->GetPosition() - camera->Right() * Time::Delta());
				if (Input::Keyboard::IsDown(GLFW_KEY_SPACE)) camera->SetPosition(camera->GetPosition() + camera->Up() * Time::Delta());
				if (Input::Keyboard::IsDown(GLFW_KEY_Q)) camera->SetPosition(camera->GetPosition() - camera->Up() * Time::Delta());
				if (Input::Keyboard::IsDown(GLFW_KEY_F)) testMesh->Rotate(glm::vec3(0.f, 0.f, 1.f) * Time::Delta());
			}
			tree.ApplyTransforms();
			renderer.Render(tree, *camera);
			Input::Keyboard::EndFrame();
		}

		vkDeviceWaitIdle(device);
	}

	void Runner::Cleanup() {
		renderer.Destroy();
		textureImage.Destroy();
		
		DescriptorPool::Cleanup();
		tree.Destroy();
		
		REngine::Loader::Shader::Destroy();
		
		vkDestroyCommandPool(device, Instance::GetInfo().commandPool, nullptr);
		pipeline.Destroy();
		whitePipeline.Destroy();
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
