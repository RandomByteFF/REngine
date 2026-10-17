#pragma once

#include "core/renderPass.hpp"
#include "scene/sceneTree.hpp"
#include "vulkan/vulkan_handles.hpp"
namespace REngine::Editor {
	class GameView {
		Core::RenderPass renderPass;
		bool running = false;
	public:
		void Initialize();
		int height = 1080;
		int width = 1920;
		void Render(Scene::SceneTree &sceneTree);
		void Stop();
		std::vector<vk::ImageView> GetImages();
	};
}