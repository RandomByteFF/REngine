#define GLFW_INCLUDE_VULKAN
#define VMA_IMPLEMENTATION
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define STB_IMAGE_IMPLEMENTATION
#include "core/runner.hpp"

int main() {
	REngine::Core::Runner runner;
	return runner.Run();
}