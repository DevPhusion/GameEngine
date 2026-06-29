#include "PointMass.h"
#include "SoftBodyComponent.h"

PointMass::PointMass(Shader shader, SoftBodyComponent* sb, glm::vec3 point, int index, bool isCenter) : Object(shader) {
	this->sb = sb;
	this->index = index;
	this->isCenter = isCenter;

	AddComponent(std::make_unique<RenderComponent>(this, std::vector<float> {}, shader, "red.jpg"));
	RenderComponent* rc = GetComponent<RenderComponent>();
	AddComponent(std::make_unique<TransformComponent>(this, shader, rc->GetCenter()));
	TransformComponent* tc = GetComponent<TransformComponent>();
	AddComponent(std::make_unique<RigidBodyComponent>(this));
	AddComponent(std::make_unique<CollisionComponent>(this));
	
	tc->AddTransformCallback([this] {UpdateParent();});
	rc->z_index = 999;
	RectangleShape shape = RectangleShape();
	shape.center = tc->GetWorldPosition();
	shape.width = 0.1f;
	shape.height = 0.1f;
	rc->SetShape(shape);
	tc->UpdateWorldPosition(point);
	rc->SetEnabled(false);
	hidden = true;
}

void PointMass::Process(float delta) {
	if (EngineManager::getInstance().debugMode) {
		GetComponent<RenderComponent>()->SetEnabled(true);
	}
	else {
		GetComponent<RenderComponent>()->SetEnabled(false);
	}
}

void PointMass::UpdateParent() {
	sb->UpdatePoint(index);
}