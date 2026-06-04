#pragma once
#include "Polygon.h"
#include "VertexPoint.h"
#include "Spring.h"
#include "InputManager.h"
#include "Gravity.h"
#include "Drag.h"
#include "SpringForce.h"
class ObjectManager
{
public:
	ObjectManager(const ObjectManager&) = delete;
	void operator=(const ObjectManager&) = delete;

	static ObjectManager& getInstance() {
		static ObjectManager instance;
		return instance;
	}

	std::vector<std::unique_ptr<Object>> allObjects;
	std::vector<VertexPoint*> vertexPoints;
	std::vector<float> vertices;

	void AddObject();
	void AddPolygon();
	void AddPolygonVertex();
	void AddSpring();

	void ProcessObjects(float delta);

private:
	ObjectManager() = default;
};

