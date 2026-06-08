#pragma once
#include "SpringForce.h"
#include "ObjectLinkComponent.h"
#include "ObjectManager.h"
class SpringComponent : public ObjectLinkComponent
{
public:
	SpringComponent(Object* parent, float springConstant, float damping, float restLength);
	SpringComponent() = default;

	SpringForce* springForceTop = nullptr;
	SpringForce* springForceBot = nullptr;

	float springConstant;
	float damping;
	float restLength;

	virtual void ProcessInspectorUI();
	virtual void AddTopObject(Object* obj);
	virtual void AddBottomObject(Object* obj);
	virtual void RemoveTopObject();
	virtual void RemoveBottomObject();
};

