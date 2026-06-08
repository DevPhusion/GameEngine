#include "CableComponent.h"

CableComponent::CableComponent(Object* parent, float maxLength, float restitution) : ObjectLinkComponent(parent) {
	this->Name = "Cable Component";
	this->maxLength = maxLength;
	this->restitution = restitution;
}

void CableComponent::ProcessInspectorUI() {
	ImGuiTreeNodeFlags root_flags = ImGuiTreeNodeFlags_OpenOnArrow |
		ImGuiTreeNodeFlags_OpenOnDoubleClick |
		ImGuiTreeNodeFlags_SpanAvailWidth |
		ImGuiTreeNodeFlags_DefaultOpen;

	ObjectSelectUI();

	ImGui::Text("Max Length ");
	ImGui::SameLine();
	ImGui::InputFloat("## Max Length", &maxLength, 0.0f, 0.0f, "%.3f m");
	ImGui::Text("Restitution ");
	ImGui::SameLine();
	ImGui::InputFloat("## Restitution", &restitution, 0.0f, 0.0f, "%.3f");
}

void CableComponent::FillContact() {
	if (topObject == nullptr || bottomObject == nullptr || EngineManager::getInstance().EnginePhysicsMode == EngineManager::PhysicsMode::Pause) return;

	glm::vec3 topPos = topObject->GetComponent<TransformComponent>()->GetWorldPosition();
	glm::vec3 bottomPos = bottomObject->GetComponent<TransformComponent>()->GetWorldPosition();
	float length = glm::length(topPos - bottomPos);
	if (length < maxLength) {
		return;
	}
	glm::vec3 normal = glm::normalize(bottomPos - topPos);

	CableContact = new Contact(std::vector<Object*> {topObject, bottomObject}, normal, length - maxLength, restitution);

	PhysicsEngine::getInstance().AddContact(CableContact);
}