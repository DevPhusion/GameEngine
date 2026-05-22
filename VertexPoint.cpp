#include "VertexPoint.h"

VertexPoint::VertexPoint(float x, float y, Shader shader, int pointIndex, Renderer* renderer) {
	this->x = x;
	this->y = y;
	this->pointIndex = pointIndex;

	this->pointIndicator = Polygon(std::vector<float> {
		x - 0.01f, y - 0.01f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		x + 0.01f, y - 0.01f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		x + 0.01f, y + 0.01f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		x - 0.01f, y + 0.01f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f
	}, shader, std::vector<std::string> {});

	renderer->AddPolygon(pointIndicator);
}

void VertexPoint::UpdatePosition(float x, float y) {
	this->x = x;
	this->y = y;
	std::vector<float> newVertices = parentPolygon->GetVertices();
	newVertices[pointIndex * 8] = x;
	newVertices[pointIndex * 8 + 1] = y;
	this->parentPolygon->SetVertices(newVertices);

	this->pointIndicator.SetVertices(std::vector<float> {
		x - 0.01f, y - 0.01f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		x + 0.01f, y - 0.01f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		x + 0.01f, y + 0.01f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		x - 0.01f, y + 0.01f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f
	});
}