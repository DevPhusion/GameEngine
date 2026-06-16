#pragma once
#include "Object.h"
#include "PhysicsComponent.h"
#include "TransformComponent.h"
class Contact
{
public:
	Contact(std::vector<Object*> objects, glm::vec3 contactNormal, glm::vec3 contactPoint, float penetration, float restitution);
	Contact() = default;

	std::vector<Object*> objects;
	glm::vec3 contactNormal = glm::vec3(0);
	glm::vec3 contactPoint = glm::vec3(0);
	float penetration = 0;
	float contactRestitution;
	float angularLimitConstant = 0.5f;
	
	glm::vec3 contactVelocity = glm::vec3(0);
	float desiredDeltaVelocity;

	glm::vec3 positionChange[2] = { glm::vec3(0.0f), glm::vec3(0.0f) };
	float rotationChange[2] = { 0.0f, 0.0f };
	glm::vec3 relativeContactPosition[2] = { glm::vec3(0.0f), glm::vec3(0.0f) };

	glm::vec3 velocityChange[2] = { glm::vec3(0.0f), glm::vec3(0.0f) };
	float angularVelocityChange[2] = { 0.0f, 0.0f };

	void calculateInternals(float delta);
	void resolveVelocity(float delta);
	void resolveInterpenetration(float delta);
	glm::vec3 WorldToContact(glm::vec3 worldPos);
	glm::vec3 ContactToWorld(glm::vec3 contactPos);
private:
	glm::vec3 calculateLocalVelocity(unsigned bodyIndex, float delta);
};

