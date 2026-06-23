#pragma once
#include "Constraint.h"
class DistanceConstraint : public Constraint
{
public:
	DistanceConstraint() = default;
	DistanceConstraint(Object* objectA, Object* objectB, glm::vec3 attachPointA, glm::vec3 attachPointB, 
		float distance, float stiffness = 0.0f, float damping = 0.0f, 
		bool extendable = false, bool retractable = false);

	float distance;
	float stiffness;
	float damping;
	bool extendable;
	bool retractable;

	virtual void Prepare(std::vector<SolverRow>& rows, float delta);
};

