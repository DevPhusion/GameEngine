#pragma once
#include "Constraint.h"
#include "ContactID.h"

class ContactConstraint : public Constraint
{
public:
	ContactConstraint(Object* objectA, Object* objectB, glm::vec3 attachPointA, glm::vec3 attachPointB, ContactID id,
		glm::vec3 normal, float penetration, float restitution, float staticFriction, float dynamicFriction);
	ContactConstraint() = default;

	ContactID contactId;

	glm::vec3 normal;
	float penetration;
	float restitution;
	float staticFriction;
	float dynamicFriction;

	int normalRowOffsetIndex = 0;
	int frictionRowOffsetIndex = -1;
	float bounceThreshold = 0.2f;

	virtual void Prepare(std::vector<SolverRow>& rows, float delta);
	virtual void PostIterationClamp(std::vector<SolverRow>& allRows, int myRowIndex, int velocityIteration);
	virtual void PostSolve(std::vector<SolverRow>& allRows);
};

