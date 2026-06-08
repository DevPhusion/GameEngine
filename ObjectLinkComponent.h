#pragma once
#include "Component.h"
class ObjectLinkComponent : public Component
{
public:
	ObjectLinkComponent(Object* parent);
	ObjectLinkComponent() = default;

	Object* topObject = nullptr;
	Object* bottomObject = nullptr;

	virtual void OnDelete();
	virtual void AddTopObject(Object* obj);
	virtual void AddBottomObject(Object* obj);
	virtual void RemoveTopObject();
	virtual void RemoveBottomObject();

	void ObjectSelectUI();
private:
	int TopObjectOnDeleteID;
	int BotObjectOnDeleteID;
};

