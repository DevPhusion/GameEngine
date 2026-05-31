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
	glm::vec3 drag = -vel * (k1 * vel.length() + k2 * std::powf(vel.length(), 2));

	object->GetComponent<PhysicsComponent>()->AddForce(drag);
}