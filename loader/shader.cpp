#include "shader.hpp"

#include <fstream>
#include <format>

#include "core/instance.hpp"

namespace REngine::Loader {
	const vk::ShaderModule Shader::Get(const std::string &name) {
		auto r = store.find(name);
		if (r == store.end()) {
			// FIXME: do something with these relative paths
			std::ifstream file(std::format("shaders/{}.spv", name), std::ios::ate | std::ios::binary);
			if (!file.is_open()) {
				throw std::runtime_error("Failed to open file!");
			}
			size_t fileSize = size_t(file.tellg());
			std::vector<char> buffer(fileSize);
			file.seekg(0);
			file.read(buffer.data(), fileSize);
			file.close();

			
			vk::ShaderModuleCreateInfo createInfo{};
			createInfo.codeSize = fileSize;
			createInfo.pCode = reinterpret_cast<const uint32_t*>(buffer.data());

			vk::ShaderModule shaderModule;
			auto module = Core::Instance::GetInfo().device.createShaderModule(createInfo);
			
			store[name] = module;
			return module;
		}
		else return (*r).second;
	}

	void Shader::Cleanup() {
		for (auto i : store) {
			Core::Instance::GetInfo().device.destroyShaderModule(i.second);
		}
	}
}