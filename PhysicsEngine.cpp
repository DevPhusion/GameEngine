#include "PhysicsEngine.h"

void PhysicsEngine::Setup(std::vector<std::unique_ptr<Object>>* objects) {
	this->allObjects = objects;
}

void PhysicsEngine::ProcessPhysics(float delta) {
	if (EngineManager::getInstance().EnginePhysicsMode == EngineManager::PhysicsMode::Pause) {
		return;
	}

	ResolveContacts(delta);

	for (int i = 0; i < ForceRegistrations.size(); i++)
	{
		ForceRegistrations[i].fg->updateForce(ForceRegistrations[i].object, delta);
	}

	for (int i = 0; i < allObjects->size(); i++)
	{
		if ((*allObjects)[i]->HasComponent<PhysicsComponent>()) {
			(*allObjects)[i]->GetComponent<PhysicsComponent>()->ProcessPhysics(delta);
		}
		if ((*allObjects)[i]->HasComponent<CollisionComponent>()) {
			(*allObjects)[i]->GetComponent<RenderComponent>()->color = glm::vec4(1);
		}
	}

	PotentialContact contacts[200];
	unsigned totalContacts = root.getPotentialContacts(contacts, 200);
	if (totalContacts > 0) {
		CheckCollision(contacts, totalContacts);
	}
}

void PhysicsEngine::RegisterForce(Object* object, ForceGenerator* fg) {
	ForceRegistrations.push_back(ForceRegistration(object, fg));
	std::function<void(int)> Wrapper = [fg](int index) {fg->processDisplay(index);};
	std::shared_ptr<std::function<void(int)>> sharedFunc = std::make_shared<std::function<void(int)>>(Wrapper);
	fg->setDisplayFunc(sharedFunc);
	object->GetComponent<PhysicsComponent>()->AddDisplayFunc(sharedFunc);
}

void PhysicsEngine::UnRegisterForce(Object* object, ForceGenerator* fg) {
	for (int i = 0; i < ForceRegistrations.size(); i++)
	{
		if (ForceRegistrations[i].fg == fg && ForceRegistrations[i].object == object) {
			ForceRegistrations.erase(ForceRegistrations.begin() + i);
			object->GetComponent<PhysicsComponent>()->RemoveDisplayFunc(fg->displayFunc);
			fg->displayFunc = nullptr;
		}
	}
}

void PhysicsEngine::UnRegisterAllForce(Object* object) {
	for (auto it = ForceRegistrations.begin(); it != ForceRegistrations.end(); )
	{
		if (it->object == object) {
			std::cout << "Remove" << std::endl;

			object->GetComponent<PhysicsComponent>()->RemoveDisplayFunc(it->fg->displayFunc);
			it->fg->displayFunc = nullptr;
			it = ForceRegistrations.erase(it);
		}
		else {
			++it;
		}
	}
}

void PhysicsEngine::ClearRegistry() {
	ForceRegistrations.clear();
}

void PhysicsEngine::AddContact(Contact* contact) {
	contactArray.push_back(contact);
}

void PhysicsEngine::ResolveContacts(float delta) {
	if (contactArray.size() <= 0)
		return;

	float iterationUsed = 0;
	while (iterationUsed < contactArray.size() * 2) {
		float max = 0;
		unsigned int maxIndex = 0;
		for (int i = 0; i < contactArray.size(); i++)
		{
			float sepVel = contactArray[i]->CalculateSeparatingVelocity();
			if (sepVel < max) {
				max = sepVel;
				maxIndex = i;
			}
		}

		contactArray[maxIndex]->ResolveContact(delta);
		delete contactArray[maxIndex];
		contactArray.erase(contactArray.begin() + maxIndex);
		iterationUsed++;

		if (contactArray.empty()) break;
	}
}

void PhysicsEngine::CheckCollision(PotentialContact* contacts, unsigned numContacts) {
	for (int i = 0; i < numContacts; i++)
	{
		Object* objA = contacts[i].obj[0];
		Object* objB = contacts[i].obj[1];

		if (objA == nullptr || objB == nullptr) return;

		if (SAT(objA, objB)) {
			objA->GetComponent<RenderComponent>()->color = glm::vec4(0, 1, 0, 1);
			objB->GetComponent<RenderComponent>()->color = glm::vec4(0, 1, 0, 1);
		}
		else {
			objA->GetComponent<RenderComponent>()->color = glm::vec4(1, 0, 0, 1);
			objB->GetComponent<RenderComponent>()->color = glm::vec4(1, 0, 0, 1);
		}
	}
}

