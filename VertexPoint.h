#pragma once
#include <memory>
#include "Polygon.h"
#include "Renderer.h"
class VertexPoint
{

public:
	float x;
	float y;
	std::shared_ptr<Polygon> parentPolygon;
	VertexPoint(float x, float y, Shader shader, int pointIndex, Renderer* renderer);
	VertexPoint() = default;
	void SetParentPolygon(std::shared_ptr<Polygon> parentPolygon) {
		this->parentPolygon = parentPolygon;
	}
	void UpdatePosition(float x, float y);
private:
	Polygon pointIndicator;
	unsigned int pointIndex;
};