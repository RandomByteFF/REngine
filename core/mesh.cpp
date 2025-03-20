#include "mesh.hpp"
#include "descriptorPool.hpp"
#include "instance.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace REngine::Core {
	void Mesh::Create(Pipeline pipeline, std::vector<Vertex> &vertices, std::vector<uint32_t> &indices) {
		this->pipeline = pipeline;
		indicesSize = uint32_t(indices.size());
		mvp.model = glm::mat4(1.0f);
		descriptorSets = DescriptorPool::CreateDescriptor(pipeline.GetLayout(), Instance::GetInfo().MAX_FRAMES_IN_FLIGHT);
		
		VkDeviceSize bufferSize = sizeof(MVP);
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
		cb.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline.layout, 0, descriptorSets[Instance::GetInfo().currentFrame], nullptr);

		vk::DeviceSize offset[] = {0};
		cb.bindVertexBuffers(0, vertexBuffer.buffer, offset);
		cb.bindIndexBuffer(indexBuffer.buffer, 0, vk::IndexType::eUint32);
		cb.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.pipeline);
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
		mvp.model = glm::rotate(mvp.model, angle, pivot);
		mvp.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		mvp.proj = glm::perspective(glm::radians(45.0f), float(800 / 600), 0.1f, 10.0f);
		mvp.proj[1][1] *= -1;
		changed = true;
	}

	void Mesh::Update() {
		if (changed) {
			uniformBuffers[Instance::GetInfo().currentFrame].CopyData(&mvp, sizeof(MVP));
			changed = false;
		}
	}
	void Mesh::Destroy() {
		for (auto i : uniformBuffers) {
			i.Destroy();
		}
		indexBuffer.Destroy();
		vertexBuffer.Destroy();
	}
}
