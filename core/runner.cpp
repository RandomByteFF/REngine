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
#include <cstdlib>

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
		player = std::shared_ptr<Scene::Player>(new Scene::Player(renderer.GetRenderPass()));
		tree->GetRoot()->AddChild(player);
		tree->GetRoot()->AddChild(testLevel);
		player->AddChild(camera);
		numberPosters.resize(8);
		for (size_t i = 0; i < 8; i++) {
			std::string n = std::format("test_files/posters/{}.png", i + 1);
			numberPosters[i] = std::shared_ptr<Scene::TextureMesh>(new Scene::TextureMesh(renderer.GetRenderPass(), "test_files/poster.obj", n));
			tree->GetRoot()->AddChild(numberPosters[i]);
			numberPosters[i]->visible = false;
		}

		posters.resize(16);
		for (size_t i = 0; i < 16; i++) {
			std::string n = std::format("test_files/posters/p{}_{}.png", int(i / 2) + 1, i % 2 + 1);
			posters[i] = std::shared_ptr<Scene::TextureMesh>(new Scene::TextureMesh(renderer.GetRenderPass(), "test_files/poster.obj", n));
			tree->GetRoot()->AddChild(posters[i]);
			if (i % 2 == 1) posters[i]->visible = false;
		}


		tree->SetActiveCamera(camera);

		camera->Rotation(glm::vec3(0., std::numbers::pi, 0.));
		camera->Position(glm::vec3(0.f, 0.7f, 0.f));
		
		Scene::Deserializer::loadTree("tree.rest");

		numberPosters[0]->visible = true;
	}

	void Runner::MainLoop() {
		std::srand(std::time({}));
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
					if (didChange) round = 0;
					else round++;
				}
				else {
					if (didChange) round++;
					else round = 0;
					std::cout << "Something changed" << std::endl;
				}
				round = std::clamp(round, 0, 7);
				for(auto i : numberPosters) i->visible = false;
				numberPosters[round]->visible = true;

				for (size_t i = 0; i < posters.size(); i++) {
					posters[i]->visible = i % 2 == 0;
				}

				bool isAnomalous = std::rand() > RAND_MAX / 2;
				didChange = isAnomalous;
				std::cout << (didChange ? "Anomalous round" : "Non anomalous round") << std::endl;

				if (isAnomalous) {
					int toChange = std::rand() % 8;
					posters[toChange * 2]->visible = false;
					posters[toChange * 2 + 1]->visible = true;
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
