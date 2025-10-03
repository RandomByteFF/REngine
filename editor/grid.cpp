#include "grid.hpp"
#include "core/instance.hpp"
#include "core/descriptorPool.hpp"
#include "scene/sceneTree.hpp"
#include <vulkan/vulkan_enums.hpp>

namespace REngine::Editor {
	void Grid::Create(Core::Swapchain swapchain, vk::RenderPass renderPass) {
		Scene::Drawable::Initialize(Scene::SceneTree::Current());
		pipeline.SetLayout({{vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex}});
		pipeline.SetSampleCount(Core::Instance::GetInfo().maxMsaa);
		pipeline.Create("gridVert", "gridFrag", renderPass);

		descriptorSets = Core::DescriptorPool::CreateDescriptor(pipeline.GetLayout(), Core::Instance::GetInfo().MAX_FRAMES_IN_FLIGHT);
		
		VkDeviceSize bufferSize = 2 * sizeof(glm::mat4);
		uniformBuffers.resize(Core::Instance::GetInfo().MAX_FRAMES_IN_FLIGHT);
		
		for (size_t i = 0; i < Core::Instance::GetInfo().MAX_FRAMES_IN_FLIGHT; i++) {
			uniformBuffers[i].Create(bufferSize, vk::BufferUsageFlagBits::eUniformBuffer, true);
			Core::DescriptorPool::SetUniform(descriptorSets[i], 0, uniformBuffers[i]);
		}
	}

	void Grid::Bind(vk::CommandBuffer cb) {
		auto camera = Scene::SceneTree::Current()->ActiveCamera();
		vp.V = camera->V();
		vp.P = camera->P();
		uniformBuffers[Core::Instance::GetInfo().currentFrame].CopyData(&vp, sizeof(vp));

		cb.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline.GetPipelineLayout(), 0, descriptorSets[Core::Instance::GetInfo().currentFrame], nullptr);
		cb.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.GetPipeline());
	}

	void Grid::Update() {
	}
	
	void Grid::Draw(vk::CommandBuffer cb) {
		Bind(cb);
		cb.draw(6, 1, 0, 0);
	}

	void Grid::Destroy() {
		Drawable::Destroy();
		Node3D::Destroy();
		for (auto i : uniformBuffers) {
			i.Destroy();
		}
		pipeline.Destroy();
	}
}