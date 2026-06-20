#pragma once
#include "ObjectLinkComponent.h"
#include "Contact.h"
#include "PhysicsEngine.h"
#include "DistanceConstraint.h"
class CableComponent : public ObjectLinkComponent
{
public:
	CableComponent(Object* parent, float maxLength, float restitution, bool retractable);
	CableComponent() = default;
	
	float maxLength;
	bool retractable;

	DistanceConstraint* constraint = nullptr;
	
	virtual void OnDelete();
	virtual void ProcessInspectorUI();
	
	Contact* CableContact;
	void FillContact();
};

