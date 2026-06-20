#pragma once
#include "DistanceConstraint.h"
#include "ObjectLinkComponent.h"
#include "ObjectManager.h"
class SpringComponent : public ObjectLinkComponent
{
public:
	SpringComponent(Object* parent, float springConstant, float damping, float restLength);
	SpringComponent() = default;

	DistanceConstraint* constraint = nullptr;

	float springConstant;
	float damping;
	float restLength;

	virtual void ProcessInspectorUI();
	virtual void OnDelete();
	virtual void AddTopObject(Object* obj);
	virtual void AddBottomObject(Object* obj);
	virtual void RemoveTopObject();
	virtual void RemoveBottomObject();
	virtual void OnTopDisplayUpdatePos();
	virtual void OnBottomDisplayUpdatePos();
};

