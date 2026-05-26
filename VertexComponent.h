#pragma once
#include "Component.h"
#include "VertexPoint.h"
#include "RenderComponent.h"
#include "TransformComponent.h"
#include "InputManager.h"
#include <memory>

class Polygon;

class VertexComponent : public Component
{
public:
	VertexComponent(std::shared_ptr<Polygon> parent);
	VertexComponent() = default;

	static bool vertexSelected;

	std::shared_ptr<Polygon> parent;
	std::vector<VertexPoint> vertexPoints;
	int selectedIndex = -1;

	void SetEnabled(bool enabled) override;
	void SetVertexPoints(std::vector<VertexPoint> vertexPoints);
	void FindSelectedPoint(int button, int action, int mods);
	void DragPoint(double xpos, double ypos);
	void UpdateTransform();
};

