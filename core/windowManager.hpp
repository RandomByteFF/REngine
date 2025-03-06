#pragma once

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

const int WIDTH = 800;
const int HEIGHT = 600;

namespace REngine::Core {
	class WindowManager {
		GLFWwindow *handle;
		bool framebufferResized = false;

		static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
	public:
		void CreateWindow();
		void GetFrameBufferSize(int &width, int &height);
		void CreateSurface(vk::Instance instance, vk::SurfaceKHR &surface);
		bool IsDirty(bool reset = false);
		bool Update();
		void Destroy();
	};
}