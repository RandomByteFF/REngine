#include "runner.hpp"
#include "input/mouse.hpp"
#include "input/keyboard.hpp"
#include "scene/deserializer.hpp"

namespace REngine::Core {
	void Runner::InitVulkan() {
		Instance::Initialize(window);
		device = Instance::GetInfo().device;
		#ifdef EDITOR
		tree = std::make_shared<Scene::SceneTree>();
		tree->SetRoot(std::shared_ptr<Scene::Node>(new Scene::Node()));
		// arrowMesh = std::shared_ptr<Scene::Mesh>(new Scene::Mesh());
		testMesh = std::shared_ptr<Scene::Mesh>(new Scene::Mesh());
		tree->GetRoot()->AddChild(testMesh);
		// testMesh->AddChild(arrowMesh);
		#else
		tree = Scene::Deserializer::loadTree("tree.rest");
		#endif
		tree->SetCurrent();

		renderer.Create(window);
		camera = std::shared_ptr<Camera>(new Camera(renderer.AspectRatio()));
		textureImage.CreateImage(REngine::Loader::Image("test_files/viking_room.png"));
		
		model.Load("test_files/viking_room.obj");
		testMesh = std::dynamic_pointer_cast<Scene::Mesh>(tree->GetRoot()->Children()[0]);
		testMesh->Create(renderer.RenderPass(), model.Verticies(), model.Indices());
		testMesh->SetImage(textureImage, renderer.Sampler());
		testMesh->name = "TestMesh";

		tree->SetActiveCamera(camera);

		camera->SetPosition(glm::vec3(0.f, 0.f, 6.f));
		model.Destroy();
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
