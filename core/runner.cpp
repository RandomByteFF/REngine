#include "runner.hpp"
#include "input/mouse.hpp"
#include "input/keyboard.hpp"
#include "scene/textureMesh.hpp"
#include "time.hpp"
#include "descriptorPool.hpp"
#include "loader/shader.hpp"
#include <iostream>
#include <numbers>
#include "scene/deserializer.hpp"

namespace REngine::Core {
	void Runner::InitVulkan() {
		Instance::Initialize(window);
		device = Instance::GetInfo().device;
		// Portal setup
		tree = std::make_shared<Scene::SceneTree>();
		tree->SetRoot(std::shared_ptr<Scene::Node>(new Scene::Node()));
		portals.resize(6);
		// for(size_t i = 0; i < portals.size(); i++) {
		// 	portals[i] = std::shared_ptr<Scene::Portal>(new Scene::Portal());
		// }
		// for(size_t i = 0; i < portals.size(); i += 2) {
		// 	portals[i]->SetPair(portals[i + 1]);
		// 	portals[i + 1]->SetPair(portals[i]);
		// }
		
		// for(size_t i = 0; i < portals.size(); i++) {
		// 	portals[i]->name = std::format("Portal {}", i);
		// }

		tree->SetCurrent();
		
		renderer.Create();
		camera = std::shared_ptr<Camera>(new Camera(renderer.AspectRatio()));
		
		testLevel = std::shared_ptr<Scene::TextureMesh>(new Scene::TextureMesh(renderer.GetRenderPass(), "test_files/test_level.obj", "test_files/checker.png"));
		player = std::shared_ptr<Scene::Player>(new Scene::Player(renderer.GetRenderPass()));
		tree->GetRoot()->AddChild(player);
		tree->GetRoot()->AddChild(testLevel);
		player->AddChild(camera);

		// for (size_t i = 0; i < portals.size(); i++) {
		// 	tree->GetRoot()->AddChild(portals[i]);
		// }
		
		// tree->GetRoot()->AddChild(portal1);
		// tree->GetRoot()->AddChild(portal2);
		
		// portal1->Create(renderer.GetRenderPass());
		// portal1->SetSampler(renderer.Sampler());
		// portal2->Create(renderer.GetRenderPass());
		// portal2->SetSampler(renderer.Sampler());

		tree->SetActiveCamera(camera);

		camera->Rotation(glm::vec3(0., std::numbers::pi, 0.));
		camera->Position(glm::vec3(0.f, 0.7f, 0.f));
		
		Scene::Deserializer::loadTree("tree.rest");
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
