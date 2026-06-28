#pragma once
#include "Object.h"
#include "PhysicsComponent.h"
#include "CollisionComponent.h"
#include "ForceGenerator.h"
#include "BAHNode.h"
#include "DebugPoint.h"
#include "Constraint.h"
#include "ContactConstraint.h"
#include "ContactID.h"
#include <numeric>

// Sutherland Hodgman

struct ClipVertex {
	ContactID id;
	glm::vec3 position;
};

struct ContactPoint {
	ContactID id;
	glm::vec3 normal;
	glm::vec3 point;
	float penetration;
};

struct CollisionData {
	bool isColliding = false;
	float penetration = 0.0f;
	glm::vec3 normal = glm::vec3(0.0f);
	std::vector<Edge> objAEdges;
	std::vector<Edge> objBEdges;
};

// SAT

struct SeparatingAxis {
	glm::vec3 normal;
	glm::vec3 start;
	glm::vec3 end;
};

struct Projection {
	float min;
	float max;

	bool Overlaps(const Projection& other) const {
		return !(this->max < other.min || other.max < this->min);
	}
};

//GJK

struct Simplex {
	std::array<glm::vec3, 3> points;
	int size = 0;

	void Add(const glm::vec3& p) {
		for (int i = std::min(size, 2); i > 0; i--)
			points[i] = points[i - 1];
		points[0] = p;
		size = std::min(size + 1, 3);
	}

	void Set(glm::vec3 a) { points[0] = a; size = 1; }
	void Set(glm::vec3 a, glm::vec3 b) { points[0] = a; points[1] = b; size = 2; }
	void Set(glm::vec3 a, glm::vec3 b, glm::vec3 c) { points[0] = a; points[1] = b; points[2] = c; size = 3; }
};

struct GJKResult {
	bool isColliding;
	Simplex simplex;
};

struct EPAResult {
	glm::vec3 normal = glm::vec3(0);
	float penetration = 0.0f;
	glm::vec3 contactPoint = glm::vec3(0);
	glm::vec3 witnessA = glm::vec3(0); 
	glm::vec3 witnessB = glm::vec3(0);
};

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

	//Collision detection
	std::vector<ContactPoint> allContactPoints;
	BAHNode<BoundingCircle> root;
	BAHNode<BoundingCircle>* RegisterBoundingAreaNode(Object* obj, BoundingCircle boundingCircle);
	void UnRegisterBoundingAreaNode(Object* obj);
	void ResolveContacts(PotentialContact* contacts, unsigned numContacts);
	CollisionData SAT(Object* objA, Object* objB);
	std::vector<ContactPoint> GenerateContactPoints(CollisionData collisionData);

	Projection ProjectOntoAxis(std::vector<glm::vec3>& vertices, SeparatingAxis axis);
	float ComputeSignedArea(const std::vector<glm::vec3>& vertices);
	Edge FindMostParallelEdge(const std::vector<Edge>& edges, const glm::vec3& normal);
	Edge FindMostAntiParallelEdge(const std::vector<Edge>& edges, const glm::vec3& normal);
	int ClipSegmentToLine(ClipVertex vOut[2], const ClipVertex vIn[2], int numInPoints,
		const glm::vec3& normal, float offset, int referenceEdgeIndex, bool isA_Reference, int clipPlaneId);

	//Constraint resolution
	std::vector<ContactCache> contactsCache;
	std::vector<Constraint*> registeredConstraints;
	void UpdateContactCache();
	void UnRegisterTemporaryConstraint();
	void RegisterConstraint(Constraint* constraint);
	void UnRegisterConstraint(Constraint* constraint);
	void ResolveConstraints(float delta);
private:
	PhysicsEngine() = default;
	std::vector<std::unique_ptr<Object>>* allObjects;
};

