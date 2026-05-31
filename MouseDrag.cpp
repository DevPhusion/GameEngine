#include "MouseDrag.h"

MouseDrag::MouseDrag(float k, float c) {
	this->k = k;
	this->c = c;
}

void MouseDrag::updateForce(Object* object, float delta) {
	TransformComponent* trans = object->GetComponent<TransformComponent>();
	PhysicsComponent* phys = object->GetComponent<PhysicsComponent>();
	// project to model space
	glm::vec3 modelPos = trans->GetTransformedPoint(glm::vec3(InputManager::glX, InputManager::glY, 0), true);

	//project to world space
	glm::vec3 MouseWorldPos = trans->ProjectToWorld(modelPos);
	glm::vec3 ObjWorldPos = trans->GetWorldPosition();

	glm::vec3 displacement = MouseWorldPos - ObjWorldPos;
	glm::vec3 objVel = phys->velocity;
	glm::vec3 force = (k * displacement) - (c * objVel);

	phys->AddForce(force);
}
