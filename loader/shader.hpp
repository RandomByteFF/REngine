#pragma once

#include "vulkan/vulkan.hpp"

#include <string>
#include <unordered_map>

namespace REngine::Loader {
	class Shader {
		inline static std::unordered_map<std::string, vk::ShaderModule> store;
	public:
		static const vk::ShaderModule Get(const std::string &name);
		static void Destroy();
	};
}