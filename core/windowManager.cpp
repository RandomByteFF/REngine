#include "windowManager.hpp"

namespace REngine::Core {
	
	void WindowManager::CreateWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		handle = glfwCreateWindow(WIDTH, HEIGHT, "REngine", nullptr, nullptr);
		glfwSetWindowUserPointer(handle, this);
		glfwSetFramebufferSizeCallback(handle, FramebufferResizeCallback);
	}

	void WindowManager::GetFrameBufferSize(int &width, int &height) {
		glfwGetFramebufferSize(handle, &width, &height);
	}

	void WindowManager::CreateSurface(vk::Instance instance, vk::SurfaceKHR &surface) {
		VkSurfaceKHR s;
		if (glfwCreateWindowSurface(instance, handle, nullptr, &s) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create window surface!");
		}
		surface = s;
	}

	void WindowManager::FramebufferResizeCallback(GLFWwindow *window, int width, int height) {
		WindowManager *manager = reinterpret_cast<WindowManager*>(glfwGetWindowUserPointer(window));
		manager->framebufferResized = true;
	}

	bool WindowManager::IsDirty(bool reset) {
		bool isDirty = framebufferResized;
		if (reset) isDirty = false;
		return isDirty;
	}

	bool WindowManager::Update() {
		if (glfwWindowShouldClose(handle)) {
			return false;
		}
		glfwPollEvents();
		return true;
	}

	void WindowManager::Destroy() {
		glfwDestroyWindow(handle);
		glfwTerminate();
	}
}