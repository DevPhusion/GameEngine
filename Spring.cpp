#include "Spring.h"

Spring::Spring(Shader shader, float springConstant, float damping, float restLength) : Object(shader) {
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
	AddComponent(std::make_unique<SpringComponent>(this, springConstant, damping, restLength));
	GetComponent<RenderComponent>()->SetEnabled(false);
}

void Spring::Process(float delta) {
	UpdateVertex();
}

void generateSegment(glm::vec2 start, glm::vec2 end, float thickness,
	std::vector<float>& vertices, std::vector<unsigned int>& indices) {
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
	//topVert = topObject->GetComponent<TransformComponent>()->ProjectToWorld(topPos, true);
	topVert = GetComponent<TransformComponent>()->GetTransformedPoint(top, true);
	glm::vec3 botPos = bottomObject->GetComponent<TransformComponent>()->GetWorldPosition();
	//Project world -> screen
	glm::vec3 bot = bottomObject->GetComponent<TransformComponent>()->GetTransformedPoint(bottomObject->GetComponent<TransformComponent>()->ProjectToWorld(botPos, true));
	botVert = GetComponent<TransformComponent>()->GetTransformedPoint(bot, true);
	std::vector<float> vertices = {};

	std::vector<unsigned int> indices = {};

	float thickness = 0.01f;
	
	glm::vec2 dir = botVert - topVert;

	float length = glm::length(dir);
	if (length < 0.0001f) return;

	float nx = -dir.y / length;
	float ny = dir.x / length;

	float halfThickness = thickness * 0.5f;
	float offsetX = nx * halfThickness;
	float offsetY = ny * halfThickness;

	unsigned int vertexOffset = vertices.size() / 5;

	vertices.insert(vertices.end(), { topVert.x + offsetX, topVert.y + offsetY, 0.0f, 0.0f, 0.0f });
	vertices.insert(vertices.end(), { botVert.x + offsetX, botVert.y + offsetY,   0.0f, 1.0f, 0.0f });
	vertices.insert(vertices.end(), { topVert.x - offsetX, topVert.y - offsetY, 0.0f, 0.0f, 1.0f });
	vertices.insert(vertices.end(), { botVert.x - offsetX, botVert.y - offsetY,   0.0f, 1.0f, 1.0f });

	indices.push_back(vertexOffset + 0);
	indices.push_back(vertexOffset + 2);
	indices.push_back(vertexOffset + 1);

	indices.push_back(vertexOffset + 1);
	indices.push_back(vertexOffset + 2);
	indices.push_back(vertexOffset + 3);


	GetComponent<RenderComponent>()->UpdateShape(vertices, indices);

	if (topObject != nullptr && bottomObject != nullptr) {
		GetComponent<RenderComponent>()->SetEnabled(true);
	}
	else {
		GetComponent<RenderComponent>()->SetEnabled(false);
	}
}