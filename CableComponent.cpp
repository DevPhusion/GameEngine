#include "CableComponent.h"

CableComponent::CableComponent(Object* parent, float maxLength, float restitution, bool retractable = true) : ObjectLinkComponent(parent) {
	this->Name = "Cable Component";
	this->maxLength = maxLength;
	this->retractable = retractable;
}

void CableComponent::OnDelete() {
	ObjectLinkComponent::OnDelete();
	if (constraint != nullptr) {
		PhysicsEngine::getInstance().UnRegisterConstraint(constraint);
	}
}

void CableComponent::ProcessInspectorUI() {
	ImGuiTreeNodeFlags root_flags = ImGuiTreeNodeFlags_OpenOnArrow |
		ImGuiTreeNodeFlags_OpenOnDoubleClick |
		ImGuiTreeNodeFlags_SpanAvailWidth |
		ImGuiTreeNodeFlags_DefaultOpen;

	ObjectSelectUI();
	
	ImGui::Text("Retractable ");
	ImGui::SameLine();
	ImGui::Checkbox("## Retractable", &retractable);
	ImGui::Text("Max Length ");
	ImGui::SameLine();
	ImGui::InputFloat("## Max Length", &maxLength, 0.0f, 0.0f, "%.3f m");
}

void CableComponent::FillContact() {
	if (constraint == nullptr) {
		constraint = new DistanceConstraint(nullptr, nullptr, glm::vec3(0.0f), glm::vec3(0.0f), maxLength);
		PhysicsEngine::getInstance().RegisterConstraint(constraint);
	}

	constraint->objectA = topObject;
	constraint->objectB = bottomObject;
	constraint->attachPointA = topConnectPoint;
	constraint->attachPointB = bottomConnectPoint;
	constraint->distance = maxLength;
	constraint->retractable = retractable;
}