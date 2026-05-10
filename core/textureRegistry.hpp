#pragma once

#include "commandBuffer.hpp"
#include "core/pipeline.hpp"
#include "vulkan/vulkan_handles.hpp"

namespace REngine::Core {
	class TextureRegistry {
	public:
		static constexpr uint32_t MAX_TEXTURES = 4096;
	
		void Create();
		void Bind(CommandBuffer cmd);
		void Destroy();
		uint32_t Register(vk::ImageView view, vk::Sampler sampler);
		void Free(uint32_t index);

		vk::DescriptorSet GetSet() const {return set;}
		vk::DescriptorSetLayout GetLayout() const {return layout;}

		static TextureRegistry &Instance();

	private:
		inline static TextureRegistry *instance = nullptr;
		vk::DescriptorPool pool;
		vk::DescriptorSetLayout layout;
		vk::DescriptorSet set;
		vk::PipelineLayout pipelineLayout;

		uint32_t nextSlot = 0;
		std::vector<uint32_t> freeSlots;
	};
}