#pragma once
#include "scene/drawable.hpp"
#include "core/pipeline.hpp"
#include "core/buffer.hpp"

namespace REngine::Editor {
	struct VP {
		glm::mat4 V;
		glm::mat4 P;
	};

	class Grid : public Scene::Drawable {
		Core::Pipeline pipeline;	
		std::vector<vk::DescriptorSet> descriptorSets;
		std::vector<Core::Buffer> uniformBuffers;
		VP vp;
	public:
		void Create(Core::Swapchain swapchain, vk::RenderPass renderPass);
		void Bind(vk::CommandBuffer cb);
		void Update() override;
		void Draw(vk::CommandBuffer cb) override;
		void Destroy() override;
	};
}