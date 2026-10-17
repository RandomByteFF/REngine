#include "gameView.hpp"
#include "core/instance.hpp"
#include "core/renderStore.hpp"
#include "scene/sceneTree.hpp"
#include "vulkan/vulkan_handles.hpp"

namespace REngine::Editor {
	void GameView::Initialize() {
		renderPass = (Core::Instance::GetInfo().maxMsaa);
		renderPass.AddColorAttachment({.sampled = true, .layoutInfo = {.finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal}});
		renderPass.SetDepthAttachment({});
		running = true;
	}

	void GameView::Render(Scene::SceneTree &sceneTree) {
		auto cb = Core::RenderStore::Get().lock()->cb;
		auto info = Core::Instance::GetInfo();
		renderPass.Begin(info.swapchainExtent, info.currentFb);
		std::shared_ptr<Core::Camera> camera = sceneTree.ActiveCamera();
		
		float aspect = float(width) / height;
		if (abs(aspect - camera->Aspect()) > 0.001) camera->Aspect(aspect);
		Scene::SceneTree::Current()->CallDrawlist([&cb, &camera](Scene::Drawable &j) {
			if (j.renderMask & (1)) {
				j.DrawFromView(cb.GetBuffer(), *camera);
			}
		});
		renderPass.End();
	}

	void GameView::Stop() {
		renderPass.Destroy();
		running = false;
	}

	std::vector<vk::ImageView> GameView::GetImages() {
		return renderPass.GetColorView(0).lock()->Views();
	}
}