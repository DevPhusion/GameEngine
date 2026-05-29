#include "PhysicsComponent.h"

PhysicsComponent::PhysicsComponent(std::shared_ptr<Object> parent) {
	this->parent = parent;
	this->acceleration = glm::vec3(0);
	this->velocity = glm::vec3(0);
	this->netForce = glm::vec3(0);
}

void PhysicsComponent::ProcessPhysics(float delta) {
	TransformComponent* transform = this->parent->GetComponent<TransformComponent>();
	glm::vec3 position = transform->GetWorldPosition();

	position += velocity * delta;

	glm::vec3 resultingAcc = acceleration;
	resultingAcc += netForce * inverseMass;
	velocity += resultingAcc * delta;
	
	transform->UpdateWorldPosition(position);

	ClearNetForce();
}

void PhysicsComponent::ClearNetForce() {
	netForce = glm::vec3(0);
}

void PhysicsComponent::AddForce(glm::vec3 force) {
	netForce += force;
}