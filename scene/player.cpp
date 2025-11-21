#include "player.hpp"
#include "GLFW/glfw3.h"
#include "core/camera.hpp"
#include "core/time.hpp"
#include "input/mouse.hpp"
#include "scene/sceneTree.hpp"
#include "input/keyboard.hpp"
#include <memory>
#include <numbers>

namespace {
	constexpr float SPEED = 10.;
	constexpr float SPRINTSPEED = 30.;
	constexpr float SENSITIVITY = 0.01;
	constexpr float yLimit = 0.6;
}

namespace REngine::Scene {
	Player::Player(vk::RenderPass rp) : TextureMesh(rp, "test_files/player.obj", "test_files/player.png") {
		name = "Player";
		Scale(glm::vec3(0.2, 0.2, 0.2));
		Rotation(glm::vec3(0., std::numbers::pi, 0.));
		Position(glm::vec3(0., 1., 6.));
	}

	void Player::Update() {
		if (Position().y - yLimit > 0.001) {
			float d = 10.0 * Core::Time::Delta() / 2.0;
			gravityV += d;
			Position(Position() + glm::vec3(0.f, -d, 0.f) * Core::Time::Delta());
			gravityV += d;
		}
		else if (Position().y < yLimit) {
			Position(glm::vec3(Position().x, yLimit, Position().z));
			gravityV = 0.f;
		}
		else {
			gravityV = 0.f;
		}

		float speed = Input::Keyboard::IsDown(GLFW_KEY_LEFT_SHIFT) ? SPRINTSPEED : SPEED;

		std::shared_ptr<Core::Camera> cam = std::dynamic_pointer_cast<Core::Camera>(Children()[0]);
		if (Input::Keyboard::IsDown(GLFW_KEY_1)) {
			Input::Mouse::Lock();
			locked = true;
		}
		if (Input::Keyboard::IsDown(GLFW_KEY_ESCAPE)) {
			Input::Mouse::Unlock();
			locked = false;
		}
		if (Input::Keyboard::IsDown(GLFW_KEY_W)) {
			Position(Position() + glm::vec3(Forward()) * Core::Time::Delta() * speed);
		}
		if (Input::Keyboard::IsDown(GLFW_KEY_S)) {
			Position(Position() + glm::vec3(-Forward()) * Core::Time::Delta() * speed);
		}
		if (Input::Keyboard::IsDown(GLFW_KEY_A)) {
			Position(Position() + glm::vec3(Right()) * Core::Time::Delta() * speed);
		}
		if (Input::Keyboard::IsDown(GLFW_KEY_D)) {
			Position(Position() + glm::vec3(-Right()) * Core::Time::Delta() * speed);
		}

		if (locked) {
			Rotate(glm::vec3(0.f, -Input::Mouse::Delta().x * SENSITIVITY, 0.f));
			cameraR -= Input::Mouse::Delta().y * SENSITIVITY;
			cameraR = std::clamp(cameraR, float(-std::numbers::pi / 2.), float(std::numbers::pi / 2.));
			cam->Rotation(glm::vec3(cameraR, std::numbers::pi, 0.f));
		}
	}
}