#pragma once

#include "windowManager.hpp"
#include "pipeline.hpp"
#include "scene/portal.hpp"
#include "scene/textureMesh.hpp"
#include "renderer.hpp"
#include "camera.hpp"
#include "scene/player.hpp"

namespace REngine::Core {
	class Runner {
		WindowManager window;
		vk::Device device;
		Pipeline pipeline;
		Renderer renderer;
		std::shared_ptr<Scene::SceneTree> tree;
		std::vector<std::shared_ptr<Scene::Portal>> portals;
		std::shared_ptr<Scene::TextureMesh> testLevel;
		std::shared_ptr<Scene::Player> player;
		std::shared_ptr<Camera> camera;
		void InitVulkan();
		void MainLoop();
		void Cleanup();
	public: 
		int Run();
	};
}