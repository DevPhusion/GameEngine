#pragma once
#include "Polygon.h"
#include "VertexPoint.h"
#include "InputManager.h"
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
	void RemoveObject(Object* obj);

	void ProcessObjects(float delta);

private:
	ObjectManager() = default;
};

