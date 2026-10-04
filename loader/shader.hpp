#pragma once

#include "vulkan/vulkan_enums.hpp"
#include <filesystem>
#include <shaderc/shaderc.hpp>

#include <string>
#include <unordered_map>

#include <set>

namespace REngine::Core {
	class Pipeline;
}

using PipelineSet = std::set<std::weak_ptr<REngine::Core::Pipeline>, std::owner_less<std::weak_ptr<REngine::Core::Pipeline>>>;
namespace REngine::Loader {
	class Shader {
		inline static std::unordered_map<std::string, vk::ShaderModule> store;
		inline static std::unordered_map<std::string, std::filesystem::file_time_type> changeTime;
		inline static std::unordered_map<std::string, PipelineSet> subscribedPipelines;
		inline static std::unordered_map<vk::ShaderStageFlagBits, shaderc_shader_kind> map = {
			{vk::ShaderStageFlagBits::eVertex, shaderc_shader_kind::shaderc_glsl_vertex_shader},
			{vk::ShaderStageFlagBits::eFragment, shaderc_shader_kind::shaderc_glsl_fragment_shader}
		};
		
	public:
		static const vk::ShaderModule Get(const std::string &name, vk::ShaderStageFlagBits shaderType, std::weak_ptr<REngine::Core::Pipeline> pipeline);
		static void WatchCodeChange();
		static void SubscribeToHotReload(std::string path, std::weak_ptr<Core::Pipeline> pipeline);
		static void Destroy();
	};
}