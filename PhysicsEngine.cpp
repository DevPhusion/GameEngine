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
		if ((*allObjects)[i]->HasComponent<CollisionComponent>()) {
			(*allObjects)[i]->GetComponent<RenderComponent>()->color = glm::vec4(1);
		}
	}

	PotentialContact potentialContacts[200];
	unsigned totalContacts = root.getPotentialContacts(potentialContacts, 200);
	if (totalContacts > 0) {
		std::vector<Contact> contacts = GetContacts(potentialContacts, totalContacts);
		ResolveContacts(contacts, delta);
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

void PhysicsEngine::ResolveContacts(std::vector<Contact>& contacts, float delta) {
	if (contacts.size() == 0) return;
	
	PrepareContacts(contacts, delta);

	AdjustPositions(contacts, delta);

	AdjustVelocities(contacts, delta);
}

void PhysicsEngine::PrepareContacts(std::vector<Contact>& contacts, float delta) {
	for (auto& c : contacts)
	{
		c.calculateInternals(delta);
	}
}

void PhysicsEngine::AdjustVelocities(std::vector<Contact>& contacts, float delta) {
	unsigned int velocityIterationsUsed = 0;
	unsigned int velocityIterations = contacts.size() * 4;

	while (velocityIterationsUsed < velocityIterations) {
		float maxVioloation = -0.005f;
		int maxIndex = -1;

		for (size_t i = 0; i < contacts.size(); i++)
		{
			if (contacts[i].desiredDeltaVelocity < maxVioloation) {
				maxVioloation = contacts[i].desiredDeltaVelocity;
				maxIndex = i;
			}
		}

		if (maxIndex == -1) break;
		Contact& resolveContact = contacts[maxIndex];
		resolveContact.resolveVelocity(delta);

		for (size_t i = 0; i < contacts.size(); i++)
		{
			if (i == maxIndex) continue;

			for (int resolvedObjIndex = 0; resolvedObjIndex < 2; resolvedObjIndex++)
			{
				Object* sharedObj = resolveContact.objects[resolvedObjIndex];
				if (!sharedObj) continue;

				glm::vec3 vChange = resolveContact.velocityChange[resolvedObjIndex];
				float wChange = resolveContact.angularVelocityChange[resolvedObjIndex];

				if (contacts[i].objects[0] == sharedObj) {
					glm::vec3 deltaRotVel = glm::vec3(-wChange * contacts[i].relativeContactPosition[0].y,
						wChange * contacts[i].relativeContactPosition[0].x, 0.0f);
					glm::vec3 totalPointVelChange = vChange + deltaRotVel;

					glm::vec3 contactVelChange = contacts[i].WorldToContact(totalPointVelChange);
					contacts[i].contactVelocity -= contactVelChange;
					contacts[i].desiredDeltaVelocity += contactVelChange.x;
				}

				if (contacts[i].objects[1] == sharedObj) {
					glm::vec3 deltaRotVel = glm::vec3(-wChange * contacts[i].relativeContactPosition[1].y,
						wChange * contacts[i].relativeContactPosition[1].x, 0.0f);
					glm::vec3 totalPointVelChange = vChange + deltaRotVel;

					glm::vec3 contactVelChange = contacts[i].WorldToContact(totalPointVelChange);
					contacts[i].contactVelocity += contactVelChange;
					contacts[i].desiredDeltaVelocity -= contactVelChange.x;
				}
			}
		}

		velocityIterationsUsed++;
	}
}

void PhysicsEngine::AdjustPositions(std::vector<Contact>& contacts, float delta) {
	unsigned int positionIterationsUsed = 0;
	unsigned int positionIterations = contacts.size() * 4;
	float positionEpsilon = 0.01f;

	while (positionIterationsUsed < positionIterations) {
		float maxPenetration = positionEpsilon;
		int maxIndex = -1;

		for (size_t i = 0; i < contacts.size(); i++)
		{
			if (contacts[i].penetration > maxPenetration) {
				maxPenetration = contacts[i].penetration;
				maxIndex = i;
			}
		}

		if (maxIndex == -1) break;
		Contact& resolveContact = contacts[maxIndex];
		resolveContact.resolveInterpenetration(delta);
		resolveContact.penetration = 0;

		for (size_t i = 0; i < contacts.size(); i++)
		{
			if (i == maxIndex) continue;

			for (int resolvedObjIndex = 0; resolvedObjIndex < 2; resolvedObjIndex++)
			{
				Object* sharedObj = resolveContact.objects[resolvedObjIndex];
				if (!sharedObj) continue;

				glm::vec3 posChange = resolveContact.positionChange[resolvedObjIndex];
				float rotChange = resolveContact.rotationChange[resolvedObjIndex];

				if (contacts[i].objects[0] == sharedObj) {
					glm::vec3 deltaRotPos = glm::vec3(-rotChange * contacts[i].relativeContactPosition[0].y,
						rotChange * contacts[i].relativeContactPosition[0].x, 0.0f);
					glm::vec3 totalPointDisplacement = posChange + deltaRotPos;
					contacts[i].penetration -= glm::dot(totalPointDisplacement, contacts[i].contactNormal);
					contacts[i].penetration = std::max(contacts[i].penetration, 0.0f);
				}

				if (contacts[i].objects[1] == sharedObj) {
					glm::vec3 deltaRotPos = glm::vec3(-rotChange * contacts[i].relativeContactPosition[1].y,
						rotChange * contacts[i].relativeContactPosition[1].x, 0.0f);
					glm::vec3 totalPointDisplacement = posChange + deltaRotPos;
					contacts[i].penetration += glm::dot(totalPointDisplacement, contacts[i].contactNormal);
					contacts[i].penetration = std::max(contacts[i].penetration, 0.0f);
				}
			}
		}

		positionIterationsUsed++;
	}
}

std::vector<Contact> PhysicsEngine::GetContacts(PotentialContact* contacts, unsigned numContacts) {
	allContactPoints = {};
	std::vector<Contact> allContacts = {};

	for (int i = 0; i < numContacts; i++)
	{
		Object* objA = contacts[i].obj[0];
		Object* objB = contacts[i].obj[1];

		if (!objA->HasComponent<PhysicsComponent>()) {
			Object* temp = objB;
			objB = objA;
			objA = temp;
		}

		if (objA == nullptr || objB == nullptr) continue;

		CollisionData colData = SAT(objA, objB);

		if (colData.isColliding) {
			if (EngineManager::getInstance().debugMode) {
				objA->GetComponent<RenderComponent>()->color = glm::vec4(0, 1, 0, 1);
				objB->GetComponent<RenderComponent>()->color = glm::vec4(0, 1, 0, 1);
			}

			std::vector<ContactPoint> points = GenerateContactPoints(colData);

			for (int i = 0; i < points.size(); i++)
			{
				allContactPoints.push_back(points[i]);
				Contact contact = Contact(std::vector<Object*> {objA, objB}, points[i].normal, points[i].point, points[i].penetration, 0.6f, 0.4f, 0.6f);
				allContacts.push_back(contact);
			}

		}
		else if (EngineManager::getInstance().debugMode) {
			objA->GetComponent<RenderComponent>()->color = glm::vec4(1, 0, 0, 1);
			objB->GetComponent<RenderComponent>()->color = glm::vec4(1, 0, 0, 1);
		}
	}

	return allContacts;
}

std::vector<ContactPoint> PhysicsEngine::GenerateContactPoints(CollisionData collisionData) {
	std::vector<ContactPoint> ContactPoints = {};

	Edge referenceEdgeA = FindMostParallelEdge(collisionData.objAEdges, collisionData.normal);
	Edge referenceEdgeB = FindMostParallelEdge(collisionData.objBEdges, -collisionData.normal);
	Edge incidentEdgeA = FindMostAntiParallelEdge(collisionData.objAEdges, -collisionData.normal);
	Edge incidentEdgeB = FindMostAntiParallelEdge(collisionData.objBEdges, collisionData.normal);

	float scoreA = glm::dot(
		glm::vec3(glm::normalize(referenceEdgeA.end - referenceEdgeA.start).y, -glm::normalize(referenceEdgeA.end - referenceEdgeA.start).x, 0),
		collisionData.normal
	);
	float scoreB = glm::dot(
		glm::vec3(glm::normalize(referenceEdgeB.end - referenceEdgeB.start).y, -glm::normalize(referenceEdgeB.end - referenceEdgeB.start).x, 0),
		-collisionData.normal
	);

	Edge referenceEdge;
	Edge incidentEdge;

	if (scoreA > scoreB) {
		referenceEdge = referenceEdgeA;
		incidentEdge = incidentEdgeB;
	}
	else {
		referenceEdge = referenceEdgeB;
		incidentEdge = incidentEdgeA;
	}

	glm::vec3 v1 = referenceEdge.start;
	glm::vec3 v2 = referenceEdge.end;

	glm::vec3 tangent = glm::normalize(v2 - v1);
	glm::vec3 refNormal = glm::normalize(glm::vec3(tangent.y, -tangent.x, 0));

	// Point refNormal toward the incident edge
	glm::vec3 refMid = (v1 + v2) * 0.5f;
	glm::vec3 incMid = (incidentEdge.start + incidentEdge.end) * 0.5f;
	if (glm::dot(refNormal, incMid - refMid) > 0.0f) {
		refNormal = -refNormal;
	}

	glm::vec3 incidentVertices[2] = { incidentEdge.start, incidentEdge.end };

	// Clip left side
	glm::vec3 clipPoints1[2];
	glm::vec3 leftNormal = -tangent;
	float leftOffset = glm::dot(leftNormal, v1);

	int numPoints = ClipSegmentToLine(clipPoints1, incidentVertices, 2, leftNormal, leftOffset);
	if (numPoints < 2) return ContactPoints;

	// Clip right side
	glm::vec3 clipPoints2[2];
	glm::vec3 rightNormal = tangent;
	float rightOffset = glm::dot(rightNormal, v2);

	numPoints = ClipSegmentToLine(clipPoints2, clipPoints1, numPoints, rightNormal, rightOffset);
	if (numPoints == 0) return ContactPoints;

	glm::vec3 colNormal = collisionData.normal;

	float refOffset = -std::numeric_limits<float>::max();
	for (const auto& e : collisionData.objBEdges) {
		refOffset = std::max(refOffset, glm::dot(e.start, colNormal));
		refOffset = std::max(refOffset, glm::dot(e.end, colNormal));
	}

	for (int i = 0; i < numPoints; i++) {
		float depth = refOffset - glm::dot(colNormal, clipPoints2[i]);
		if (depth >= 0.0f) {
			ContactPoint cp;
			cp.point = clipPoints2[i];
			cp.penetration = depth;
			cp.normal = colNormal;
			ContactPoints.push_back(cp);
		}
	}

	return ContactPoints;
}

CollisionData PhysicsEngine::SAT(Object* objA, Object* objB) {
	RenderComponent* rcA = objA->GetComponent<RenderComponent>();
	TransformComponent* tcA = objA->GetComponent<TransformComponent>();
	RenderComponent* rcB = objB->GetComponent<RenderComponent>();
	TransformComponent* tcB = objB->GetComponent<TransformComponent>();

	std::vector<Edge> edgesA = rcA->edges;
	std::vector<Edge> edgesB = rcB->edges;

	std::vector<Edge> globalEdgesA;
	std::vector<Edge> globalEdgesB;

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

		Edge edge = Edge();
		edge.start = axis.start;
		edge.end = axis.end;
		globalEdgesA.push_back(edge);

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

		Edge edge = Edge();
		edge.start = axis.start;
		edge.end = axis.end;
		globalEdgesB.push_back(edge);

		vertsB.push_back(axis.start);
		glm::vec3 tangent = axis.end - axis.start;
		axis.normal = glm::normalize(glm::vec3(tangent.y, -tangent.x, 0));
		Axes.push_back(axis);
	}
	
	CollisionData data = CollisionData();
	float minOverlap = std::numeric_limits<float>::max();
	glm::vec3 minAxisNormal = glm::vec3(0);

	for (int i = 0; i < Axes.size(); i++)
	{
		Projection projA = ProjectOntoAxis(vertsA, Axes[i]);
		Projection projB = ProjectOntoAxis(vertsB, Axes[i]);

		if (!projA.Overlaps(projB)) {
			data.isColliding = false;
			return data;
		}

		float overlap = std::min(projA.max, projB.max) - std::max(projA.min, projB.min);

		if (overlap < minOverlap) {
			minOverlap = overlap;
			minAxisNormal = Axes[i].normal;
		}
	}
	glm::vec3 centerA = tcA->GetWorldPosition();
	glm::vec3 centerB = tcB->GetWorldPosition();
	glm::vec3 dirAB = centerA - centerB;

	if (glm::dot(minAxisNormal, dirAB) < 0.0f) {
		minAxisNormal = -minAxisNormal;
	}

	data.isColliding = true;
	data.penetration = minOverlap;
	data.normal = minAxisNormal;

	if (ComputeSignedArea(vertsA) > 0.0f) {
		std::reverse(vertsA.begin(), vertsA.end());
		globalEdgesA.clear();
		for (int i = 0; i < vertsA.size(); i++) {
			Edge e;
			e.start = vertsA[i];
			e.end = vertsA[(i + 1) % vertsA.size()];
			globalEdgesA.push_back(e);
		}
	}

	if (ComputeSignedArea(vertsB) > 0.0f) {
		std::reverse(vertsB.begin(), vertsB.end());
		globalEdgesB.clear();
		for (int i = 0; i < vertsB.size(); i++) {
			Edge e;
			e.start = vertsB[i];
			e.end = vertsB[(i + 1) % vertsB.size()];
			globalEdgesB.push_back(e);
		}
	}

	data.objAEdges = globalEdgesA;
	data.objBEdges = globalEdgesB;

	return data;
}

