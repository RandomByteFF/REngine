#pragma once

namespace REngine::Scene {
	class Node;
	class Node3D;
	class Mesh;
}

namespace REngine::Common {
	class NodeVisitor {
	public:
		virtual void Visit(Scene::Node *node) {}
		virtual void Visit(Scene::Node3D *node) {}
		virtual void Visit(Scene::Mesh *node) {}
	};
}

#define VISIT() \
void Visit(Scene::Node *node) override; \
void Visit(Scene::Node3D *node) override; \
void Visit(Scene::Mesh *node) override; 
