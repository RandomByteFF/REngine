#pragma once
#include "imgui.h"
namespace REngine::Scene {
	class Node;
	class Node3D;
	class Mesh;
}

namespace REngine::Editor {
	class InspectorVisitor {
	public:
		void Visit(Scene::Node *node);
		void Visit(Scene::Node3D *node);
		void Visit(Scene::Mesh *node);
	};
}