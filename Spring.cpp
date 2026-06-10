#include "Spring.h"

Spring::Spring(Shader shader, float springConstant, float damping, float restLength, float angularSpringConstant, float angularDamping, float restAngle) : Object(shader) {
	float sizeY = 0.01f;
	float sizeX = 0.01f;

	std::vector<float> vertices = {
		-sizeX, -sizeY, 0.0f, 0.0f, 0.0f,
		+sizeX, -sizeY, 0.0f, 1.0f, 0.0f,
		+sizeX, +sizeY, 0.0f, 1.0f, 1.0f,
		-sizeX, +sizeY, 0.0f, 0.0f, 1.0f
	};

	AddComponent(std::make_unique<RenderComponent>(this, vertices, shader, "floorTiled.png"));
	AddComponent(std::make_unique<TransformComponent>(this, shader, GetComponent<RenderComponent>()->GetCenter()));
	AddComponent(std::make_unique<SpringComponent>(this, springConstant, damping, restLength, angularSpringConstant, angularDamping, restAngle));
	GetComponent<RenderComponent>()->SetEnabled(false);
	GetComponent<RenderComponent>()->z_index = -999;
}

void Spring::Process(float delta) {
	UpdateVertex();
}

void Spring::GenerateSegment(glm::vec2 start, glm::vec2 end, float thickness,
	std::vector<float>& vertices, std::vector<unsigned int>& indices) {

	glm::vec2 dir = end - start;

	float length = glm::length(dir);
	if (length < 0.0001f) return;

	float nx = -dir.y / length;
	float ny = dir.x / length;

	float halfThickness = thickness * 0.5f;
	float offsetX = nx * halfThickness;
	float offsetY = ny * halfThickness;

	unsigned int vertexOffset = vertices.size() / 5;

	vertices.insert(vertices.end(), { start.x + offsetX, start.y + offsetY, 0.0f, 0.0f, 0.0f });
	vertices.insert(vertices.end(), { end.x + offsetX, end.y + offsetY,   0.0f, 1.0f, 0.0f });
	vertices.insert(vertices.end(), { start.x - offsetX, start.y - offsetY, 0.0f, 0.0f, 1.0f });
	vertices.insert(vertices.end(), { end.x - offsetX, end.y - offsetY,   0.0f, 1.0f, 1.0f });

	indices.push_back(vertexOffset + 0);
	indices.push_back(vertexOffset + 2);
	indices.push_back(vertexOffset + 1);

	indices.push_back(vertexOffset + 1);
	indices.push_back(vertexOffset + 2);
	indices.push_back(vertexOffset + 3);
}

void Spring::UpdateVertex() {
	Object* topObject = GetComponent<SpringComponent>()->topObject;
	Object* bottomObject = GetComponent<SpringComponent>()->bottomObject;

	if (topObject == nullptr || bottomObject == nullptr) {
		GetComponent<RenderComponent>()->SetEnabled(false);
		return;
	}

	glm::vec2 topVert = glm::vec2(0);
	glm::vec2 botVert = glm::vec2(0);

	glm::vec3 topPos = topObject->GetComponent<TransformComponent>()->GetWorldPosition();
	// Project world -> screen
	glm::vec3 top = topObject->GetComponent<TransformComponent>()->GetTransformedPoint(topObject->GetComponent<TransformComponent>()->ProjectToWorld(topPos, true));
	topVert = GetComponent<TransformComponent>()->GetTransformedPoint(top, true);
	glm::vec3 botPos = bottomObject->GetComponent<TransformComponent>()->GetWorldPosition();
	//Project world -> screen
	glm::vec3 bot = bottomObject->GetComponent<TransformComponent>()->GetTransformedPoint(bottomObject->GetComponent<TransformComponent>()->ProjectToWorld(botPos, true));
	botVert = GetComponent<TransformComponent>()->GetTransformedPoint(bot, true);
	std::vector<float> vertices = {};

	std::vector<unsigned int> indices = {};

	int segmentsCount = 10;
	float thickness = 0.01f;
	float amplitude = 0.05f;


	glm::vec2 axis = botVert - topVert;
	float totalLength = glm::length(axis);

	if (totalLength > 0.0001f) {
		glm::vec2 dir = axis / totalLength;

		glm::vec2 perp = glm::vec2(-dir.y, dir.x);

		std::vector<glm::vec2> springPoints;
		springPoints.push_back(topVert);

		for (int i = 1; i < segmentsCount; i++) {
			float t = (float)i / (float)segmentsCount;

			glm::vec2 basePoint = topVert + dir * (totalLength * t);

			float sideSign = (i % 2 == 1) ? 1.0f : -1.0f;

			glm::vec2 zigZagPoint = basePoint + perp * (amplitude * sideSign);
			springPoints.push_back(zigZagPoint);
		}

		springPoints.push_back(botVert);

		for (size_t i = 0; i < springPoints.size() - 1; i++) {
			GenerateSegment(springPoints[i], springPoints[i + 1], thickness, vertices, indices);
		}
	}
	

	GetComponent<RenderComponent>()->UpdateShape(vertices, indices);

	if (topObject != nullptr && bottomObject != nullptr) {
		GetComponent<RenderComponent>()->SetEnabled(true);
	}
	else {
		GetComponent<RenderComponent>()->SetEnabled(false);
	}
}