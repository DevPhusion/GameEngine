#pragma once
#include "Component.h"
#include "ObjectManager.h"
#include "SpringForce.h"
class SpringComponent : public Component
{
public:
	SpringComponent(Object* parent, float springConstant, float damping, float restLength);
	SpringComponent() = default;

	Object* topObject = nullptr;
	Object* bottomObject = nullptr;

	SpringForce* springForceTop = nullptr;
	SpringForce* springForceBot = nullptr;

	float springConstant;
	float damping;
	float restLength;

	virtual void OnDelete();
	virtual void ProcessInspectorUI();
private:
	int TopObjectOnDeleteID;
	int BotObjectOnDeleteID;

	void AddTopObject(Object* obj);
	void AddBottomObject(Object* obj);
	void RemoveTopObject(Object* obj);
	void RemoveBottomObject(Object* obj);
};

