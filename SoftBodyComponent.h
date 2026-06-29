#pragma once
#include "Component.h"
#include "PointMass.h"
#include "SpringConstraint.h"
#include <variant>
class SoftBodyComponent : public Component
{
public:
	SoftBodyComponent(Object* parent);
	SoftBodyComponent() = default;

	std::vector<PointMass*> MassAggregate = {};
	std::vector<SpringConstraint*> springs = {};

	float mass;
	float acceleration;

	float stiffness = 500.0f;
	float damping = 10.0f;

	void BuildMassAggregate();
	void UpdateMassAggregate();
	void UpdatePoint(int index);

	virtual void ProcessInspectorUI();
	virtual void OnDelete();
	virtual void CopyTo(Object* other);
private:
	bool updatingFromParent = false;
	int transformCallbackID;
};

