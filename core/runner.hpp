#pragma once

#include "windowManager.hpp"
#include "instance.hpp"
#include "image.hpp"
#include "loader/shader.hpp"
#include "pipeline.hpp"
#include "drawable/mesh.hpp"
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
		std::vector<Mesh> objects;
		REngine::Loader::Obj model;
		Camera camera;
		void InitVulkan();
		void MainLoop();
		void Cleanup();
	public: 
		int Run();
	};
}