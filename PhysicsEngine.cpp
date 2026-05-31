#include "PhysicsEngine.h"

void PhysicsEngine::Setup(std::vector<std::unique_ptr<Object>>* objects) {
	this->allObjects = objects;
}

void PhysicsEngine::ProcessPhysics(float delta) {
	if (EngineManager::getInstance().EnginePhysicsMode == EngineManager::PhysicsMode::Pause) {
		return;
	}

	for (int i = 0; i < ForceRegistrations.size(); i++)
	{
		ForceRegistrations[i].fg->updateForce(ForceRegistrations[i].object, delta);
	}

	for (int i = 0; i < allObjects->size(); i++)
	{
		if ((*allObjects)[i]->HasComponent<PhysicsComponent>()) {
			(*allObjects)[i]->GetComponent<PhysicsComponent>()->ProcessPhysics(delta);
		}
	}
}

void PhysicsEngine::RegisterForce(Object* object, ForceGenerator* fg) {
	ForceRegistrations.push_back(ForceRegistration(object, fg));
}

void PhysicsEngine::UnRegisterForce(Object* object, ForceGenerator* fg) {
	for (int i = 0; i < ForceRegistrations.size(); i++)
	{
		if (ForceRegistrations[i].fg == fg) {
			ForceRegistrations.erase(ForceRegistrations.begin() + i);
		}
	}
}

void PhysicsEngine::ClearRegistry() {
	ForceRegistrations.clear();
}