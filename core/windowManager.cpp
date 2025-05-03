#include "windowManager.hpp"
#include "instance.hpp"
#include "imgui_impl_glfw.h"
#include "input/keyboard.hpp"
#include "input/mouse.hpp"

namespace REngine::Core {
	
	void WindowManager::CreateWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		handle = glfwCreateWindow(WIDTH, HEIGHT, "REngine", nullptr, nullptr);
		glfwMaximizeWindow(handle);
		glfwSetWindowUserPointer(handle, this);
		glfwSetFramebufferSizeCallback(handle, FramebufferResizeCallback);
		ImGui_ImplGlfw_InitForVulkan(handle, true);
		Input::Keyboard::Initialize(handle);
		Input::Mouse::Initialize(handle);
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
		#ifndef EDITOR
		WindowManager *manager = reinterpret_cast<WindowManager*>(glfwGetWindowUserPointer(window));
		manager->framebufferResized = true;
		Instance::FrameBufferResized(width, height);
		#endif
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