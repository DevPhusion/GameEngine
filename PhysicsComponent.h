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

	//For UI display
	glm::vec2 netForceDisplay = glm::vec3(0);
	glm::vec2 accelDisplay = glm::vec3(0);
	std::vector<std::shared_ptr<std::function<void(int)>>> forceDisplayFunc;

	//Physics process
	glm::vec3 velocity = glm::vec3(0);
	glm::vec3 acceleration = glm::vec3(0);
	glm::vec3 netForce = glm::vec3(0);

	float inverseMass; 

	virtual void ProcessInspectorUI();
	void ProcessPhysics(float delta);
	void ClearNetForce();
	void AddForce(glm::vec3 force);
	void AddDisplayFunc(std::shared_ptr<std::function<void(int)>> func);
	void RemoveDisplayFunc(std::shared_ptr<std::function<void(int)>> func);
};

