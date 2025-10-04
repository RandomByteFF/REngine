#pragma once

#include "windowManager.hpp"
#include "image.hpp"
#include "pipeline.hpp"
#include "scene/portal.hpp"
#include "scene/textureMesh.hpp"
#include "renderer.hpp"
#include "loader/obj.hpp"
#include "camera.hpp"

namespace REngine::Core {
	class Runner {
		WindowManager window;
		vk::Device device;
		Pipeline pipeline;
		Renderer renderer;
		Image textureImage;
		Image levelTexture;
		Image frameTexture;
		std::shared_ptr<Scene::SceneTree> tree;
		std::shared_ptr<Scene::TextureMesh> testMesh;
		std::shared_ptr<Scene::TextureMesh> levelMesh;
		std::shared_ptr<Scene::Portal> portal1;
		std::shared_ptr<Scene::Portal> portal2;
		std::shared_ptr<Scene::TextureMesh> frame;
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