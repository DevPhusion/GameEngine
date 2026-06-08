#pragma once
#include "ObjectLinkComponent.h"
#include "Contact.h"
#include "PhysicsEngine.h"
class CableComponent : public ObjectLinkComponent
{
public:
	CableComponent(Object* parent, float maxLength, float restitution);
	CableComponent() = default;
	
	float maxLength;
	float restitution;

	virtual void ProcessInspectorUI();
	
	Contact* CableContact;
	void FillContact();
};

