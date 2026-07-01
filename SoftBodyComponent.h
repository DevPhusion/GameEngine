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

	std::vector<std::unique_ptr<PointMass>> MassAggregate = {};
	std::vector<SoftBodySpringConstraint*> springs = {};

	PointMass* CenterPM = nullptr;

	bool isDragging;

	float inverseMass = 1.0f;

	float stiffness = 0.5f;
	float damping = 0.001f;

	void ProcessSoftBody(float delta);
	void IntegrateVelocities(float delta);
	void IntegratePositions(float delta);
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

