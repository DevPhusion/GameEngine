#include "MouseDrag.h"

MouseDrag::MouseDrag(float k, float c) {
	this->k = k;
	this->c = c;
}

void MouseDrag::updateForce(Object* object, float delta) {
	TransformComponent* trans = object->GetComponent<TransformComponent>();
	PhysicsComponent* phys = object->GetComponent<PhysicsComponent>();
	
	float scaledK = k * (1 / phys->inverseMass);
	float scaledC = c * (1 / phys->inverseMass);
	
	// project to model space
	glm::vec3 modelPos = trans->GetTransformedPoint(glm::vec3(InputManager::glX, InputManager::glY, 0), true);

	//project to world space
	glm::vec3 MouseWorldPos = trans->ProjectToWorld(modelPos);
	glm::vec3 ObjWorldPos = trans->GetWorldPosition();

	glm::vec3 displacement = MouseWorldPos - ObjWorldPos;
	glm::vec3 objVel = phys->velocity;
	Force = (scaledK * displacement) - (scaledC * objVel);

	phys->AddForce(Force);
}

void MouseDrag::processDisplay() {
	ImGuiTreeNodeFlags root_flags = ImGuiTreeNodeFlags_OpenOnArrow |
		ImGuiTreeNodeFlags_OpenOnDoubleClick |
		ImGuiTreeNodeFlags_SpanAvailWidth |
		ImGuiTreeNodeFlags_DefaultOpen;

	if (ImGui::TreeNodeEx("Mouse Drag", root_flags)) {
		ImGui::Text("k ");
		ImGui::SameLine();
		ImGui::InputFloat("## k", &k, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);

		ImGui::Text("c ");
		ImGui::SameLine();
		ImGui::InputFloat("## c", &c, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);

		ImGui::Text("Mouse Drag ");
		ImGui::SameLine();
		float force[] = { Force.x, Force.y };
		ImGui::InputFloat2("## Mouse Drag", force, "%.3f N", ImGuiInputTextFlags_ReadOnly);

		ImGui::TreePop();
	}
}