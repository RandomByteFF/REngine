#include "sceneTree.hpp"
#include "imgui.h"
#include "scene/sceneTree.hpp"
#include <typeinfo>

namespace REngine::Editor {
	auto leafFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
	auto defaultFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
	
	void SceneTree::TraverseNode(const std::shared_ptr<Scene::Node> node, int &id) {
		std::string name = typeid(*node).name();
		std::string toDisplay = (name + "##" + std::to_string(id)).c_str();
		bool s = selected.has_value() && selected.value() == node;
		ImGuiTreeNodeFlags sFlags = s ? ImGuiTreeNodeFlags_Selected : 0;
		if (node->Children().size() == 0) {
			ImGui::TreeNodeEx(toDisplay.c_str(), leafFlags | defaultFlags | sFlags);
			if (ImGui::IsItemClicked()) selected = node;
			return;
		}
		bool open = ImGui::TreeNodeEx(toDisplay.c_str(), defaultFlags | sFlags);
		if (ImGui::IsItemClicked()) selected = node;
		if (open) {
			for (auto i : node->Children()) TraverseNode(i, ++id);
			ImGui::TreePop();
		}
	}

	void SceneTree::Gui() {
		auto root = Scene::SceneTree::Current()->GetRoot();
		ImGui::Begin("Scene tree");
		int id = -1;
		TraverseNode(root, id);
		ImGui::End();
	}
}