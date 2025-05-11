#include "inspectorVisitor.hpp"
#include "imgui.h"
#include "widgets/widgets.hpp"
#include "scene/node.hpp"
#include "scene/node3d.hpp"
#include "common/converter.hpp"

namespace REngine::Editor {
	void InspectorVisitor::Visit(Scene::Node *node) {
		ImGui::SeparatorText("Node");
		ImGui::Text(node->name.c_str());
	}

	void InspectorVisitor::Visit(Scene::Node3D *node) {
		ImGui::SeparatorText("Node3D");
		ImGui::Text("Position");
		ImVec4 position;
		Common::Convert(position, node->Position());
		if (ImGui::Vec3("##Position", position, 0.1f)) {
			node->Position(Common::Convert(position));
		}
		ImVec4 scale;
		Common::Convert(scale, node->Scale());
		ImGui::Text("Scale");
		if (ImGui::Vec3("##Scale", scale, 0.1f)) {
			node->Scale(Common::Convert(scale));
		}
		ImVec4 rotation;
		Common::Convert(rotation, node->Rotation());
		ImGui::Text("Rotation");
		if (ImGui::Vec3("##Rotation", rotation, 0.1f)) {
			node->Rotation(Common::Convert(rotation));
		}
	}
	
	void InspectorVisitor::Visit(Scene::Mesh *node) {
		ImGui::SeparatorText("Mesh");
	}
}