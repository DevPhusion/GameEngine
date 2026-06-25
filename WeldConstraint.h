#pragma once
#include "Constraint.h"
class WeldConstraint : public Constraint
{
public:
	WeldConstraint(Object* objectA, Object* objectB, glm::vec3 attachPointA, glm::vec3 attachPointB, float angularOffset);
	WeldConstraint() = default;

	float angularOffset;
	float cacheLambda[3] = { 0.0f, 0.0f, 0.0f };

	int xRowOffset = 0;
	int yRowOffset = 0;
	int thetaRowOffset = 0;

	virtual void SetObjectA(Object* obj);
	virtual void SetObjectB(Object* obj);

	virtual void Prepare(std::vector<SolverRow>& rows, float delta);
	virtual void PostSolve(std::vector<SolverRow>& allRows);
	virtual void ProcessInspectorUI(Object* parent);
};

