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
	PhysicsEngine(std::vector<Object>* objects);
	void ProcessPhysics(float delta);
	void RegisterForce(Object object, ForceGenerator* fg);
	void UnRegisterForce(Object object, ForceGenerator* fg);
	void ClearRegistry();
private:
	std::vector<Object>* allObjects;
};

