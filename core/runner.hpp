#pragma once

#include "windowManager.hpp"
#include "pipeline.hpp"
#include "scene/textureMesh.hpp"
#include "renderer.hpp"
#include "camera.hpp"
#include "scriptEngine.hpp"

namespace REngine::Core {
	class Runner {
		WindowManager window;
		vk::Device device;
		Pipeline pipeline;
		Renderer renderer;
		std::shared_ptr<Scene::SceneTree> tree;
		std::shared_ptr<Scene::TextureMesh> testLevel;
		std::shared_ptr<Camera> camera;
		std::shared_ptr<Editor::Grid> grid;
		ScriptEngine scriptEngine;

		void Init();
		void MainLoop();
		void Cleanup();
	public: 
		int Run();
	};
}