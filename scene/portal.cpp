#include "portal.hpp"
#include "core/instance.hpp"
#include "core/vertex.hpp"
#include "scene/sceneTree.hpp"
#include "vulkan/vulkan_structs.hpp"
#include <memory>

namespace REngine::Scene {
	void Portal::Create(vk::RenderPass rp) {
		portalCounter++;
		if (!pipeline) {
			pipeline = std::make_shared<Core::Pipeline>();
			pipeline->SetLayout({
				{vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex},
			});
			pipeline->SetInput({Vertex::GetBindingDescription()}, Vertex::GetAttributeDescriptions());
			pipeline->Create("portalVert", "portalFrag", rp);

			renderPass.AddColorAttachment().finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
			renderPass.AddColorImage();
		}
		pPipeline = pipeline;

		std::vector<Vertex> vertices {
			Vertex{glm::vec3(-1, -1, 0), glm::vec3(0, 0, 0), glm::vec2(0, 1)}, 
			Vertex{glm::vec3(1, -1, 0), glm::vec3(0, 0, 0), glm::vec2(1, 0)},
			Vertex{glm::vec3(1, 1, 0), glm::vec3(0, 0, 0), glm::vec2(1, 0)},
			Vertex{glm::vec3(-1, 1, 0), glm::vec3(0, 0, 0), glm::vec2(0, 0)}};
		std::vector<uint32_t> indices = {0, 1, 2, 0, 2, 3};
		Mesh::Create(rp, vertices, indices);
	}

	void Portal::PreDraw(Core::CommandBuffer cb) {
		auto info = Core::Instance::GetInfo();
		cb.BeginPass(renderPass.GetRenderPass(), info.swapchainExtent, renderPass.GetFramebuffer()[info.currentFb]);
		SceneTree::Current()->CallDrawlist([&cb](Drawable &j) {
			j.DrawFromView(cb.GetBuffer(), *SceneTree::Current()->ActiveCamera());
		});
	}

	void Portal::Destroy() {
		Mesh::Destroy();
		renderPass.Destroy();
		if (--portalCounter == 0) {
			pipeline->Destroy();
			pipeline = nullptr;
		}
	}
}