float PhysicsEngine::ComputeSignedArea(const std::vector<glm::vec3>& vertices) {
	float area = 0.0f;
	int n = vertices.size();
	for (int i = 0; i < n; i++) {
		int j = (i + 1) % n;
		area += vertices[i].x * vertices[j].y;
		area -= vertices[j].x * vertices[i].y;
	}
	return area * 0.5f;
}

Edge PhysicsEngine::FindMostParallelEdge(const std::vector<Edge>& edges, const glm::vec3& normal) {
	float maxDot = -INFINITY;
	Edge best;
	for (const auto& edge : edges) {
		glm::vec3 tangent = glm::normalize(edge.end - edge.start);
		glm::vec3 edgeNormal = glm::vec3(tangent.y, -tangent.x, 0.0f);
		float dot = glm::dot(edgeNormal, normal);
		if (dot > maxDot) { maxDot = dot; best = edge; }
	}
	return best;
}

Edge PhysicsEngine::FindMostAntiParallelEdge(const std::vector<Edge>& edges, const glm::vec3& normal) {
	float minDot = INFINITY;
	Edge best;
	for (const auto& edge : edges) {
		glm::vec3 tangent = glm::normalize(edge.end - edge.start);
		glm::vec3 edgeNormal = glm::vec3(tangent.y, -tangent.x, 0.0f);
		float dot = glm::dot(edgeNormal, normal);
		if (dot < minDot) { minDot = dot; best = edge; }
	}
	return best;
}

int PhysicsEngine::ClipSegmentToLine(glm::vec3 vOut[2], const glm::vec3 vIn[2], int numInPoints, const glm::vec3& normal, float offset) {
	int numOutPoints = 0;
	float d0 = glm::dot(normal, vIn[0]) - offset;
	float d1 = glm::dot(normal, vIn[1]) - offset;

	if (d0 <= 0.0f) vOut[numOutPoints++] = vIn[0];
	if (d1 <= 0.0f) vOut[numOutPoints++] = vIn[1];

	if ((d0 < 0.0f) != (d1 < 0.0f)) {
		float t = d0 / (d0 - d1);
		vOut[numOutPoints < 2 ? numOutPoints++ : 1] = vIn[0] + t * (vIn[1] - vIn[0]);
	}

	return numOutPoints;
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