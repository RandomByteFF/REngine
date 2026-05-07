#pragma once

#include "scene/textureMesh.hpp"

namespace REngine::Scene {
	class Player : public TextureMesh {
		bool locked = false;
		float cameraR = 0.f;
		float gravityV = 0.f;
	public:
		Player(Core::RenderPass rp);
		virtual void Update() override;
	};
}