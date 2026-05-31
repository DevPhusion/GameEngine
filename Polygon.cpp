#include "Polygon.h"

Polygon::Polygon(std::vector<float> vertices, Shader shader, std::vector<std::string> textures) : Object(shader) {
	AddComponent(std::make_unique<RenderComponent>(this, vertices, shader, textures));
	auto* render = GetComponent<RenderComponent>();
	AddComponent(std::make_unique<TransformComponent>(this, shader, render->GetCenter()));
	AddComponent(std::make_unique<VertexComponent>(this));  
	AddComponent(std::make_unique<MouseInteractComponent>(this, true));
	AddComponent(std::make_unique<PhysicsComponent>(this));
}
