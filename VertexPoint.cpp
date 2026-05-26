#include "VertexPoint.h"

VertexPoint::VertexPoint(float x, float y, Shader shader) : Object(shader) {
	this->x = x;
	this->y = y;

	std::vector<float> vertices = {
		x - 0.01f, y - 0.01f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		x + 0.01f, y - 0.01f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		x + 0.01f, y + 0.01f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		x - 0.01f, y + 0.01f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f
	};

	std::vector<unsigned int> indices = {
		0, 1, 2,
		2, 3, 0
	};

	AddComponent(new RenderComponent(vertices, indices, shader, std::vector<std::string>{}));
	AddComponent(new TransformComponent(shader, glm::vec3(0)));
	GetComponent<TransformComponent>()->SetEnabled(false);
}

void VertexPoint::UpdatePosition(float x, float y) {
	this->x = x;
	this->y = y;

	std::vector<float> vertices = {
		x - 0.01f, y - 0.01f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		x + 0.01f, y - 0.01f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		x + 0.01f, y + 0.01f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		x - 0.01f, y + 0.01f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f
	};

	std::vector<unsigned int> indices = {
		0, 1, 2,
		2, 3, 0
	};

	GetComponent<RenderComponent>()->UpdateShape(vertices, indices);
}