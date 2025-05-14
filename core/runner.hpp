#pragma once

#include "windowManager.hpp"
#include "instance.hpp"
#include "image.hpp"
#include "loader/shader.hpp"
#include "pipeline.hpp"
#include "scene/mesh.hpp"
#include "descriptorPool.hpp"
#include "renderer.hpp"
#include "loader/obj.hpp"
#include "time.hpp"
#include "camera.hpp"

#include <iostream>

namespace REngine::Core {
	class Runner {
		WindowManager window;
		vk::Device device;
		Pipeline pipeline;
		Pipeline whitePipeline;
		Renderer renderer;
		Image textureImage;
		Scene::SceneTree tree;
		std::shared_ptr<Scene::Mesh> testMesh;
		std::shared_ptr<Scene::Mesh> arrowMesh;
		REngine::Loader::Obj model;
		std::shared_ptr<Camera> camera;
		void InitVulkan();
		void MainLoop();
		void Cleanup();
		bool locked = false;
	public: 
		int Run();
	};
}