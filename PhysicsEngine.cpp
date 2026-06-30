#include "PhysicsEngine.h"
#include "SoftBodyComponent.h"

void PhysicsEngine::Setup(std::vector<std::unique_ptr<Object>>* objects) {
	this->allObjects = objects;
}

void PhysicsEngine::ProcessPhysics(float delta) {
	if (EngineManager::getInstance().EnginePhysicsMode == EngineManager::PhysicsMode::Pause) {
		return;
	}

	UnRegisterTemporaryConstraint();

	for (int i = 0; i < ForceRegistrations.size(); i++)
	{
		ForceRegistrations[i].fg->updateForce(ForceRegistrations[i].object, delta);
	}

	for (int i = 0; i < allObjects->size(); i++)
	{
		if ((*allObjects)[i]->HasComponent<RigidBodyComponent>()) {
			(*allObjects)[i]->GetComponent<RigidBodyComponent>()->IntegrateVelocities(delta);
		}
		if ((*allObjects)[i]->HasComponent<SoftBodyComponent>()) {
			(*allObjects)[i]->GetComponent<SoftBodyComponent>()->ProcessSoftBody(delta);
		}
		if ((*allObjects)[i]->HasComponent<CollisionComponent>()) {
			(*allObjects)[i]->GetComponent<RenderComponent>()->color = glm::vec4(1);
		}
	}

	PotentialContact potentialContacts[200];
	unsigned totalContacts = root.getPotentialContacts(potentialContacts, 200);
	if (totalContacts > 0) {
		ResolveContacts(potentialContacts, totalContacts);
	}

	ResolveConstraints(delta);

	UpdateContactCache();

	for (int i = 0; i < allObjects->size(); i++)
	{
		if ((*allObjects)[i]->HasComponent<RigidBodyComponent>()) {
			(*allObjects)[i]->GetComponent<RigidBodyComponent>()->IntegratePositions(delta);
		}
	}
}

void PhysicsEngine::RegisterForce(Object* object, ForceGenerator* fg) {
	ForceRegistrations.push_back(ForceRegistration(object, fg));
	std::function<void(int)> Wrapper = [fg](int index) {fg->processDisplay(index);};
	std::shared_ptr<std::function<void(int)>> sharedFunc = std::make_shared<std::function<void(int)>>(Wrapper);
	fg->setDisplayFunc(sharedFunc);
	object->GetComponent<RigidBodyComponent>()->AddDisplayFunc(sharedFunc);
}

void PhysicsEngine::UnRegisterForce(Object* object, ForceGenerator* fg) {
	for (int i = 0; i < ForceRegistrations.size(); i++)
	{
		if (ForceRegistrations[i].fg == fg && ForceRegistrations[i].object == object) {
			ForceRegistrations.erase(ForceRegistrations.begin() + i);
			object->GetComponent<RigidBodyComponent>()->RemoveDisplayFunc(fg->displayFunc);
			fg->displayFunc = nullptr;
		}
	}
}

