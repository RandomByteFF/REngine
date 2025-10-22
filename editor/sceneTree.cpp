#include "sceneTree.hpp"
#include "imgui.h"
#include "scene/portal.hpp"
#include "scene/sceneTree.hpp"
#include <memory>
#include <typeinfo>

namespace {
	std::array<std::string, 1> nodes = {"Portal"};
}

namespace REngine::Editor {
	auto leafFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
	auto defaultFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
	
	void SceneTree::TraverseNode(const std::shared_ptr<Scene::Node> node, int &id) {
		std::string name = node->name;
		if (name.empty()) {
			name = typeid(*node).name();
		}
		std::string toDisplay = (name + "##" + std::to_string(id)).c_str();
		bool s = selected.has_value() && selected.value() == node;
		ImGuiTreeNodeFlags sFlags = s ? ImGuiTreeNodeFlags_Selected : 0;

		bool open = false;
		open = ImGui::TreeNodeEx(toDisplay.c_str(), defaultFlags | sFlags | (node->Children().size() == 0 ? leafFlags : 0));
		if (ImGui::BeginDragDropSource()) {
			ImGui::SetDragDropPayload("NODE", &node->id, sizeof(node->id));
			ImGui::Text(node->name.c_str());
			ImGui::EndDragDropSource();
		}
		
		if (ImGui::IsItemClicked()) selected = node;
		if (open && node->Children().size() > 0) {
			for (auto i : node->Children()) TraverseNode(i, ++id);
			ImGui::TreePop();
		}
	}

	void SceneTree::Gui() {
		auto root = Scene::SceneTree::Current()->GetRoot();
		ImGui::Begin("Scene tree");
		int id = -1;
		TraverseNode(root, id);
		GenericPopup();
		ImGui::End();
	}

	std::optional<std::shared_ptr<Scene::Node>> SceneTree::GetSelected() {
		return selected;
	}

	void SceneTree::GenericPopup() {
		int selected = -1;
		if (ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
			ImGui::OpenPopup("sceneTreeGeneric");
		}
		if (ImGui::BeginPopup("sceneTreeGeneric")) {
			if (ImGui::BeginMenu("Add node")) {
				for (size_t i = 0; i < nodes.size(); i++) {
					if (ImGui::MenuItem(nodes[i].c_str())) {
						selected = i;
					}
				}
				ImGui::EndMenu();
			}
			ImGui::EndPopup();
		}

		std::shared_ptr<Scene::Node> newNode;
		if (selected == 0) {
			newNode = std::dynamic_pointer_cast<Scene::Node>(std::make_shared<Scene::Portal>());
		}

		if (newNode) {
			Scene::SceneTree::Current()->GetRoot()->AddChild(newNode);
		}
	}
}