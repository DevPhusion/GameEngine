#pragma once
#include "ObjectLinkComponent.h"
#include "Contact.h"
#include "PhysicsEngine.h"
class CableComponent : public ObjectLinkComponent
{
public:
	CableComponent(Object* parent, float maxLength, float restitution, bool retractable);
	CableComponent() = default;
	
	float maxLength;
	float restitution;
	bool retractable;

	virtual void ProcessInspectorUI();
	
	Contact* CableContact;
	void FillContact();
};

