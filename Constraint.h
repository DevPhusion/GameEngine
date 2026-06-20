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
	
	float effectiveMass;
	float bias;
	float lambda;
	float softnessCFM;

	float minLambda;
	float maxLambda;

	Object* objectA;
	Object* objectB;
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

	float beta = 0.2f; //Baumgarte bias tuning

	virtual SolverRow Prepare(float delta) = 0;
};

