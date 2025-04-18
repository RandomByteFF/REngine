#pragma once
#include <GLFW/glfw3.h>
#include <array>

namespace REngine::Input {
	class Keyboard {
		inline static std::array<bool, GLFW_KEY_LAST + 1> keys;

		static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
	public:
		static void Initialize(GLFWwindow *window);
		static bool IsDown(int glfw_key);
	};
}