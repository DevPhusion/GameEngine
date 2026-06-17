#include "Contact.h"

Contact::Contact(std::vector<Object*> objects, glm::vec3 contactNormal, glm::vec3 contactPoint, float penetration, float restitution, float staticFriction, float dynamicFriction) {
	this->objects = objects;
	this->contactNormal = contactNormal;
	this->contactPoint = contactPoint;
	this->penetration = penetration;
	this->contactRestitution = restitution;
	this->staticFrictionCoefficient = staticFriction;
	this->dynamicFrictionCoefficient = dynamicFriction;
}

glm::vec3 Contact::WorldToContact(glm::vec3 worldPos) {
	glm::vec2 normal = glm::vec2(contactNormal.x, contactNormal.y);
	glm::vec2 tangent = glm::vec2(-normal.y, normal.x);
	glm::mat2 worldToContact = glm::transpose(glm::mat2(normal, tangent));
	glm::vec2 transformed = worldToContact * glm::vec2(worldPos.x, worldPos.y);
	return glm::vec3(transformed.x, transformed.y, 0);
}

glm::vec3 Contact::ContactToWorld(glm::vec3 contactPos) {
	glm::vec2 normal = glm::vec2(contactNormal.x, contactNormal.y);
	glm::vec2 tangent = glm::vec2(-normal.y, normal.x);
	glm::mat2 contactToWorld = glm::mat2(normal, tangent);
	glm::vec2 transformed = contactToWorld * glm::vec2(contactPos.x, contactPos.y);
	return glm::vec3(transformed.x, transformed.y, 0);
}

void Contact::calculateInternals(float delta) {
	glm::vec3 velA = glm::vec3(0.0f);
	glm::vec3 velB = glm::vec3(0.0f);

	if (objects[0] && objects[0]->HasComponent<PhysicsComponent>()) {
		TransformComponent* tc = objects[0]->GetComponent<TransformComponent>();
		relativeContactPosition[0] = contactPoint - tc->GetWorldPosition();
		velA = calculateLocalVelocity(0, delta);
	}

	if (objects[1] && objects[1]->HasComponent<PhysicsComponent>()) {
		TransformComponent* tc = objects[1]->GetComponent<TransformComponent>();
		relativeContactPosition[1] = contactPoint - tc->GetWorldPosition();
		velB = calculateLocalVelocity(1, delta);
	}

	glm::vec3 relativeVel = velB - velA;
	contactVelocity = WorldToContact(relativeVel);

	float velFromAccA = 0.0f, velFromAccB = 0.0f;
	if (objects[0] && objects[0]->HasComponent<PhysicsComponent>()) {
		PhysicsComponent* pc = objects[0]->GetComponent<PhysicsComponent>();
		velFromAccA = glm::dot(glm::vec3(pc->netAcceleration, 0), contactNormal) * delta;
	}
	if (objects[1] && objects[1]->HasComponent<PhysicsComponent>()) {
		PhysicsComponent* pc = objects[1]->GetComponent<PhysicsComponent>();
		velFromAccB = glm::dot(glm::vec3(pc->netAcceleration, 0), contactNormal) * delta;
	}
	float velFromAcc = velFromAccB - velFromAccA;
	float closingVel = contactVelocity.x - velFromAcc;
	float restitution = (abs(contactVelocity.x) < 0.5f) ? 0.0f : contactRestitution;
	desiredRestitutionVelocity = restitution * closingVel;
	desiredDeltaVelocity = -(1 + restitution) * closingVel;  
}

glm::vec3 Contact::calculateLocalVelocity(unsigned bodyIndex, float delta) {
	TransformComponent* tc = objects[bodyIndex]->GetComponent<TransformComponent>();
	PhysicsComponent* pc = objects[bodyIndex]->GetComponent<PhysicsComponent>();
	glm::vec3 r = contactPoint - tc->GetWorldPosition();
	glm::vec3 rotationalVel = glm::vec3(-pc->angularVelocity * r.y, pc->angularVelocity * r.x, 0.0f);
	return pc->velocity + rotationalVel;
}

