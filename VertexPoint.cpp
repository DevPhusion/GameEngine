#include "VertexPoint.h"

VertexPoint::VertexPoint(float x, float y, Shader shader) : Object(shader) {
	this->x = x;
	this->y = y;

	float sizeY = 0.01f;
	float sizeX = 0.01f;

	std::vector<float> vertices = {
		x - sizeX, y - sizeY, 0.0f, 0.0f, 0.0f,
		x + sizeX, y - sizeY, 0.0f, 1.0f, 0.0f,
		x + sizeX, y + sizeY, 0.0f, 1.0f, 1.0f,
		x - sizeX, y + sizeY, 0.0f, 0.0f, 1.0f
	};

	AddComponent(std::make_unique<RenderComponent>(this, vertices, shader, std::vector<std::string>{"floorTiled.png"}));
	AddComponent(std::make_unique<TransformComponent>(this, shader, glm::vec3(0)));
}

void VertexPoint::Process(float delta) {

}

void VertexPoint::UpdatePosition(float x, float y) {
	this->x = x;
	this->y = y;

	float sizeY = 0.01f;
	float sizeX = 0.01f;

	std::vector<float> vertices = {
		x - sizeX, y - sizeY, 0.0f, 0.0f, 0.0f,
		x + sizeX, y - sizeY, 0.0f, 1.0f, 0.0f,
		x + sizeX, y + sizeY, 0.0f, 1.0f, 1.0f,
		x - sizeX, y + sizeY, 0.0f, 0.0f, 1.0f
	};

	GetComponent<RenderComponent>()->UpdateShape(vertices, GetComponent<RenderComponent>()->Indices);
}