#pragma once

#include "headers.h"
#include <GLFW/glfw3.h>

const int WIDTH = 800;
const int HEIGHT = 600;

namespace REngine::Core {
	class WindowManager {
		bool framebufferResized = false;
		GLFWwindow *handle;

		static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
		inline static WindowManager *instance;
	public:
		void CreateWindow();
		void GetFrameBufferSize(int &width, int &height);
		void CreateSurface(vk::Instance instance, vk::SurfaceKHR &surface);
		bool IsDirty(bool reset = false);
		bool Update();
		void Destroy();
		static WindowManager *Instance();
	};
}