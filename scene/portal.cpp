#include "portal.hpp"
#include "core/vertex.hpp"

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

	void Portal::Destroy() {
		Mesh::Destroy();
		if (--portalCounter == 0) {
			pipeline->Destroy();
			pipeline = nullptr;
		}
	}
}