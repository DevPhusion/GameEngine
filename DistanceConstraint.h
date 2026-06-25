#pragma once
#include "Constraint.h"
class DistanceConstraint : public Constraint
{
public:
	DistanceConstraint() = default;
	DistanceConstraint(Object* objectA, Object* objectB, glm::vec3 attachPointA, glm::vec3 attachPointB, 
		float distance, bool extendable = false, bool retractable = false);

	float distance;
	bool extendable;
	bool retractable;

	virtual void ProcessConstraintDisplay();
	virtual void Prepare(std::vector<SolverRow>& rows, float delta);
	virtual void ProcessInspectorUI(Object* parent);
};

