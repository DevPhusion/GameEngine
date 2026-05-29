#pragma once
#include "Object.h"
#include "PhysicsComponent.h"
#include "ForceGenerator.h"
class PhysicsEngine
{
protected:
	struct ForceRegistration {
		Object object;
		ForceGenerator* fg;
	};

	typedef std::vector<ForceRegistration> Registry;
	Registry ForceRegistrations;
public:
	PhysicsEngine(const PhysicsEngine&) = delete;
	void operator=(const PhysicsEngine&) = delete;

	static PhysicsEngine& getInstance() {
		static PhysicsEngine instance;
		return instance;
	}

	void Setup(std::vector<Object>* objects);
	void ProcessPhysics(float delta);
	void RegisterForce(Object object, ForceGenerator* fg);
	void UnRegisterForce(Object object, ForceGenerator* fg);
	void ClearRegistry();
private:
	PhysicsEngine() = default;
	std::vector<Object>* allObjects;
};

