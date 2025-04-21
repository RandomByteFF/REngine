#pragma once
#include "core/drawable/drawable.hpp"
#include "core/pipeline.hpp"
#include "core/buffer.hpp"

namespace REngine::Editor {
	struct VP {
		glm::mat4 V;
		glm::mat4 P;
	};

	class Grid : public Core::Drawable {
		Core::Pipeline pipeline;	
		std::vector<vk::DescriptorSet> descriptorSets;
		std::vector<Core::Buffer> uniformBuffers;
		VP vp;
	public:
		void Create(Core::Swapchain swapchain, vk::RenderPass renderPass);
		void Bind(vk::CommandBuffer cb) override;
		void Update(Core::Camera &camera) override;
		void Draw(vk::CommandBuffer cb) override;
		void Destroy() override;
	};
}