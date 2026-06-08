#include "Contact.h"

Contact::Contact(std::vector<Object*> objects, glm::vec3 contactNormal, float penetration, float restitution) {
	this->objects = objects;
	this->contactNormal = contactNormal;
	this->penetration = penetration;
	this->contactRestitution = restitution;
}

void Contact::ResolveContact(float delta) {
	resolveVelocity(delta);
	resolveInterpenetration(delta);
}

void Contact::resolveInterpenetration(float delta) {
	if (penetration <= 0) return;

	float totalInverseMass = 0;
	if (objects[0]->HasComponent<PhysicsComponent>()) {
		totalInverseMass += objects[0]->GetComponent<PhysicsComponent>()->inverseMass;
	}
	if (objects[1]->HasComponent<PhysicsComponent>()) {
		totalInverseMass += objects[1]->GetComponent<PhysicsComponent>()->inverseMass;
	}

	if (totalInverseMass <= 0) return;

	glm::vec3 movePerIMass = contactNormal * (penetration / totalInverseMass);

	if (objects[0]->HasComponent<PhysicsComponent>()) {
		glm::vec3 pos = objects[0]->GetComponent<TransformComponent>()->GetWorldPosition();
		pos += movePerIMass * objects[0]->GetComponent<PhysicsComponent>()->inverseMass;
		objects[0]->GetComponent<TransformComponent>()->UpdateWorldPosition(pos);
	}
	if (objects[1]->HasComponent<PhysicsComponent>()) {
		glm::vec3 pos = objects[1]->GetComponent<TransformComponent>()->GetWorldPosition();
		pos += movePerIMass * -objects[1]->GetComponent<PhysicsComponent>()->inverseMass;
		objects[1]->GetComponent<TransformComponent>()->UpdateWorldPosition(pos);
	}
}

void Contact::resolveVelocity(float delta) {
	float sepVel = CalculateSeparatingVelocity();

	if (sepVel > 0) {
		return;
	}

	float newSepVel = -sepVel * contactRestitution;

	glm::vec3 accCausedVel = glm::vec3(0);
	if (objects[0]->HasComponent<PhysicsComponent>()) {
		accCausedVel = glm::vec3(objects[0]->GetComponent<PhysicsComponent>()->accelDisplay, 0.0f);
		if (objects[1]->HasComponent<PhysicsComponent>()) {
			accCausedVel -= glm::vec3(objects[1]->GetComponent<PhysicsComponent>()->accelDisplay, 0.0f);
		}
	}
	else if (objects[1]->HasComponent<PhysicsComponent>()) {
		//  If Object 0 is static, relative acceleration is -Object1
		accCausedVel = glm::vec3(-objects[1]->GetComponent<PhysicsComponent>()->accelDisplay, 0.0f);
	}

	float accCausedSepVel = glm::dot(accCausedVel, contactNormal * delta);

	if (accCausedSepVel < 0) {
		newSepVel += contactRestitution * accCausedSepVel;
		if (newSepVel < 0) newSepVel = 0;
	}

	float deltaVel = newSepVel - sepVel;

	float totalInverseMass = 0;
	if (objects[0]->HasComponent<PhysicsComponent>()) {
		totalInverseMass += objects[0]->GetComponent<PhysicsComponent>()->inverseMass;
	}
	if (objects[1]->HasComponent<PhysicsComponent>()) {
		totalInverseMass += objects[1]->GetComponent<PhysicsComponent>()->inverseMass;
	}

	if (totalInverseMass <= 0) return;

	float impulse = deltaVel / totalInverseMass;
	glm::vec3 impulsePerIMass = contactNormal * impulse;
	if (objects[0]->HasComponent<PhysicsComponent>()) {
		objects[0]->GetComponent<PhysicsComponent>()->velocity += impulsePerIMass * objects[0]->GetComponent<PhysicsComponent>()->inverseMass;
	}
	if (objects[1]->HasComponent<PhysicsComponent>()) {
		objects[1]->GetComponent<PhysicsComponent>()->velocity += impulsePerIMass * -objects[1]->GetComponent<PhysicsComponent>()->inverseMass;
	}
}

float Contact::CalculateSeparatingVelocity() const {
	if (objects.size() <= 0) return 0;

	if (objects[0]->HasComponent<PhysicsComponent>()) {
		glm::vec3 relativeVelocity = objects[0]->GetComponent<PhysicsComponent>()->velocity;
		if (objects[1]->HasComponent<PhysicsComponent>()) {
			relativeVelocity -= objects[1]->GetComponent<PhysicsComponent>()->velocity;
		}
		return glm::dot(relativeVelocity, contactNormal);
	}
	else if (objects[1]->HasComponent<PhysicsComponent>()) {
		glm::vec3 relativeVelocity = -objects[1]->GetComponent<PhysicsComponent>()->velocity;
		return glm::dot(relativeVelocity, contactNormal);
	}
	return 0; 
}