#pragma once

#include <string>
#include <vector>
#include "core/vertex.hpp"

namespace REngine::Loader {
	class Obj {
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
	public:
		void Load(const std::string &name);
		std::vector<Vertex> &Verticies();
		std::vector<uint32_t> &Indices();
		void Destroy();
	};
}