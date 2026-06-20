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
	glm::vec2 netAcceleration = glm::vec3(0);
	float torqueDisplay = 0;
	float angularAcceleration = 0;
	std::vector<std::shared_ptr<std::function<void(int)>>> forceDisplayFunc;

	//Physics process
	glm::vec3 velocity = glm::vec3(0);
	glm::vec3 acceleration = glm::vec3(0);
	glm::vec3 netForce = glm::vec3(0);

	//For sleeping optimization
	float motion;
	bool isAwake = true;
	float sleepEpsilon = 0.4f;
	float bias = 0.98f;

	float angularDamping = 0.995f;
	float linearDamping = 0.995f;
	float angularVelocity;
	float Torque;
	float Inertia;
	float inverseInertia;

	float inverseMass; 

	virtual void OnDelete();
	virtual void ProcessInspectorUI();
	void IntegrateVelocities(float delta);
	void IntegratePositions(float delta);
	void SetAwake(const bool awake);
	void ClearAccumulators();
	void AddForce(glm::vec3 force);
	void AddForceAtBodyPoint(glm::vec3 force, glm::vec3 point); // point is in model space
	void AddForceAtPoint(glm::vec3 force, glm::vec3 point); // point is in world space
	void CalculateInertia();
	void AddDisplayFunc(std::shared_ptr<std::function<void(int)>> func);
	void RemoveDisplayFunc(std::shared_ptr<std::function<void(int)>> func);
};