void Contact::resolveInterpenetration(float delta) {

	float angularInertia[2] = { 0.0f, 0.0f };
	float linearInertia[2] = { 0.0f, 0.0f };
	float totalInertia = 0.0f;

	glm::vec3 r[2] = { glm::vec3(0.0f), glm::vec3(0.0f) };

	positionChange[0] = positionChange[1] = glm::vec3(0.0f);
	rotationChange[0] = rotationChange[1] = 0.0f;

	for (int i = 0; i < 2; i++)
	{
		if (objects[i] && objects[i]->HasComponent<PhysicsComponent>()) {
			PhysicsComponent* pc = objects[i]->GetComponent<PhysicsComponent>();
			TransformComponent* tc = objects[i]->GetComponent<TransformComponent>();

			r[i] = contactPoint - tc->GetWorldPosition();
			relativeContactPosition[i] = r[i];

			float torque = r[i].x * contactNormal.y - r[i].y * contactNormal.x;
			angularInertia[i] = (torque * torque) * pc->inverseInertia;
			linearInertia[i] = pc->inverseMass;
			totalInertia += linearInertia[i] + angularInertia[i];
		}
	}

	if (totalInertia <= 0) return;
	float inverseInertia = 1 / totalInertia;
	if (objects[0]->HasComponent<PhysicsComponent>()) {
		TransformComponent* tcA = objects[0]->GetComponent<TransformComponent>();
		PhysicsComponent* pcA = objects[0]->GetComponent<PhysicsComponent>();
		float linearMove = penetration * linearInertia[0] * inverseInertia;
		float angularMove = penetration * angularInertia[0] * inverseInertia;

		float limit = angularLimitConstant * glm::length(r[0]);
		if (abs(angularMove) > limit) {
			float totalMove = linearMove + angularMove;
			if (angularMove >= 0) {
				angularMove = limit;
			}
			else {
				angularMove = -limit;
			}
			linearMove = totalMove - angularMove;
		}


		float torqueA = r[0].x * contactNormal.y - r[0].y * contactNormal.x;

		positionChange[0] = contactNormal * linearMove;
		rotationChange[0] = (torqueA == 0.0f) ? 0.0f : angularMove / torqueA;
		tcA->UpdateWorldPosition(tcA->GetWorldPosition() + positionChange[0]);
		tcA->Rotate(tcA->rotation + rotationChange[0]);
	}
	if (objects[1]->HasComponent<PhysicsComponent>()) {
		TransformComponent* tcB = objects[1]->GetComponent<TransformComponent>();
		PhysicsComponent* pcB = objects[1]->GetComponent<PhysicsComponent>();
		float linearMove = -penetration * linearInertia[1] * inverseInertia;
		float angularMove = -penetration * angularInertia[1] * inverseInertia;

		float limit = angularLimitConstant * glm::length(r[1]);
		if (abs(angularMove) > limit) {
			float totalMove = linearMove + angularMove;
			if (angularMove >= 0) {
				angularMove = limit;
			}
			else {
				angularMove = -limit;
			}
			linearMove = totalMove - angularMove;
		}

		float torqueB = -(r[1].x * contactNormal.y - r[1].y * contactNormal.x);
		
		positionChange[1] = contactNormal * linearMove;
		rotationChange[1] = (torqueB == 0.0f) ? 0.0f : angularMove / torqueB;
		tcB->UpdateWorldPosition(tcB->GetWorldPosition() + positionChange[1]);
		tcB->Rotate(tcB->rotation + rotationChange[1]);
	}
}