void PhysicsEngine::UnRegisterAllForce(Object* object) {
	for (auto it = ForceRegistrations.begin(); it != ForceRegistrations.end(); )
	{
		if (it->object == object) {
			std::cout << "Remove" << std::endl;

			object->GetComponent<RigidBodyComponent>()->RemoveDisplayFunc(it->fg->displayFunc);
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

// Collision

void PhysicsEngine::ResolveContacts(PotentialContact* contacts, unsigned numContacts) {
	allContactPoints = {};

	for (int i = 0; i < numContacts; i++)
	{
		Object* objA = contacts[i].obj[0];
		Object* objB = contacts[i].obj[1];

		if (objA == nullptr || objB == nullptr) continue;

		if (!objA->HasComponent<RigidBodyComponent>() || (objB->HasComponent<RigidBodyComponent>() && objA > objB)) {
			std::swap(objA, objB);
		}

		RenderComponent* rcA = objA->GetComponent<RenderComponent>();
		RenderComponent* rcB = objB->GetComponent<RenderComponent>();
		TransformComponent* tcA = objA->GetComponent<TransformComponent>();
		TransformComponent* tcB = objB->GetComponent<TransformComponent>();

		float rA = -1;
		float rB = -1;

		std::visit([&](auto&& s) {
			using T = std::decay_t<decltype(s)>;
			if constexpr (std::is_same_v<T, CircleShape>) {
				rA = s.radius;
			}
		}, rcA->currentShape);

		std::visit([&](auto&& s) {
			using T = std::decay_t<decltype(s)>;
			if constexpr (std::is_same_v<T, CircleShape>) {
				rB = s.radius;
			}
		}, rcB->currentShape);

		PhysicsBody bodyA = PhysicsBody();
		RigidBodyComponent* pcA = objA->GetComponent<RigidBodyComponent>();
		bodyA.obj = objA;

		if (tcA) {
			bodyA.position = &tcA->worldPosition;
			bodyA.transformMatrix = &tcA->WorldMatrix;
			bodyA.rotation = &tcA->rotation;
		}
		if (pcA) {
			bodyA.velocity = &pcA->velocity;
			bodyA.angularVelocity = &pcA->angularVelocity;
			bodyA.invInertia = &pcA->inverseInertia;
			bodyA.invMass = &pcA->inverseMass;
		}

		PhysicsBody bodyB = PhysicsBody();
		RigidBodyComponent* pcB = objB->GetComponent<RigidBodyComponent>();
		bodyB.obj = objB;

		if (tcB) {
			bodyB.position = &tcB->worldPosition;
			bodyB.transformMatrix = &tcB->WorldMatrix;
			bodyB.rotation = &tcB->rotation;
		}
		if (pcB) {
			bodyB.velocity = &pcB->velocity;
			bodyB.angularVelocity = &pcB->angularVelocity;
			bodyB.invInertia = &pcB->inverseInertia;
			bodyB.invMass = &pcB->inverseMass;
		}


		if (rA > 0.0f && rB > 0.0f && tcA->size.x == tcA->size.y && tcB->size.x == tcB->size.y) {
			glm::vec3 d = tcA->GetWorldPosition() - tcB->GetWorldPosition();
			float dist = glm::length(d);
			if (dist < rA * tcA->size.x + rB * tcB->size.x) {

				if (EngineManager::getInstance().debugMode) {
					objA->GetComponent<RenderComponent>()->color = glm::vec4(0, 1, 0, 1);
					objB->GetComponent<RenderComponent>()->color = glm::vec4(0, 1, 0, 1);
				}

				glm::vec3 normal = glm::normalize(d);
				float penetration = rA * tcA->size.x + rB * tcB->size.x - dist;
				glm::vec3 cPoint = tcA->GetWorldPosition() - normal * (rA * tcA->size.x - penetration / 2.0f);

				ContactPoint cp;
				cp.point = cPoint;
				cp.normal = normal;
				cp.penetration = penetration;
				cp.id = ContactID();
				allContactPoints.push_back(cp);
				
				ContactConstraint* constraint = new ContactConstraint(
					bodyA, bodyB, cPoint, cPoint, ContactID(), normal, penetration, 0.2f, 0.4f, 0.6f);
				RegisterConstraint(constraint);
			}
		}
		else if ((rA > 0.0f && tcA->size.x == tcA->size.y) || (rB > 0.0f && tcB->size.x == tcB->size.y)) {
			Object* circleObj = (rA > 0.0f) ? objA : objB;
			Object* polyObj = (rA > 0.0f) ? objB : objA;

			TransformComponent* tcCircle = circleObj->GetComponent<TransformComponent>();
			TransformComponent* tcPoly = polyObj->GetComponent<TransformComponent>();
			RenderComponent* rcCircle = circleObj->GetComponent<RenderComponent>();
			RenderComponent* rcPoly = polyObj->GetComponent<RenderComponent>();

			float radius = std::get<CircleShape>(rcCircle->currentShape).radius * tcCircle->size.x;
			glm::vec3 center = tcCircle->GetWorldPosition();

			std::vector<Edge> worldEdges;
			for (const auto& e : rcPoly->edges) {
				Edge we;
				we.start = tcPoly->ProjectToWorld(e.start);
				we.end = tcPoly->ProjectToWorld(e.end);
				worldEdges.push_back(we);
			}

			float    bestDist = INFINITY;
			glm::vec3 bestPoint = glm::vec3(0.0f);
			glm::vec3 bestNormal = glm::vec3(0.0f);
			bool     centerInside = true;

			for (const auto& edge : worldEdges) {
				glm::vec3 ab = edge.end - edge.start;
				glm::vec3 ac = center - edge.start;
				float     len = glm::length(ab);

				glm::vec3 abNorm = ab / len;
				float     t = glm::clamp(glm::dot(ac, abNorm), 0.0f, len);
				glm::vec3 closest = edge.start + abNorm * t;

				glm::vec3 edgeNormal = glm::normalize(glm::vec3(ab.y, -ab.x, 0.0f));

				if (glm::dot(edgeNormal, ac) > 0.0f) {
					centerInside = false;
				}

				float dist = glm::length(center - closest);
				if (dist < bestDist) {
					bestDist = dist;
					bestPoint = closest;
					bestNormal = edgeNormal;
				}
			}

			bool isColliding = false;
			glm::vec3 contactNormal;
			float     penetration;
			glm::vec3 contactPoint;

			if (centerInside) {
				contactNormal = bestNormal;
				if (glm::dot(contactNormal, center - tcPoly->GetWorldPosition()) < 0.0f)
					contactNormal = -contactNormal;
				penetration = radius + bestDist;
				contactPoint = bestPoint;
				isColliding = true;
			}
			else if (bestDist < radius) {
				glm::vec3 dir = center - bestPoint;
				contactNormal = (glm::length(dir) > 1e-6f)
					? glm::normalize(dir)
					: bestNormal;
				penetration = radius - bestDist;
				contactPoint = bestPoint;
				isColliding = true;
			}

			if (isColliding) {
				if (EngineManager::getInstance().debugMode) {
					circleObj->GetComponent<RenderComponent>()->color = glm::vec4(0, 1, 0, 1);
					polyObj->GetComponent<RenderComponent>()->color = glm::vec4(0, 1, 0, 1);
				}

				if (circleObj != objA) {
					contactNormal = -contactNormal;
				}

				ContactPoint cp;
				cp.point = contactPoint;
				cp.normal = contactNormal;
				cp.penetration = penetration;
				cp.id = ContactID();
				allContactPoints.push_back(cp);

				float cachedLambda = 0.0f;
				for (const auto& cached : contactsCache) {
					if (cached.objectA == objA && cached.objectB == objB) {
						cachedLambda = cached.lambda;
						break;
					}
				}

				ContactConstraint* constraint = new ContactConstraint(
					bodyA, bodyB,
					contactPoint, contactPoint,   
					ContactID(), contactNormal, penetration,
					0.2f, 0.4f, 0.6f);
				constraint->SetInitialImpulse(cachedLambda);
				RegisterConstraint(constraint);
			}
			else if (EngineManager::getInstance().debugMode) {
				rcA->color = glm::vec4(1, 0, 0, 1);
				rcB->color = glm::vec4(1, 0, 0, 1);
			}
		}
		else {

			CollisionData colData = SAT(objA, objB);

			if (colData.isColliding) {
				if (EngineManager::getInstance().debugMode) {
					objA->GetComponent<RenderComponent>()->color = glm::vec4(0, 1, 0, 1);
					objB->GetComponent<RenderComponent>()->color = glm::vec4(0, 1, 0, 1);
				}

				std::vector<ContactPoint> points = GenerateContactPoints(colData);

				for (int j = 0; j < points.size(); j++)
				{
					allContactPoints.push_back(points[j]);

					float cachedLambda = 0.0f;
					for (const auto& cached : contactsCache) {
						if (cached.objectA == objA && cached.objectB == objB &&
							cached.id.referenceEdgeA == points[j].id.referenceEdgeA &&
							cached.id.incidentEdgeB == points[j].id.incidentEdgeB &&
							cached.id.vertexTypeA == points[j].id.vertexTypeA &&
							cached.id.vertexTypeB == points[j].id.vertexTypeB)
						{
							cachedLambda = cached.lambda;
							break;
						}
					}

					ContactConstraint* constraint = new ContactConstraint(bodyA, bodyB, points[j].point, points[j].point, points[j].id, points[j].normal, points[j].penetration, 0.2f, 0.4f, 0.6f);
					constraint->SetInitialImpulse(cachedLambda);
					RegisterConstraint(constraint);
				}

			}
			else if (EngineManager::getInstance().debugMode) {
				objA->GetComponent<RenderComponent>()->color = glm::vec4(1, 0, 0, 1);
				objB->GetComponent<RenderComponent>()->color = glm::vec4(1, 0, 0, 1);
			}
		}
	}
}

std::vector<ContactPoint> PhysicsEngine::GenerateContactPoints(CollisionData collisionData) {
	std::vector<ContactPoint> ContactPoints = {};

	Edge referenceEdgeA = FindMostParallelEdge(collisionData.objAEdges, collisionData.normal);
	Edge referenceEdgeB = FindMostParallelEdge(collisionData.objBEdges, -collisionData.normal);
	Edge incidentEdgeA = FindMostAntiParallelEdge(collisionData.objAEdges, -collisionData.normal);
	Edge incidentEdgeB = FindMostAntiParallelEdge(collisionData.objBEdges, collisionData.normal);

	float scoreA = glm::dot(glm::vec3(glm::normalize(referenceEdgeA.end - referenceEdgeA.start).y, -glm::normalize(referenceEdgeA.end - referenceEdgeA.start).x, 0), collisionData.normal);
	float scoreB = glm::dot(glm::vec3(glm::normalize(referenceEdgeB.end - referenceEdgeB.start).y, -glm::normalize(referenceEdgeB.end - referenceEdgeB.start).x, 0), -collisionData.normal);

	Edge referenceEdge, incidentEdge;
	bool isA_Reference = true;
	int refEdgeIdx = 0;
	int incEdgeIdx = 0;

	if (scoreA > scoreB) {
		referenceEdge = referenceEdgeA;
		incidentEdge = incidentEdgeB;
		isA_Reference = true;
		for (size_t i = 0; i < collisionData.objAEdges.size(); ++i) if (collisionData.objAEdges[i].start == referenceEdge.start) refEdgeIdx = i;
		for (size_t i = 0; i < collisionData.objBEdges.size(); ++i) if (collisionData.objBEdges[i].start == incidentEdge.start) incEdgeIdx = i;
	}
	else {
		referenceEdge = referenceEdgeB;
		incidentEdge = incidentEdgeA;
		isA_Reference = false;
		for (size_t i = 0; i < collisionData.objBEdges.size(); ++i) if (collisionData.objBEdges[i].start == referenceEdge.start) refEdgeIdx = i;
		for (size_t i = 0; i < collisionData.objAEdges.size(); ++i) if (collisionData.objAEdges[i].start == incidentEdge.start) incEdgeIdx = i;
	}

	glm::vec3 v1 = referenceEdge.start;
	glm::vec3 v2 = referenceEdge.end;
	glm::vec3 tangent = glm::normalize(v2 - v1);

	ClipVertex incidentVertices[2];
	incidentVertices[0].position = incidentEdge.start;
	incidentVertices[1].position = incidentEdge.end;

	incidentVertices[0].id = { refEdgeIdx, incEdgeIdx, 0, 0 };
	incidentVertices[1].id = { refEdgeIdx, incEdgeIdx, 1, 0 };

	ClipVertex clipPoints1[2];
	glm::vec3 leftNormal = -tangent;
	float leftOffset = glm::dot(leftNormal, v1);
	int numPoints = ClipSegmentToLine(clipPoints1, incidentVertices, 2, leftNormal, leftOffset, refEdgeIdx, isA_Reference, 1);
	if (numPoints < 2) return ContactPoints;

	ClipVertex clipPoints2[2];
	glm::vec3 rightNormal = tangent;
	float rightOffset = glm::dot(rightNormal, v2);
	numPoints = ClipSegmentToLine(clipPoints2, clipPoints1, numPoints, rightNormal, rightOffset, refEdgeIdx, isA_Reference, 2);
	if (numPoints == 0) return ContactPoints;

	for (int i = 0; i < numPoints; i++) {
		float depth = 0.0f;

		if (isA_Reference) {
			depth = glm::dot(collisionData.normal, clipPoints2[i].position - referenceEdge.start);
		}
		else {
			depth = glm::dot(collisionData.normal, referenceEdge.start - clipPoints2[i].position);
		}

		if (depth >= 0.0f) {
			ContactPoint cp;
			cp.point = clipPoints2[i].position;
			cp.penetration = depth;
			cp.normal = collisionData.normal;
			cp.id = clipPoints2[i].id;
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

int PhysicsEngine::ClipSegmentToLine(ClipVertex vOut[2], const ClipVertex vIn[2], int numInPoints,
	const glm::vec3& normal, float offset, int referenceEdgeIndex, bool isA_Reference, int clipPlaneId) {
	int numOutPoints = 0;
	float d0 = glm::dot(normal, vIn[0].position) - offset;
	float d1 = glm::dot(normal, vIn[1].position) - offset;

	if (d0 <= 0.0f) vOut[numOutPoints++] = vIn[0];
	if (d1 <= 0.0f) vOut[numOutPoints++] = vIn[1];

	if ((d0 < 0.0f) != (d1 < 0.0f)) {
		float t = d0 / (d0 - d1);
		ClipVertex intersectionPoint;
		intersectionPoint.position = vIn[0].position + t * (vIn[1].position - vIn[0].position);

		intersectionPoint.id.referenceEdgeA = referenceEdgeIndex;
		intersectionPoint.id.incidentEdgeB = vIn[0].id.incidentEdgeB;
		intersectionPoint.id.vertexTypeA = vIn[0].id.vertexTypeA; 

		intersectionPoint.id.vertexTypeB = clipPlaneId;

		vOut[numOutPoints < 2 ? numOutPoints++ : 1] = intersectionPoint;
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

// Constraints

void PhysicsEngine::RegisterConstraint(Constraint* constraint) {
	registeredConstraints.push_back(constraint);
}

void PhysicsEngine::UpdateContactCache() {
	contactsCache.clear();

	for (auto* constraint : registeredConstraints) {
		if (constraint->isTemporary) {
			auto* contact = static_cast<ContactConstraint*>(constraint);

			ContactCache entry;
			entry.objectA = contact->objectA.obj; 
			entry.objectB = contact->objectB.obj;
			entry.id = contact->contactId;
			entry.lambda = contact->cacheLambda;

			contactsCache.push_back(entry);
		}
	}
}

void PhysicsEngine::UnRegisterTemporaryConstraint() {
	for (auto it = registeredConstraints.begin(); it != registeredConstraints.end(); ) {
		if ((*it)->isTemporary) {
			delete* it;
			it = registeredConstraints.erase(it);
		}
		else {
			++it;
		}
	}
}

void PhysicsEngine::UnRegisterConstraint(Constraint* constraint) {
	for (int i = 0; i < registeredConstraints.size(); i++)
	{
		if (registeredConstraints[i] == constraint) {
			registeredConstraints[i]->Unregister();
			registeredConstraints.erase(registeredConstraints.begin() + i);
		}
	}
}

void PhysicsEngine::ResolveConstraints(float delta) {
	std::vector<SolverRow> solverRows;
	solverRows.reserve(registeredConstraints.size() * 3);

	for (auto* constraint : registeredConstraints) {
		constraint->Prepare(solverRows, delta);
	}

	std::vector<int> sortedIndices(solverRows.size());
	std::iota(sortedIndices.begin(), sortedIndices.end(), 0);
	std::stable_sort(sortedIndices.begin(), sortedIndices.end(), [&](int a, int b) {
		return solverRows[a].bias > solverRows[b].bias;
		});

	for (int idx : sortedIndices) {
		auto& row = solverRows[idx];
		if (!row.warmStart || row.lambda == 0.0f) continue;
		if (row.objectA.velocity != nullptr && row.objectA.angularVelocity != nullptr) {
			*row.objectA.velocity += *row.objectA.invMass * row.jacobian.linearA * row.lambda;
			*row.objectA.angularVelocity += *row.objectA.invInertia * row.jacobian.angularA * row.lambda;
		}
		if (row.objectB.velocity != nullptr && row.objectB.angularVelocity != nullptr) {
			*row.objectB.velocity += *row.objectB.invMass * row.jacobian.linearB * row.lambda;
			*row.objectB.angularVelocity += *row.objectB.invInertia * row.jacobian.angularB * row.lambda;
		}
	}

	const int velocityIterations = 30;
	for (int i = 0; i < velocityIterations; i++) {
		for (int idx : sortedIndices) {
			auto& row = solverRows[idx];

			float relVel = 0.0f;
			if (row.objectA.velocity != nullptr && row.objectA.angularVelocity != nullptr) relVel += glm::dot(row.jacobian.linearA, *row.objectA.velocity)
				+ row.jacobian.angularA * *row.objectA.angularVelocity;
			if (row.objectB.velocity != nullptr && row.objectB.angularVelocity != nullptr) relVel += glm::dot(row.jacobian.linearB, *row.objectB.velocity)
				+ row.jacobian.angularB * *row.objectB.angularVelocity;

			float lambdaRaw = row.effectiveMass * (row.bias - relVel - row.softnessCFM * row.lambda);
			float lambdaOld = row.lambda;
			row.lambda += lambdaRaw;

			if (row.parentConstraint) {
				row.parentConstraint->PostIterationClamp(solverRows, idx, i);
			}

			float deltaLambda = row.lambda - lambdaOld;
			if (row.objectA.velocity != nullptr && row.objectA.angularVelocity != nullptr) {
				*row.objectA.velocity += *row.objectA.invMass * row.jacobian.linearA * deltaLambda;
				*row.objectA.angularVelocity += *row.objectA.invInertia * row.jacobian.angularA * deltaLambda;
			}
			if (row.objectB.velocity != nullptr && row.objectB.angularVelocity != nullptr) {
				*row.objectB.velocity += *row.objectB.invMass * row.jacobian.linearB * deltaLambda;
				*row.objectB.angularVelocity += *row.objectB.invInertia * row.jacobian.angularB * deltaLambda;
			}
		}
	}

	for (auto* constraint : registeredConstraints) {
		constraint->PostSolve(solverRows);
	}
}