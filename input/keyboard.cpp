#include "keyboard.hpp"
#include <cassert>

namespace REngine::Input {
	void Keyboard::KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
		if (key >= 0) {
			switch(action) {
				case GLFW_PRESS:
					keys[key] = true;
					keysThisFrame[key] = true;
					break;
				case GLFW_RELEASE:
					keys[key] = false;
					break;
			}
		}
	}

	void Keyboard::Initialize(GLFWwindow *window) {
		glfwSetKeyCallback(window, Keyboard::KeyCallback);
	}

	void Keyboard::EndFrame() {
		keysThisFrame.fill(false);
	}

	bool Keyboard::IsDown(int glfw_key) {
		assert(glfw_key >= 0 && glfw_key < keys.size());
		return keys[glfw_key];
	}
	bool Keyboard::IsJustPressed(int glfw_key) {
		assert(glfw_key >= 0 && glfw_key < keysThisFrame.size());
		return keysThisFrame[glfw_key];
	}
}