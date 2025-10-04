#include "runner.hpp"
#include "GLFW/glfw3.h"
#include "input/mouse.hpp"
#include "input/keyboard.hpp"
#include "loader/image.hpp"
#include "scene/mesh.hpp"
#include "scene/textureMesh.hpp"
#include "time.hpp"
#include "descriptorPool.hpp"
#include "loader/shader.hpp"
#include <iostream>

namespace REngine::Core {
	void Runner::InitVulkan() {
		Instance::Initialize(window);
		device = Instance::GetInfo().device;
		#ifdef EDITOR
		tree = std::make_shared<Scene::SceneTree>();
		tree->SetRoot(std::shared_ptr<Scene::Node>(new Scene::Node()));
		// arrowMesh = std::shared_ptr<Scene::Mesh>(new Scene::Mesh());
		testMesh = std::shared_ptr<Scene::TextureMesh>(new Scene::TextureMesh());
		levelMesh = std::shared_ptr<Scene::TextureMesh>(new Scene::TextureMesh());
		portal1 = std::shared_ptr<Scene::Portal>(new Scene::Portal());
		portal2 = std::shared_ptr<Scene::Portal>(new Scene::Portal());
		frame = std::shared_ptr<Scene::TextureMesh>(new Scene::TextureMesh());
		portal2->SetPair(portal1);
		portal1->SetPair(portal2);
		portal1->Position(glm::vec3(0, 1.3, 0.5));
		portal2->Position(glm::vec3(2, 1.3, 0.5));
		tree->GetRoot()->AddChild(testMesh);
		tree->GetRoot()->AddChild(levelMesh);
		tree->GetRoot()->AddChild(portal1);
		tree->GetRoot()->AddChild(portal2);
		portal1->AddChild(frame);
		// testMesh->AddChild(arrowMesh);
		#else
		tree = Scene::Deserializer::loadTree("tree.rest");
		#endif
		tree->SetCurrent();

		renderer.Create();
		camera = std::shared_ptr<Camera>(new Camera(renderer.AspectRatio()));
		textureImage.CreateImage(REngine::Loader::Image("test_files/viking_room.png"));
		levelTexture.CreateImage(REngine::Loader::Image("test_files/levelTexture.png"));
		frameTexture.CreateImage(REngine::Loader::Image("test_files/black.png"));
		
		model.Load("test_files/viking_room.obj");
		testMesh = std::dynamic_pointer_cast<Scene::TextureMesh>(tree->GetRoot()->Children()[0]);
		testMesh->Create(renderer.GetRenderPass(), model.Verticies(), model.Indices());
		model.Destroy();
		model.Load("test_files/rengine-level1.obj");
		levelMesh->Create(renderer.GetRenderPass(), model.Verticies(), model.Indices());
		model.Destroy();
		model.Load("test_files/frame.obj");
		frame->Create(renderer.GetRenderPass(), model.Verticies(), model.Indices());
		portal1->Create(renderer.GetRenderPass());
		portal1->SetSampler(renderer.Sampler());
		portal2->Create(renderer.GetRenderPass());
		portal2->SetSampler(renderer.Sampler());
		testMesh->SetImage(textureImage, renderer.Sampler());
		levelMesh->SetImage(levelTexture, renderer.Sampler());
		frame->SetImage(frameTexture, renderer.Sampler());
		levelMesh->name = "Level";
		testMesh->name = "TestMesh";

		tree->SetActiveCamera(camera);

		camera->SetPosition(glm::vec3(0.f, 0.f, 6.f));
		// arrow.Destroy();
	}

	void Runner::MainLoop() {
		// testMesh->Rotate(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		Time::Start();
		camera->Rotate(glm::vec3(0.f));
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
			tree->ApplyTransforms();
			renderer.Render(*tree, *camera);
			Input::Keyboard::EndFrame();
		}

		vkDeviceWaitIdle(device);
	}

	void Runner::Cleanup() {
		renderer.Destroy();
		textureImage.Destroy();
		levelTexture.Destroy();
		frameTexture.Destroy();
		
		DescriptorPool::Cleanup();
		tree->Destroy();
		
		REngine::Loader::Shader::Destroy();
		
		vkDestroyCommandPool(device, Instance::GetInfo().commandPool, nullptr);
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
