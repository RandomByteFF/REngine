#include "widgets.hpp"

namespace ImGui {
	bool Vec3(const char* label, ImVec4 &val, float speed) {
		PushItemWidth(GetWindowSize().x / 3 - 40);
		bool ret = false;
		AlignTextToFramePadding();
		TextColored(ImVec4(0.9f, 0.0f, 0.0f, 1.f), "X:");
		SameLine();
		PushID(0);
		ret |= DragFloat(label, &val.x, speed);
		PopID();
		SameLine();
		TextColored(ImVec4(0.f, 0.9f, 0.f, 1.f), "Y:");
		SameLine();
		PushID(1);
		ret |= DragFloat(label, &val.y, speed);
		PopID();
		SameLine();
		TextColored(ImVec4(0.5f, 0.5f, 1.f, 1.f), "Z:");
		SameLine();
		PushID(2);
		ret |= DragFloat(label, &val.z, speed);
		PopID();
		return ret;
	}
}