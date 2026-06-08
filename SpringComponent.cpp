#include "SpringComponent.h"

SpringComponent::SpringComponent(Object* parent, float springConstant, float damping, float restLength) : ObjectLinkComponent(parent) {
	Name = "Spring Component";
	this->springConstant = springConstant;
	this->damping = damping;
	this->restLength = restLength;
}

void SpringComponent::ProcessInspectorUI() {
	ObjectSelectUI();
		
	ImGui::Text("Spring Constant ");
	ImGui::SameLine();
	ImGui::InputFloat("## Spring Constant", &springConstant, 0.0f, 0.0f, "%.3f N/m");
	ImGui::Text("Damping ");
	ImGui::SameLine();
	ImGui::InputFloat("## Damping", &damping, 0.0f, 0.0f, "%.3f Ns/m");
	ImGui::Text("Rest Length ");
	ImGui::SameLine();
	ImGui::InputFloat("## Rest Length", &restLength, 0.0f, 0.0f, "%.3f m");

	if (springForceBot != nullptr) {
		springForceBot->springConstant = springConstant;
		springForceBot->damping = damping;
		springForceBot->restLength = restLength;
	}

	if (springForceTop != nullptr) {
		springForceTop->springConstant = springConstant;
		springForceTop->damping = damping;
		springForceTop->restLength = restLength;
	}
}

void SpringComponent::AddTopObject(Object* object)  {
	ObjectLinkComponent::AddTopObject(object);

	if (springForceTop == nullptr) {
		springForceTop = new SpringForce(topObject, bottomObject, springConstant, damping, restLength);
	}
	else {
		springForceTop->thisObject = topObject;
	}

	if (springForceBot != nullptr) {
		springForceBot->otherObject = topObject;
	}

	if (!topObject->HasComponent<PhysicsComponent>()) return;

	PhysicsEngine::getInstance().RegisterForce(topObject, springForceTop);
}

void SpringComponent::AddBottomObject(Object* object) {
	ObjectLinkComponent::AddBottomObject(object);

	if (springForceBot == nullptr) {
		springForceBot = new SpringForce(bottomObject, topObject, springConstant, damping, restLength);
	}
	else {
		springForceBot->thisObject = bottomObject;
	}

	if (springForceTop != nullptr) {
		springForceTop->otherObject = bottomObject;
	}

	if (!bottomObject->HasComponent<PhysicsComponent>()) return;

	PhysicsEngine::getInstance().RegisterForce(bottomObject, springForceBot);
}

void SpringComponent::RemoveTopObject() {
	if (topObject->HasComponent<PhysicsComponent>() && springForceTop != nullptr) {
		PhysicsEngine::getInstance().UnRegisterForce(topObject, springForceTop);	
		springForceTop = nullptr;
	}

	if (bottomObject != nullptr && bottomObject->HasComponent<PhysicsComponent>() && springForceBot != nullptr) {
		springForceBot->otherObject = nullptr;
	}

	ObjectLinkComponent::RemoveTopObject();
}

void SpringComponent::RemoveBottomObject() {
	if (bottomObject->HasComponent<PhysicsComponent>() && springForceBot != nullptr) {
		PhysicsEngine::getInstance().UnRegisterForce(bottomObject, springForceBot);
		springForceBot = nullptr;
	}

	if (topObject != nullptr && topObject->HasComponent<PhysicsComponent>() && springForceTop != nullptr) {
		springForceTop->otherObject = nullptr;
	}

	ObjectLinkComponent::RemoveBottomObject();
}