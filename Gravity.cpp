#include "Gravity.h"

Gravity::Gravity(glm::vec3 gravity) {
	this->gravity = gravity;
}

void Gravity::updateForce(Object object, float delta) {
	PhysicsComponent* phys = object.GetComponent<PhysicsComponent>();
	if (phys->inverseMass != 0) {
		phys->AddForce(gravity * phys->inverseMass);
	}
}
