#pragma once

#include "scene/textureMesh.hpp"

namespace REngine::Scene {
	class Player : public TextureMesh {
		bool locked = false;
	public:
		Player(vk::RenderPass rp);
		virtual void Update() override;
	};
}