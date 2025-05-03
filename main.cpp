#define GLFW_INCLUDE_VULKAN
#define VMA_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "core/runner.hpp"

int main() {
	REngine::Core::Runner runner;
	return runner.Run();
}