#include "Gravity.h"

Gravity::Gravity(float gravity) {
	this->gravity = gravity;
}

void Gravity::updateForce(Object* object, float delta) {
	PhysicsComponent* phys = object->GetComponent<PhysicsComponent>();
	if (phys->inverseMass != 0) {
		glm::vec3 grav = glm::vec3(0, gravity, 0);
		Force = grav * (1 / phys->inverseMass);
		phys->AddForce(Force);
	}
}

void Gravity::processDisplay() {
	if (ImGui::TreeNode("Gravity")) {
		ImGui::Text("g ");
		ImGui::SameLine();
		ImGui::InputFloat("## Gravity accel", &gravity, 0.0f, 0.0f, "%.3f m/s²");

		ImGui::Text("Gravity ");
		ImGui::SameLine();
		float force[] = { Force.x, Force.y };
		ImGui::InputFloat2("## Gravity", force, "%.3f N", ImGuiInputTextFlags_ReadOnly);

		ImGui::TreePop();
	}
}