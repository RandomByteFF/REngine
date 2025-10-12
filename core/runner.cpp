#include "runner.hpp"
#include "GLFW/glfw3.h"
#include "input/mouse.hpp"
#include "input/keyboard.hpp"
#include "scene/textureMesh.hpp"
#include "time.hpp"
#include "descriptorPool.hpp"
#include "loader/shader.hpp"
#include <iostream>
#include <numbers>

namespace REngine::Core {
	void Runner::InitVulkan() {
		Instance::Initialize(window);
		device = Instance::GetInfo().device;
		#ifdef EDITOR
		tree = std::make_shared<Scene::SceneTree>();
		tree->SetRoot(std::shared_ptr<Scene::Node>(new Scene::Node()));
		portal1 = std::shared_ptr<Scene::Portal>(new Scene::Portal());
		portal2 = std::shared_ptr<Scene::Portal>(new Scene::Portal());
		portal2->SetPair(portal1);
		portal1->SetPair(portal2);
		portal1->Position(glm::vec3(0, 1.3, 0.5));
		portal2->Position(glm::vec3(2, 1.3, 0.5));
		#else
		tree = Scene::Deserializer::loadTree("tree.rest");
		#endif
		tree->SetCurrent();
		
		renderer.Create();
		camera = std::shared_ptr<Camera>(new Camera(renderer.AspectRatio()));
		
		testMesh = std::shared_ptr<Scene::TextureMesh>(new Scene::TextureMesh(renderer.GetRenderPass(), "test_files/viking_room.obj", "test_files/viking_room.png"));
		levelMesh = std::shared_ptr<Scene::TextureMesh>(new Scene::TextureMesh(renderer.GetRenderPass(), "test_files/rengine-level1.obj", "test_files/levelTexture.png"));
		frame = std::shared_ptr<Scene::TextureMesh>(new Scene::TextureMesh(renderer.GetRenderPass(), "test_files/frame.obj", "test_files/black.png"));
		player = std::shared_ptr<Scene::Player>(new Scene::Player(renderer.GetRenderPass()));
		tree->GetRoot()->AddChild(player);
		tree->GetRoot()->AddChild(testMesh);
		tree->GetRoot()->AddChild(levelMesh);
		player->AddChild(camera);
		
		tree->GetRoot()->AddChild(portal1);
		tree->GetRoot()->AddChild(portal2);
		portal1->AddChild(frame);
		
		portal1->Create(renderer.GetRenderPass());
		portal1->SetSampler(renderer.Sampler());
		portal2->Create(renderer.GetRenderPass());
		portal2->SetSampler(renderer.Sampler());
		levelMesh->name = "Level";
		testMesh->name = "TestMesh";

		tree->SetActiveCamera(camera);

		camera->Rotation(glm::vec3(0., std::numbers::pi, 0.));
		camera->Position(glm::vec3(0.f, 0.7f, 0.f));
	}

	void Runner::MainLoop() {
		Time::Start();
		camera->Rotate(glm::vec3(0.f));
		while(window.Update()) {
			Time::Tick();
			Input::Mouse::RecordDelta();

			tree->Update();

			renderer.Render(*tree, *camera);
			Input::Keyboard::EndFrame();
			Input::Mouse::EndFrame();
		}

		vkDeviceWaitIdle(device);
	}

	void Runner::Cleanup() {
		renderer.Destroy();
		
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
