#include "runner.hpp"
#include "input/mouse.hpp"
#include "input/keyboard.hpp"
#include "scene/portal.hpp"
#include "scene/sceneTree.hpp"
#include "scene/textureMesh.hpp"
#include "time.hpp"
#include "descriptorPool.hpp"
#include "loader/shader.hpp"
#include <iostream>
#include <memory>
#include <numbers>
#include "scene/deserializer.hpp"

namespace REngine::Core {
	void Runner::Init() {
		Instance::Initialize(window);
		device = Instance::GetInfo().device;
		tree = std::make_shared<Scene::SceneTree>();
		tree->SetRoot(std::shared_ptr<Scene::Node>(new Scene::Node()));
		portals.resize(6);

		tree->SetCurrent();
		
		renderer.Create();
		camera = std::shared_ptr<Camera>(new Camera(renderer.AspectRatio()));
		
		testLevel = std::shared_ptr<Scene::TextureMesh>(new Scene::TextureMesh(renderer.GetRenderPass(), "test_files/exit8.obj", "test_files/checker.png"));
		poster = std::shared_ptr<Scene::TextureMesh>(new Scene::TextureMesh(renderer.GetRenderPass(), "test_files/poster.obj", "test_files/checker.png"));
		player = std::shared_ptr<Scene::Player>(new Scene::Player(renderer.GetRenderPass()));
		tree->GetRoot()->AddChild(player);
		tree->GetRoot()->AddChild(testLevel);
		tree->GetRoot()->AddChild(poster);
		player->AddChild(camera);


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

			std::shared_ptr<Scene::Portal> sp = std::dynamic_pointer_cast<Scene::Portal>(Scene::SceneTree::Current()->Find("SouthPortal", *Scene::SceneTree::Current()->GetRoot()));
			std::shared_ptr<Scene::Portal> np = std::dynamic_pointer_cast<Scene::Portal>(Scene::SceneTree::Current()->Find("NorthPortal", *Scene::SceneTree::Current()->GetRoot()));

			bool teleported = false;
			bool tnteleported = northTeleported;
			if (sp->DidTeleport()) {
				northTeleported = false;
				teleported = true;
			}
			if (np->DidTeleport()) {
				northTeleported = true;
				teleported = true;
			}

			if (teleported) {
				if (tnteleported == northTeleported) {
					std::cout << "Nothing changed" << std::endl;
				}
				else {
					std::cout << "Something changed" << std::endl;
				}

			}

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
			Init();
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
