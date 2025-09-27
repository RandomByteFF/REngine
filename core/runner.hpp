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
		Renderer renderer;
		Image textureImage;
		std::shared_ptr<Scene::SceneTree> tree;
		std::shared_ptr<Scene::Mesh> testMesh;
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