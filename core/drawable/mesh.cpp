#include "mesh.hpp"
#include "core/descriptorPool.hpp"
#include "core/instance.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace REngine::Core {
	void Mesh::Create(Pipeline pipeline, std::vector<Vertex> &vertices, std::vector<uint32_t> &indices) {
		this->pipeline = pipeline;
		indicesSize = uint32_t(indices.size());
		model = glm::mat4(1.0f);
		descriptorSets = DescriptorPool::CreateDescriptor(pipeline.GetLayout(), Instance::GetInfo().MAX_FRAMES_IN_FLIGHT);
		
		VkDeviceSize bufferSize = sizeof(mvp);
		uniformBuffers.resize(Instance::GetInfo().MAX_FRAMES_IN_FLIGHT);
		
		for (size_t i = 0; i < Instance::GetInfo().MAX_FRAMES_IN_FLIGHT; i++) {
			uniformBuffers[i].Create(bufferSize, vk::BufferUsageFlagBits::eUniformBuffer, true);
			DescriptorPool::SetUniform(descriptorSets[i], 0, uniformBuffers[i]);
		}

		vertexBuffer.Create(sizeof(vertices[0]) * vertices.size(), vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer);
		vertexBuffer.Stage(vertices.data());

		indexBuffer.Create(sizeof(indices[0]) * indices.size(), vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer);
		indexBuffer.Stage(indices.data());
	}
	void Mesh::Bind(vk::CommandBuffer cb) {
		cb.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline.GetPipelineLayout(), 0, descriptorSets[Instance::GetInfo().currentFrame], nullptr);

		vk::DeviceSize offset[] = {0};
		cb.bindVertexBuffers(0, vertexBuffer.GetBuffer(), offset);
		cb.bindIndexBuffer(indexBuffer.GetBuffer(), 0, vk::IndexType::eUint32);
		cb.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.GetPipeline());
	}

	void Mesh::Draw(vk::CommandBuffer cb) {
		cb.drawIndexed(indicesSize, 1, 0, 0, 0);	
	}

	void Mesh::SetImage(Image image, vk::Sampler sampler) {
		for (size_t i = 0; i < Instance::GetInfo().MAX_FRAMES_IN_FLIGHT; i++) {
			DescriptorPool::SetImage(descriptorSets[i], 1, image, sampler);
		}
	}

	void Mesh::Rotate(glm::f32 angle, glm::vec3 pivot) {
		model = glm::rotate(model, angle, pivot);
	}

	void Mesh::Update(Camera &camera) {
		// TODO: don't upload if camera and model hasn't changed
		mvp = camera.VP() * model;
		uniformBuffers[Instance::GetInfo().currentFrame].CopyData(&mvp, sizeof(mvp));
	}
	void Mesh::Destroy() {
		for (auto i : uniformBuffers) {
			i.Destroy();
		}
		indexBuffer.Destroy();
		vertexBuffer.Destroy();
	}
}
