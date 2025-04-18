#pragma once

#include <GLFW/glfw3.h>
#include <array>
#include <glm/vec2.hpp>

namespace REngine::Input {
	class Mouse {
		inline static GLFWwindow *windowHandle;
		inline static std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> keys;
		inline static glm::vec2 previousPos;
		inline static glm::vec2 delta;
		inline static bool firstRun = true;

		static void ButtonCallback(GLFWwindow *window, int button, int action, int mods);

	public:
		static void Initialize(GLFWwindow *window);
		static bool IsDown(int glfw_button);
		static glm::vec2 CursorPos();
		static void RecordDelta();
		static glm::vec2 Delta();
		static void Lock();
		static void Unlock();
	};
}