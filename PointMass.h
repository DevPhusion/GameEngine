#pragma once
#include "Object.h"
#include "RenderComponent.h"
#include "TransformComponent.h"
#include "RigidBodyComponent.h"
#include "CollisionComponent.h"

class SoftBodyComponent;

class PointMass : public Object
{
public:
	PointMass(Shader shader, SoftBodyComponent* sb, glm::vec3 point, int index, bool isCenter);
	PointMass() = default;

	SoftBodyComponent* sb;

	int index;
	bool isCenter;

	virtual void Process(float delta);
	void UpdateParent();
};

