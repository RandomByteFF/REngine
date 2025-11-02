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
		std::vector<std::shared_ptr<Scene::TextureMesh>> numberPosters;
		std::vector<std::shared_ptr<Scene::TextureMesh>> posters;
		std::shared_ptr<Scene::Player> player;
		std::shared_ptr<Camera> camera;
		std::shared_ptr<Editor::Grid> grid;

		bool northTeleported = true;
		int round = 0;
		bool didChange = false;

		void Init();
		void MainLoop();
		void Cleanup();
	public: 
		int Run();
	};
}