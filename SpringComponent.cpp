#include "SpringComponent.h"

SpringComponent::SpringComponent(Object* parent, float springConstant, float damping, float restLength, float angularSpringConstant, float angularDamping, float restAngle) : ObjectLinkComponent(parent) {
	Name = "Spring Component";
	this->springConstant = springConstant;
	this->damping = damping;
	this->restLength = restLength;
	this->angularSpringConstant = angularSpringConstant;
	this->angularDamping = angularDamping;
	this->restAngle = restAngle;
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

	ImGui::Text("Angular Spring Constant ");
	ImGui::SameLine();
	ImGui::InputFloat("## Angular Spring Constant", &angularSpringConstant, 0.0f, 0.0f, "%.3f Nm/rad");
	ImGui::Text("Angular Damping ");
	ImGui::SameLine();
	ImGui::InputFloat("## Angular Damping", &angularDamping, 0.0f, 0.0f, "%.3f Nms/rad");
	ImGui::Text("Rest Angle ");
	ImGui::SameLine();
	ImGui::SliderAngle("## Rest Angle", &restAngle);

	if (springForceBot != nullptr) {
		springForceBot->springConstant = springConstant;
		springForceBot->damping = damping;
		springForceBot->restLength = restLength;
		springForceBot->angularSpringConstant = angularSpringConstant;
		springForceBot->angularDamping = angularDamping;
		springForceBot->restAngle = restAngle;
	}

	if (springForceTop != nullptr) {
		springForceTop->springConstant = springConstant;
		springForceTop->damping = damping;
		springForceTop->restLength = restLength;
		springForceTop->angularSpringConstant = angularSpringConstant;
		springForceTop->angularDamping = angularDamping;
		springForceTop->restAngle = restAngle;
	}
}

void SpringComponent::AddTopObject(Object* object)  {
	ObjectLinkComponent::AddTopObject(object);

	if (springForceTop == nullptr) {
		springForceTop = new SpringForce(topObject, bottomObject, 
			topConnectPoint, bottomConnectPoint, 
			springConstant, damping, restLength,
			angularSpringConstant, angularDamping, restAngle
		);
	}
	else {
		springForceTop->thisConnectionPoint = topConnectPoint;
		springForceTop->thisObject = topObject;
	}

	if (springForceBot != nullptr) {
		springForceBot->otherObject = topObject;
		springForceBot->otherConnectionPoint = topConnectPoint;
	}

	if (!topObject->HasComponent<PhysicsComponent>()) return;

	PhysicsEngine::getInstance().RegisterForce(topObject, springForceTop);
}

void SpringComponent::AddBottomObject(Object* object) {
	ObjectLinkComponent::AddBottomObject(object);

	if (springForceBot == nullptr) {
		springForceBot = new SpringForce(bottomObject, topObject, 
			bottomConnectPoint, topConnectPoint, 
			springConstant, damping, restLength,
			angularSpringConstant, angularDamping, restAngle
		);
	}
	else {
		springForceBot->thisConnectionPoint = bottomConnectPoint;
		springForceBot->thisObject = bottomObject;
	}

	if (springForceTop != nullptr) {
		springForceTop->otherConnectionPoint = bottomConnectPoint;
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

void SpringComponent::OnTopDisplayUpdatePos() {
	ObjectLinkComponent::OnTopDisplayUpdatePos();

	springForceTop->thisConnectionPoint = topConnectPoint;
	if (springForceBot != nullptr) {
		springForceBot->otherConnectionPoint = topConnectPoint;
	}
}

void SpringComponent::OnBottomDisplayUpdatePos() {
	ObjectLinkComponent::OnBottomDisplayUpdatePos();

	springForceBot->thisConnectionPoint = bottomConnectPoint;
	if (springForceTop != nullptr) {
		springForceTop->otherConnectionPoint = bottomConnectPoint;
	}
}