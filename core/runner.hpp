#pragma once

#include "core/windowManager.hpp"
#include "core/instance.hpp"
#include "core/image.hpp"
#include "loader/shader.hpp"
#include "core/pipeline.hpp"
#include "core/mesh.hpp"
#include "core/descriptorPool.hpp"
#include "core/renderer.hpp"
#include "loader/obj.hpp"
#include "core/time.hpp"

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
		void InitVulkan();
		void MainLoop();
		void Cleanup();
	public: 
		int Run();
	};
}