#pragma once
#include "Object.h"
#include "Contact.h"
#include "PhysicsComponent.h"
#include "ForceGenerator.h"
class PhysicsEngine
{
protected:
	struct ForceRegistration {
		Object* object;
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

	void Setup(std::vector<std::unique_ptr<Object>>* objects);
	void ProcessPhysics(float delta);
	//Force
	void RegisterForce(Object* object, ForceGenerator* fg);
	void UnRegisterForce(Object* object, ForceGenerator* fg);
	void UnRegisterAllForce(Object* object);
	void ClearRegistry();

	//Contact resolution
	std::vector<Contact*> contactArray;
	void AddContact(Contact* contact);
	void ResolveContacts(float delta);
private:
	PhysicsEngine() = default;
	std::vector<std::unique_ptr<Object>>* allObjects;
};

