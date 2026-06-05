#pragma once
#include "Component.h"
#include "InputManager.h"
#include "Object.h"
#include "TransformComponent.h"
#include "RenderComponent.h"
#include "EngineManager.h"
#include "VertexComponent.h"
#include "MouseDrag.h"
#include "PhysicsEngine.h"

class MouseInteractComponent : public Component
{
public:
	MouseInteractComponent(Object* parent, bool physicsInteract);
	MouseInteractComponent() = default;

	static bool ObjectSelected; //prevent multiple selection;
	bool physicsInteract;
	bool Selected;

	MouseDrag* mouseDragForce = nullptr;


	virtual void ProcessInspectorUI();
	virtual void OnDelete();
	void FindSelectedPolygon(int button, int action, int mods);
	void DragPolygon(double xpos, double ypos);
	void SetSelectedPolygon(Object* obj, bool enable);
	void OnPhysicsModeChanged();
private:
	int mouseButtonCallbackID;
	int cursorPosCallbackID;
	int physicsModeChangedCallbackID;
};

