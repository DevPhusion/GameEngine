#include "SpringComponent.h"

SpringComponent::SpringComponent(Object* parent, float springConstant, float damping, float restLength) : ObjectLinkComponent(parent) {
	Name = "Spring Component";
	this->springConstant = springConstant;
	this->damping = damping;
	this->restLength = restLength;
}

void SpringComponent::OnDelete() {
	ObjectLinkComponent::OnDelete();
	if (constraint != nullptr) {
		PhysicsEngine::getInstance().UnRegisterConstraint(constraint);
	}
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

	if (constraint == nullptr) {
		constraint = new DistanceConstraint(topObject, bottomObject, topConnectPoint, bottomConnectPoint, restLength);
		PhysicsEngine::getInstance().RegisterConstraint(constraint);
	}

	constraint->distance = restLength;
	constraint->stiffness = springConstant;
	constraint->damping = damping;
}

void SpringComponent::AddTopObject(Object* object)  {
	ObjectLinkComponent::AddTopObject(object);

	if (constraint == nullptr) {
		constraint = new DistanceConstraint(topObject, bottomObject, topConnectPoint, bottomConnectPoint, restLength);
		PhysicsEngine::getInstance().RegisterConstraint(constraint);
	}

	constraint->objectA = topObject;
	constraint->attachPointA = topConnectPoint;
}

void SpringComponent::AddBottomObject(Object* object) {
	ObjectLinkComponent::AddBottomObject(object);

	if (constraint == nullptr) {
		constraint = new DistanceConstraint(topObject, bottomObject, topConnectPoint, bottomConnectPoint, restLength);
		PhysicsEngine::getInstance().RegisterConstraint(constraint);
	}

	constraint->objectB = bottomObject;
	constraint->attachPointB = bottomConnectPoint;
}

void SpringComponent::RemoveTopObject() {
	
	if (constraint != nullptr) {
		constraint->objectA = nullptr;
		PhysicsEngine::getInstance().RegisterConstraint(constraint);
	}

	ObjectLinkComponent::RemoveTopObject();
}

void SpringComponent::RemoveBottomObject() {
	if (constraint != nullptr) {
		constraint->objectB = nullptr;
		PhysicsEngine::getInstance().RegisterConstraint(constraint);
	}

	ObjectLinkComponent::RemoveBottomObject();
}

void SpringComponent::OnTopDisplayUpdatePos() {
	ObjectLinkComponent::OnTopDisplayUpdatePos();

	if (constraint == nullptr) {
		constraint = new DistanceConstraint(topObject, bottomObject, topConnectPoint, bottomConnectPoint, restLength);
		PhysicsEngine::getInstance().RegisterConstraint(constraint);
	}

	constraint->attachPointA = topConnectPoint;
}

void SpringComponent::OnBottomDisplayUpdatePos() {
	ObjectLinkComponent::OnBottomDisplayUpdatePos();

	if (constraint == nullptr) {
		constraint = new DistanceConstraint(topObject, bottomObject, topConnectPoint, bottomConnectPoint, restLength);
		PhysicsEngine::getInstance().RegisterConstraint(constraint);
	}

	constraint->attachPointB = bottomConnectPoint;
}