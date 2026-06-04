#include "PhysicsComponent.h"

PhysicsComponent::PhysicsComponent(Object* parent) : Component(parent) {
	Name = "Physics Component";
	this->inverseMass = 1;
	this->acceleration = glm::vec3(0);
	this->velocity = glm::vec3(0);
	this->netForce = glm::vec3(0);
}

void PhysicsComponent::ProcessInspectorUI() {
	if (!this->parent->GetComponent<TransformComponent>()->Enabled) {
		SetEnabled(false);
	}

	ImGui::Text("Mass ");
	ImGui::SameLine();
	float mass = 1 / inverseMass;
	if (ImGui::InputFloat("##Mass", &mass, 0.0f, 0.0f, "%.3f kg")) {
		if (mass <= 0) {
			std::cout << "Mass can't be 0 or lower" << std::endl;
		}
		else {
			inverseMass = 1 / mass;
		}
	}

	ImGui::Text("Velocity ");
	ImGui::SameLine();
	float velocity[] = { this->velocity.x, this->velocity.y };
	if (ImGui::InputFloat2("##Velocity", velocity, "%.3f m/s")) {
		this->velocity = glm::vec3(velocity[0], velocity[1], 0);
	}

	ImGui::Text("Acceleration ");
	ImGui::SameLine();
	float accel[] = { this->accelDisplay.x, this->accelDisplay.y };
	if (ImGui::InputFloat2("##Acceleration", accel, "%.3f m/s²")) {
		this->acceleration = glm::vec3(accel[0], accel[1], 0);
	}

	ImGui::Text("Net Force ");
	ImGui::SameLine();
	float force[] = { this->netForceDisplay.x, this->netForceDisplay.y };
	if (ImGui::InputFloat2("##Net Force", force, "%.3f N")) {
		this->netForce = glm::vec3(force[0], force[1], 0);
	}

	ImGuiTreeNodeFlags root_flags = ImGuiTreeNodeFlags_OpenOnArrow |
		ImGuiTreeNodeFlags_OpenOnDoubleClick |
		ImGuiTreeNodeFlags_SpanAvailWidth |
		ImGuiTreeNodeFlags_DefaultOpen;

	if (ImGui::TreeNodeEx("Forces", root_flags)) {
		for (int i = 0; i < forceDisplayFunc.size(); i++)
		{
			(*forceDisplayFunc[i])(i);
		}

		ImGui::TreePop();
	}
}

void PhysicsComponent::ProcessPhysics(float delta) {
	if (!Enabled) {
		return;
	}

	TransformComponent* transform = this->parent->GetComponent<TransformComponent>();
	glm::vec3 position = transform->GetWorldPosition();

	position += velocity * delta;

	glm::vec3 resultingAcc = acceleration;
	resultingAcc += netForce * inverseMass;
	velocity += resultingAcc * delta;
	
	transform->UpdateWorldPosition(position);

	accelDisplay = glm::vec2(resultingAcc.x, resultingAcc.y);
	netForceDisplay = glm::vec2(netForce.x, netForce.y);

	ClearNetForce();
}

void PhysicsComponent::ClearNetForce() {
	netForce = glm::vec3(0);
}

void PhysicsComponent::AddForce(glm::vec3 force) {
	netForce += force;
}

void PhysicsComponent::AddDisplayFunc(std::shared_ptr<std::function<void(int)>> func) {
	forceDisplayFunc.push_back(func);
}

void PhysicsComponent::RemoveDisplayFunc(std::shared_ptr<std::function<void(int)>> func) {
	for (int i = 0; i < forceDisplayFunc.size(); i++)
	{
		if (forceDisplayFunc[i] == func) {
			forceDisplayFunc.erase(forceDisplayFunc.begin() + i);
		}
	}
}