void Contact::resolveVelocity(float delta) {
	float deltaVelocityNormal = 0;
	float deltaVelocityTangent = 0;
	glm::vec3 velA = glm::vec3(0);
	glm::vec3 velB = glm::vec3(0);
	PhysicsComponent* pcA = objects[0]->GetComponent<PhysicsComponent>();
	PhysicsComponent* pcB = objects[1]->GetComponent<PhysicsComponent>();

	TransformComponent* tcA = objects[0]->GetComponent<TransformComponent>();
	glm::vec3 rA = contactPoint - tcA->GetWorldPosition();

	TransformComponent* tcB = objects[1]->GetComponent<TransformComponent>();
	glm::vec3 rB = contactPoint - tcB->GetWorldPosition();

	glm::vec3 contactTangent = glm::vec3(-contactNormal.y, contactNormal.x, 0.0f);

	if (pcA != nullptr) {
		float torqueANormal = rA.x * contactNormal.y - rA.y * contactNormal.x;
		float torqueATangent = rA.x * contactTangent.y - rA.y * contactTangent.x;
		deltaVelocityNormal += pcA->inverseMass + (torqueANormal * torqueANormal) * pcA->inverseInertia;
		deltaVelocityTangent += pcA->inverseMass + (torqueATangent * torqueATangent) * pcA->inverseInertia;

		glm::vec3 rotVelA = glm::vec3(-pcA->angularVelocity * rA.y, pcA->angularVelocity * rA.x, 0.0f);
		velA += pcA->velocity + rotVelA;
	}
	if (pcB != nullptr) {
		float torqueBNormal = rB.x * contactNormal.y - rB.y * contactNormal.x;
		float torqueBTangent = rB.x * contactTangent.y - rB.y * contactTangent.x;
		deltaVelocityNormal += pcB->inverseMass + (torqueBNormal * torqueBNormal) * pcB->inverseInertia;
		deltaVelocityTangent += pcB->inverseMass + (torqueBTangent * torqueBTangent) * pcB->inverseInertia;

		glm::vec3 rotVelB = glm::vec3(-pcB->angularVelocity * rB.y, pcB->angularVelocity * rB.x, 0.0f);
		velB += pcB->velocity + rotVelB;
	}

	glm::vec3 relativeVelocity = velB - velA;
	glm::vec3 contactVel = WorldToContact(relativeVelocity);

	// FIX 1: Drive the solver directly using the calculated remaining delta velocity constraint
	float targetDeltaVelocity = desiredDeltaVelocity;
	if (targetDeltaVelocity >= 0) return;

	float frictionImpulse = -contactVel.y / deltaVelocityTangent;
	float contactImpulse = targetDeltaVelocity / deltaVelocityNormal;

	float maxStatic = staticFrictionCoefficient * abs(contactImpulse);

	if (abs(frictionImpulse) > maxStatic) {
		frictionImpulse = glm::sign(frictionImpulse) * dynamicFrictionCoefficient * abs(contactImpulse);
	}

	glm::vec3 impulseContact = glm::vec3(contactImpulse, frictionImpulse, 0);
	glm::vec3 impulse = ContactToWorld(impulseContact);

	if (pcA != nullptr) {
		glm::vec3 velocityChangeA = impulse * pcA->inverseMass;
		float impulsiveTorque = rA.x * impulse.y - rA.y * impulse.x;
		float rotationalChangeA = pcA->inverseInertia * impulsiveTorque;

		pcA->velocity -= velocityChangeA;
		pcA->angularVelocity -= rotationalChangeA;
		velocityChange[0] = -velocityChangeA;
		angularVelocityChange[0] = -rotationalChangeA;
	}

	if (pcB != nullptr) {
		glm::vec3 velocityChangeB = impulse * pcB->inverseMass;
		float impulsiveTorque = rB.x * impulse.y - rB.y * impulse.x;
		float rotationalChangeB = pcB->inverseInertia * impulsiveTorque;

		pcB->velocity += velocityChangeB;
		pcB->angularVelocity += rotationalChangeB;
		velocityChange[1] = velocityChangeB;
		angularVelocityChange[1] = rotationalChangeB;
	}

	// FIX 2: This contact constraint is met for this iteration step
	desiredDeltaVelocity = 0;
}