bool PhysicsEngine::SAT(Object* objA, Object* objB) {
	RenderComponent* rcA = objA->GetComponent<RenderComponent>();
	TransformComponent* tcA = objA->GetComponent<TransformComponent>();
	RenderComponent* rcB = objB->GetComponent<RenderComponent>();
	TransformComponent* tcB = objB->GetComponent<TransformComponent>();

	std::vector<Edge> edgesA = rcA->edges;
	std::vector<Edge> edgesB = rcB->edges;

	std::vector<SeparatingAxis> Axes;
	std::vector<glm::vec3> vertsA;
	std::vector<glm::vec3> vertsB;

	for (int i = 0; i < edgesA.size(); i++)
	{
		glm::vec3 start = edgesA[i].start;
		glm::vec3 end = edgesA[i].end;

		SeparatingAxis axis = SeparatingAxis();
		axis.start = tcA->ProjectToWorld(start);
		axis.end = tcA->ProjectToWorld(end);
		vertsA.push_back(axis.start);
		glm::vec3 tangent = axis.end - axis.start;
		axis.normal = glm::normalize(glm::vec3(tangent.y, -tangent.x, 0));
		Axes.push_back(axis);
	}

	for (int i = 0; i < edgesB.size(); i++)
	{
		glm::vec3 start = edgesB[i].start;
		glm::vec3 end = edgesB[i].end;

		SeparatingAxis axis = SeparatingAxis();
		axis.start = tcB->ProjectToWorld(start);
		axis.end = tcB->ProjectToWorld(end);
		vertsB.push_back(axis.start);
		glm::vec3 tangent = axis.end - axis.start;
		axis.normal = glm::normalize(glm::vec3(tangent.y, -tangent.x, 0));
		Axes.push_back(axis);
	}
	
	for (int i = 0; i < Axes.size(); i++)
	{
		Projection projA = ProjectOntoAxis(vertsA, Axes[i]);
		Projection projB = ProjectOntoAxis(vertsB, Axes[i]);

		if (!projA.Overlaps(projB)) return false;
	}

	return true;
}

Projection PhysicsEngine::ProjectOntoAxis(std::vector<glm::vec3>& vertices, SeparatingAxis axis) {
	float max = -INFINITY;
	float min = INFINITY;

	for (int i = 0; i < vertices.size(); i++)
	{
		float p = glm::dot(vertices[i], axis.normal);
		if (p > max) {
			max = p;
		}
		if (p < min) {
			min = p;
		}
	}

	Projection projection = Projection();
	projection.max = max;
	projection.min = min;
	return projection;
}

BAHNode<BoundingCircle>* PhysicsEngine::RegisterBoundingAreaNode(Object* obj, BoundingCircle boundingCircle) {
	if (root.obj == nullptr && root.children[0] == nullptr) {
		root.obj = obj;
		root.area = boundingCircle;
		for (int i = 0; i < allObjects->size(); i++)
		{
			if ((*allObjects)[i]->HasComponent<CollisionComponent>()) {
				(*allObjects)[i]->GetComponent<CollisionComponent>()->BAHnode = root.searchFor((*allObjects)[i].get());
			}
		}
		return &root;
	}
	else {
		BAHNode<BoundingCircle>* node = root.insert(obj, boundingCircle);

		for (int i = 0; i < allObjects->size(); i++)
		{
			if ((*allObjects)[i]->HasComponent<CollisionComponent>()) {
				(*allObjects)[i]->GetComponent<CollisionComponent>()->BAHnode = root.searchFor((*allObjects)[i].get());
			}
		}

		return node;
	}
}

void PhysicsEngine::UnRegisterBoundingAreaNode(Object* obj) {
	BAHNode<BoundingCircle>* node = root.searchFor(obj);
	node->removeLeaf();
}