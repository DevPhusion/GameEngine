#pragma once
#include "Component.h"
#include "InputManager.h"
#include "Object.h"
#include "TransformComponent.h"
#include "RenderComponent.h"
#include "EngineManager.h"
#include "MouseDrag.h"
#include "PhysicsEngine.h"

class MouseInteractComponent : public Component
{
public:
	MouseInteractComponent(Object parent, bool physicsInteract);
	MouseInteractComponent() = default;

	Object parent;
	bool physicsInteract;
	bool Selected;

	MouseDrag* mouseDragForce = nullptr;

	void FindSelectedPolygon(int button, int action, int mods);
	void DragPolygon(double xpos, double ypos);
};

