#pragma once
#include "Component.h"
#include "RenderComponent.h"
#include "TransformComponent.h"
#include "BoundingCircle.h"
#include "BAHNode.h"

class CollisionComponent : public Component
{
public:
	CollisionComponent(Object* parent);
	CollisionComponent() = default;

	BoundingCircle boundingCircle;
	BAHNode<BoundingCircle>* BAHnode;

	virtual void SetEnabled(bool enabled);
	virtual void OnDelete();
	virtual void ProcessInspectorUI();
	void calculateBoundingCircle();
};

