#include "player.hpp"
#include "core/time.hpp"
#include "input/mouse.hpp"
#include "scene/sceneTree.hpp"
#include "input/keyboard.hpp"
#include <numbers>

namespace {
	constexpr float SPEED = 10.;
	constexpr float SENSITIVITY = 0.01;
}

namespace REngine::Scene {
	Player::Player(vk::RenderPass rp) : TextureMesh(rp, "test_files/player.obj", "test_files/player.png") {
		name = "Player";
		Scale(glm::vec3(0.2, 0.2, 0.2));
		Rotation(glm::vec3(0., std::numbers::pi, 0.));
		Position(glm::vec3(0., 1., 6.));
	}

	void Player::Update() {
		if (Input::Keyboard::IsDown(GLFW_KEY_1)) {
			Input::Mouse::Lock();
			locked = true;
		}
		if (Input::Keyboard::IsDown(GLFW_KEY_ESCAPE)) {
			Input::Mouse::Unlock();
			locked = false;
		}
		if (Input::Keyboard::IsDown(GLFW_KEY_W)) {
			Position(Position() + glm::vec3(GetModel()[2]) * Core::Time::Delta() * SPEED);
		}
		if (locked) {
			Rotate(glm::vec3(0.f, -Input::Mouse::Delta().x * SENSITIVITY, 0.f));
		}
	}
}