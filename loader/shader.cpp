#include "shader.hpp"

#include <filesystem>
#include <fstream>
#include <format>
#include <ostream>
#include <sstream>
#include <iostream>

#include "core/instance.hpp"
#include "core/pipeline.hpp"
#include "vulkan/vulkan_core.h"

namespace REngine::Loader {
	const vk::ShaderModule Shader::Get(const std::string &name, vk::ShaderStageFlagBits shaderType, std::weak_ptr<REngine::Core::Pipeline> pipeline) {
		// FIXME: do something with these relative paths
		std::string path = std::format("shaders/{}", name);
		auto r = store.find(path);
		
		if (r != store.end()) {
			return (*r).second;
		}
		std::ifstream file(path);
		if (!file.is_open()) {
			throw std::runtime_error("Failed to open file!");
		}
		std::stringstream ss;
		ss << file.rdbuf();
		file.close();
		
		shaderc::Compiler c;
		auto compiled = c.CompileGlslToSpv(ss.str(), map[shaderType], path.c_str());
		if (compiled.GetCompilationStatus() != shaderc_compilation_status_success) {
			throw std::runtime_error(std::format("Shader compilation failed: {}", path));
		}

		
		vk::ShaderModuleCreateInfo createInfo{};
		createInfo.codeSize = (compiled.end() - compiled.begin()) * sizeof(unsigned int);
		createInfo.pCode = compiled.begin();

		vk::ShaderModule shaderModule;
		auto module = Core::Instance::GetInfo().device.createShaderModule(createInfo);
		
		store[path] = module;
		changeTime[path] = std::filesystem::last_write_time(path);
		SubscribeToHotReload(path, pipeline);

		return module;
	}

	void Shader::WatchCodeChange() {
		for (auto i : changeTime) {
			auto write = std::filesystem::last_write_time(i.first);
			if (write != i.second) {
				std::cout << "Code change happened!" << std::endl;
				vkDeviceWaitIdle(Core::Instance::GetInfo().device);

				Core::Instance::GetInfo().device.destroyShaderModule(store[i.first]);
				store.erase(i.first);
				auto& set = subscribedPipelines[i.first];
				for (auto it = set.begin(); it != set.end();) {
					if (auto p = it->lock()) {
						p->Rebuild();
						++it;
					} else {
						it = set.erase(it);
					}
				}

				changeTime[i.first] = write;
			}

		}
	}

	void Shader::Destroy() {
		for (auto i : store) {
			Core::Instance::GetInfo().device.destroyShaderModule(i.second);
		}
	}

	void Shader::SubscribeToHotReload(std::string path, std::weak_ptr<Core::Pipeline> pipeline) {
 	   subscribedPipelines[path].insert(std::move(pipeline));
	}
}