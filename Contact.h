#pragma once
#include "Object.h"
#include "PhysicsComponent.h"
#include "TransformComponent.h"
class Contact
{
public:
	Contact(std::vector<Object*> objects, glm::vec3 contactNormal, float penetration, float restitution);
	Contact() = default;

	std::vector<Object*> objects;
	glm::vec3 contactNormal = glm::vec3(0);
	float penetration = 0;
	float contactRestitution;

	void ResolveContact(float delta);
	float CalculateSeparatingVelocity() const;
private:
	void resolveVelocity(float delta);
	void resolveInterpenetration(float delta);
};

