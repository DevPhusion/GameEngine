#pragma once
#include "Component.h"
#include "PointMass.h"
#include "SoftBodySpringConstraint.h"
#include <variant>
class SoftBodyComponent : public ComponentBase<SoftBodyComponent>
{
public:
	SoftBodyComponent(Object* parent);
	SoftBodyComponent() = default;

	std::vector<PointMass*> MassAggregate = {};
	std::vector<SoftBodySpringConstraint*> springs = {};

	float mass;
	float acceleration;

	float stiffness = 50.0f;
	float damping = 10.0f;

	void ProcessSoftBody(float delta);
	void BuildMassAggregate();
	void UpdateMassAggregate();
	void SyncMeshFromMassAggregate();

	virtual void ProcessInspectorUI();
	virtual void OnDelete();
	virtual void CopyTo(Object* other);
private:
	bool updatingFromPoints = false;
	bool updatingFromParent = false;
	int transformCallbackID;
};

