#include "Drag.h"

Drag::Drag(float k1, float k2) {
	this->k1 = k1;
	this->k2 = k2;
}

void Drag::updateForce(Object* object, float delta) {
	if (object->GetComponent<PhysicsComponent>()->velocity == glm::vec3(0)) {
		return;
	}
	glm::vec3 vel = glm::normalize(object->GetComponent<PhysicsComponent>()->velocity);
	Force = -vel * (k1 * vel.length() + k2 * std::powf(vel.length(), 2));

	object->GetComponent<PhysicsComponent>()->AddForce(Force);
}

void Drag::processDisplay() {
	if (ImGui::TreeNode("Drag")) {
		ImGui::Text("k1 ");
		ImGui::SameLine();
		ImGui::InputFloat("## k1", &k1, 0.0f, 0.0f, "%.3f");

		ImGui::Text("k2 ");
		ImGui::SameLine();
		ImGui::InputFloat("## k2", &k2, 0.0f, 0.0f, "%.3f");

		ImGui::Text("Drag ");
		ImGui::SameLine();
		float force[] = { Force.x, Force.y };
		ImGui::InputFloat2("## Drag", force, "%.3f N", ImGuiInputTextFlags_ReadOnly);

		ImGui::TreePop();
	}
}