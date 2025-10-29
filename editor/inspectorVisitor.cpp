#include "inspectorVisitor.hpp"
#include "imgui.h"
#include "scene/portalMesh.hpp"
#include "scene/sceneTree.hpp"
#include "widgets/widgets.hpp"
#include "scene/node.hpp"
#include "scene/node3d.hpp"
#include "scene/portal.hpp"
#include "common/converter.hpp"
#include <memory>

namespace REngine::Editor {
	void InspectorVisitor::Visit(Scene::Node *node) {
		char name[30];
		// node->name.copy(name, sizeof(name) - 1);
		sprintf(name, "%.29s", node->name.c_str());
		ImGui::SeparatorText("Node");
		ImGui::InputText("##Name", name, sizeof(name));
		node->name = name;
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

	void InspectorVisitor::Visit(Scene::Portal *node) {
		ImGui::SeparatorText("Portal");
		std::string buttonText = (node->GetPair() ? node->GetPair()->name : "None") + "##" + std::to_string(node->id);
		ImGui::Button(buttonText.c_str());
		if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NODE")) {
				int64_t id = *(int64_t*)(payload->Data);
				std::shared_ptr<Scene::Node> n = Scene::SceneTree::Current()->Find(id, *Scene::SceneTree::Current()->GetRoot());
				// fixme: id's should everywhere be uint32_t
				std::shared_ptr<Scene::Portal> portal = std::dynamic_pointer_cast<Scene::Portal>(n);
				if (portal) {
					node->SetPair(portal);
				}
			}
		}
	}

	void InspectorVisitor::Visit(Scene::PortalMesh *node) {
		
	}
}