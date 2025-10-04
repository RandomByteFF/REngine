#include "mesh.hpp"
#include "core/descriptorPool.hpp"
#include "core/instance.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <vulkan/vulkan_enums.hpp>
#include "core/pipeline.hpp"
#include "sceneTree.hpp"
#include "vulkan/vulkan_handles.hpp"

using namespace REngine::Core;

namespace REngine::Scene {
	void Mesh::Create(vk::RenderPass rp, std::vector<Vertex> &vertices, std::vector<uint32_t> &indices) {
		Drawable::Initialize(sceneTree);

		indicesSize = uint32_t(indices.size());
		descriptorSets = DescriptorPool::CreateDescriptor(pPipeline.lock()->GetLayout(), Instance::GetInfo().MAX_FRAMES_IN_FLIGHT);
		
		VkDeviceSize bufferSize = sizeof(mvp);
		uniformBuffers.resize(Instance::GetInfo().MAX_FRAMES_IN_FLIGHT);
		
		vertexBuffer.Create(sizeof(vertices[0]) * vertices.size(), vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer);
		vertexBuffer.Stage(vertices.data());

		indexBuffer.Create(sizeof(indices[0]) * indices.size(), vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer);
		indexBuffer.Stage(indices.data());
	}
	void Mesh::Bind(vk::CommandBuffer cb, Core::Camera &camera) {
		mvp = camera.VP() * GetModel();
		cb.pushConstants(pPipeline.lock()->GetPipelineLayout(), vk::ShaderStageFlagBits::eVertex, 0, sizeof(mvp), &mvp);
		// uniformBuffers[Instance::GetInfo().currentFrame].CopyData(&mvp, sizeof(mvp));
		
		cb.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pPipeline.lock()->GetPipelineLayout(), 0, descriptorSets[Instance::GetInfo().currentFrame], nullptr);

		vk::DeviceSize offset[] = {0};
		cb.bindVertexBuffers(0, vertexBuffer.GetBuffer(), offset);
		cb.bindIndexBuffer(indexBuffer.GetBuffer(), 0, vk::IndexType::eUint32);
		cb.bindPipeline(vk::PipelineBindPoint::eGraphics, pPipeline.lock()->GetPipeline());
	}

	void Mesh::Draw(vk::CommandBuffer cb) {
		Bind(cb, *SceneTree::Current()->ActiveCamera());
		cb.drawIndexed(indicesSize, 1, 0, 0, 0);
	}
	
	void Mesh::DrawFromView(vk::CommandBuffer cb, Core::Camera &camera) {
		Bind(cb, camera);
		cb.drawIndexed(indicesSize, 1, 0, 0, 0);
	}


	void Mesh::Destroy() {
		Node3D::Destroy();
		Drawable::Destroy();
		for (auto i : uniformBuffers) {
			i.Destroy();
		}
		indexBuffer.Destroy();
		vertexBuffer.Destroy();
	}
}
