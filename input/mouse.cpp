#include "mouse.hpp"
#include "GLFW/glfw3.h"
#include <cassert>
#ifdef EDITOR
	// I kinda have to put this here, referencing to renderer or editor would be really weird
	#include <imgui.h>
#endif

namespace REngine::Input {
	void Mouse::ButtonCallback(GLFWwindow *window, int button, int action, int mods) {
		#ifdef EDITOR
			ImGuiIO &io = ImGui::GetIO();
			io.AddMouseButtonEvent(button, action == GLFW_PRESS);
			// if (io.WantCaptureMouse) return;
			// This messes up the controls inside the editor
		#endif
		if (button >= 0) {
			switch(action) {
				case GLFW_PRESS:
					keys[button] = true;
					break;
				case GLFW_RELEASE:
					keys[button] = false;
					break;
			}
		}
	}

	void Mouse::ScrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
		scroll += yoffset;
	}

	void Mouse::Initialize(GLFWwindow *window) {
		windowHandle = window;
		glfwSetMouseButtonCallback(window, Mouse::ButtonCallback);
		glfwSetScrollCallback(window, Mouse::ScrollCallback);
		if (glfwRawMouseMotionSupported()) {
			glfwSetInputMode(windowHandle, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
		}
	}

	bool Mouse::IsDown(int glfw_button) {
		assert(glfw_button >= 0 && glfw_button < keys.size());
		return keys[glfw_button];
	}

	glm::vec2 Mouse::CursorPos()
	{
		double x, y;
		glfwGetCursorPos(windowHandle, &x, &y);
		return glm::vec2(float(x), float(y));
	}

	void Mouse::RecordDelta() {
		if (firstRun) {
			delta = glm::vec2(0.f, 0.f);
			firstRun = false;
			previousPos = CursorPos();
			return;
		}
		glm::vec2 p = CursorPos();
		delta = p - previousPos;
		previousPos = p;
	}

	glm::vec2 Mouse::Delta()
	{
		return delta;
	}

	void Mouse::Lock() {
		glfwSetInputMode(windowHandle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	void Mouse::Unlock() {
		glfwSetInputMode(windowHandle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	float Mouse::Scroll() {
		return scroll;
	}

	void Mouse::EndFrame() {
		scroll = 0.f;
	}
}