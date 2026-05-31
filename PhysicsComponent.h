#pragma once
#include "Component.h"
#include "TransformComponent.h"
#include "Object.h"
#include <glm/glm.hpp>
class PhysicsComponent : public Component
{
public:
	PhysicsComponent(Object* parent);
	PhysicsComponent() = default;

	glm::vec3 velocity = glm::vec3(0);
	glm::vec3 acceleration = glm::vec3(0);
	glm::vec3 netForce = glm::vec3(0);

	float inverseMass; 

	virtual void ProcessInspectorUI();
	void ProcessPhysics(float delta);
	void ClearNetForce();
	void AddForce(glm::vec3 force);
};

