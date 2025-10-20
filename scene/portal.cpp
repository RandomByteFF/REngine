#include "portal.hpp"
#include "common/math.hpp"
#include "core/camera.hpp"
#include "glm/geometric.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/matrix.hpp"
#include "node3d.hpp"
#include "scene/sceneTree.hpp"
#include <memory>
#include <vulkan/vulkan_enums.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace REngine::Scene {
	Portal::Portal() : camera(1.0) {
		portalMesh = std::make_shared<PortalMesh>();
	}

	void Portal::EnteredTree() {
		Node3D::EnteredTree();
		std::string p = "Player";
		player = std::dynamic_pointer_cast<Player>(SceneTree::Current()->Find(p, *SceneTree::Current()->GetRoot()));

		wasAhead = glm::dot(Forward(), glm::normalize(player->GlobalPosition() - GlobalPosition())) > 0;

		AddChild(portalMesh);
	}

	void Portal::Create(vk::RenderPass rp) {
		portalMesh->Create(rp);
		portalMesh->SetRenderCam(&pair->camera);
	}

	void Portal::SetSampler(vk::Sampler sampler) {
		portalMesh->SetSampler(sampler);
	}


	void Portal::SetPair(std::shared_ptr<Portal> portal) {
		pair = portal;
	}

	void Portal::UpdateCamera() {
		std::shared_ptr<Core::Camera> mainCamera = SceneTree::Current()->ActiveCamera();
		glm::vec3 scale;
		glm::quat rotation;
		glm::vec3 translation;
		glm::vec3 skew;
		glm::vec4 perspective;
		
		// set dynamic scale
		// bool camFacingPortal = glm::dot(-mainCamera->Forward(), GlobalPosition() - mainCamera->GlobalPosition()) > 0;
		portalMesh->Scale(glm::vec3(portalMesh->Scale().x, portalMesh->Scale().y, 0.1));

		// set own camera position
		glm::mat4 c = glm::inverse(mainCamera->V());
		glm::mat4 m = modelNoScale * inverse(pair->modelNoScale) * c;
		glm::decompose(m, scale, rotation, translation, skew, perspective);
		camera.Position(translation);
		camera.Rotation(glm::eulerAngles(rotation));

		// set oblique near clipping plane
		float dot = Math::sgn(glm::dot(Forward(), GlobalPosition() - camera.GlobalPosition()));

		glm::vec3 camSpaceNormal = glm::mat3(camera.V()) * (Forward() * dot);
		glm::vec3 camSpacePos = camera.V() * glm::vec4(GlobalPosition(), 1.);
		float camSpaceDst = -glm::dot(camSpacePos, camSpaceNormal);

		glm::vec4 clipPlaneCameraSpace = glm::vec4(camSpaceNormal, camSpaceDst);

		if (std::abs(camSpaceDst) < 0.1) camera.ResetProjection();
		else camera.ObliqueMatrix(clipPlaneCameraSpace);

		//TODO: fix camera quat rotation
	}

	void Portal::Update() {
		Node3D::Update();
		bool ahead = glm::dot(Forward(), glm::normalize(player->GlobalPosition() - GlobalPosition())) > 0;
		portalMesh->Position(glm::vec3(0., 0., ahead ? -0.1 : 0.1));

		if (ahead != wasAhead && wasClose) {
			glm::vec3 scale;
			glm::quat rotation;
			glm::vec3 translation;
			glm::vec3 skew;
			glm::vec4 perspective;

			glm::mat4 m = pair->modelNoScale * glm::inverse(modelNoScale) * player->GetModel();
			glm::decompose(m, scale, rotation, translation, skew, perspective);
			player->Position(translation);
			player->RotationQuat(rotation);
		}

		glm::vec3 d = player->GlobalPosition() - GlobalPosition();
		wasClose = glm::length(glm::vec2(d.x, d.z)) < 1.0;

		wasAhead = ahead;

		UpdateCamera();
	}

	void Portal::ApplyTransforms() {
		Node3D::ApplyTransforms();
		glm::mat4 pModel = GetParent() ? GetParent()->GetModel() : glm::mat4(1.f);
		modelNoScale = pModel * glm::translate(glm::mat4(1.f), Position()) * glm::toMat4(RotationQuat());
	};
}