#include "textureMesh.hpp"
#include "core/descriptorPool.hpp"

namespace REngine::Scene {
	void TextureMesh::Create(vk::RenderPass rp, std::vector<Vertex> &vertices, std::vector<uint32_t> &indices) {
		meshCounter++;
		if (!pipeline) {
			pipeline = std::make_shared<Core::Pipeline>();
			pipeline->SetLayout({
				{vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex},
				{vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment}
			});
			pipeline->SetInput({Vertex::GetBindingDescription()}, Vertex::GetAttributeDescriptions());
			pipeline->Create("vertex", "fragment", rp);
		}
		pPipeline = pipeline;

		Mesh::Create(rp, vertices, indices);
	}
	
	void TextureMesh::SetImage(Core::Image image, vk::Sampler sampler) {
		for (size_t i = 0; i < Core::Instance::GetInfo().MAX_FRAMES_IN_FLIGHT; i++) {
			Core::DescriptorPool::SetImage(descriptorSets[i], 1, image, sampler);
		}
	}

	void TextureMesh::Destroy() {
		Mesh::Destroy();
		if (--meshCounter == 0) {
			pipeline->Destroy();
			pipeline = nullptr;
		}
	}
}