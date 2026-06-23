#pragma once
#include "Object.h"
#include "TransformComponent.h"
#include "PhysicsComponent.h"

struct JacobianRow {
	glm::vec3 linearA;
	float angularA;
	glm::vec3 linearB;
	float angularB;
};

struct SolverRow {
	JacobianRow jacobian;
	
	float effectiveMass = 0.0f;
	float bias = 0.0f;
	float lambda = 0.0f;
	float softnessCFM = 0.0f;

	float minLambda = -INFINITY;
	float maxLambda = INFINITY;

	Object* objectA = nullptr;
	Object* objectB = nullptr;

	bool warmStart = true;
	class Constraint* parentConstraint = nullptr; 
};

class Constraint
{
public:
	Constraint(Object* objectA, Object* objectB, glm::vec3 attachPointA, glm::vec3 attachPointB);
	Constraint() = default;

	Object* objectA;
	Object* objectB;
	glm::vec3 attachPointA;
	glm::vec3 attachPointB;

	bool isTemporary = false;
	float cacheLambda = 0.0f;
	float beta = 0.2f; //Baumgarte bias tuning

	void SetInitialImpulse(float lambda) { cacheLambda = lambda; }
	virtual void Prepare(std::vector<SolverRow>& rows, float delta) = 0;
	virtual void PostIterationClamp(std::vector<SolverRow>& allRows, int myRowIndex, int velocityIteration) {
		allRows[myRowIndex].lambda = glm::clamp(allRows[myRowIndex].lambda, allRows[myRowIndex].minLambda, allRows[myRowIndex].maxLambda);
	}
	virtual void PostSolve(std::vector<SolverRow>& allRows) {
		return;
	}
};

