#include "inspector.hpp"
#include "imgui.h"


namespace REngine::Editor {
	void Inspector::Gui(std::optional<std::shared_ptr<Scene::Node>> target) {
		ImGui::Begin("Inspector");
		if (target.has_value()) {
			(*target)->AcceptGui(&visitor);
		}
		ImGui::End();
	}
}