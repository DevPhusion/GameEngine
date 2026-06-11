#pragma once
#include "Component.h"
#include <glm/glm.hpp>
class ObjectLinkComponent : public Component
{
public:
	ObjectLinkComponent(Object* parent);
	ObjectLinkComponent() = default;

	Object* topObject = nullptr;
	Object* bottomObject = nullptr;

	Object* topConnectDisplay = nullptr;
	Object* bottomConnectDisplay = nullptr;

	glm::vec3 topConnectPoint = glm::vec3(0);
	glm::vec3 bottomConnectPoint = glm::vec3(0);

	virtual void OnDelete();
	virtual void AddTopObject(Object* obj);
	virtual void AddBottomObject(Object* obj);
	virtual void RemoveTopObject();
	virtual void RemoveBottomObject();
	virtual void OnTopObjectUpdatePos();
	virtual void OnBottomObjectUpdatePos();
	virtual void OnTopDisplayUpdatePos();
	virtual void OnBottomDisplayUpdatePos();

	void ObjectSelectUI();
	void OnPhysicsModeChange();
private:
	bool TopUseCenter = true;
	bool BotUseCenter = true;
	bool PosSetTop = false;
	bool PosSetBot = false;
	int TopObjectOnTransformID;
	int BotObjectOnTransformID;
	int TopObjectOnDeleteID;
	int BotObjectOnDeleteID;
};

