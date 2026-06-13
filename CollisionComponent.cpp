#include "CollisionComponent.h"
#include "PhysicsEngine.h"

CollisionComponent::CollisionComponent(Object* parent) : Component(parent) {
	Name = "Collision Component";
	calculateBoundingCircle();
	parent->GetComponent<TransformComponent>()->AddTransformCallback([this]() {this->calculateBoundingCircle();});
	BAHnode = PhysicsEngine::getInstance().RegisterBoundingAreaNode(parent, boundingCircle);
}

void CollisionComponent::ProcessInspectorUI() {

}

void CollisionComponent::OnDelete() {
	PhysicsEngine::getInstance().UnRegisterBoundingAreaNode(parent);
}

void CollisionComponent::calculateBoundingCircle() {
    std::vector<std::vector<float>> points = parent->GetComponent<RenderComponent>()->points;
    TransformComponent* tc = parent->GetComponent<TransformComponent>();
	glm::vec3 center = tc->GetWorldPosition();

	float radius = 0;
	for (int i = 0; i < points.size(); i++)
	{
		glm::vec3 p = glm::vec3(points[i][0], points[i][1], 0);
        glm::vec3 worldP = tc->ProjectToWorld(p);

        float dist = glm::distance(center, worldP);
        if (dist > radius) {
            radius = dist;
        }
    }

    BAHnode = PhysicsEngine::getInstance().root.searchFor(parent);

    boundingCircle = BoundingCircle(center, radius);
    if (BAHnode != nullptr) {
        BAHnode->area = boundingCircle;
        BAHnode->recalculateBoundingArea();

        if (BAHnode->parent != nullptr) {
            BAHnode->parent->recalculateBoundingArea();
        }
    }
}
