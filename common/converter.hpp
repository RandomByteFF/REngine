#pragma once
#include "imgui.h"
#include <glm/glm.hpp>

namespace REngine::Common {
	void Convert(ImVec4 &dst, const glm::vec3 &src) {
		std::memcpy(&dst.x, &src.x, sizeof(src));
	}

	glm::vec3 Convert(const ImVec4 &src) {
		glm::vec3 ret;
		std::memcpy(&ret.x, &src.x, sizeof(ret));
		return ret;
	};
}