#include "SoftBodyComponent.h"
#include "ObjectManager.h"

SoftBodyComponent::SoftBodyComponent(Object* parent) : Component(parent) {
	Name = "Soft Body Component";
	BuildMassAggregate();

	transformCallbackID = parent->GetComponent<TransformComponent>()->AddTransformCallback([this] {UpdateMassAggregate();});
}

void SoftBodyComponent::ProcessInspectorUI() {
	
}

void SoftBodyComponent::CopyTo(Object* other) {
	SoftBodyComponent* target = other->GetComponent<SoftBodyComponent>();
	if (!target) {
		other->AddComponent(std::make_unique<SoftBodyComponent>(other));
		target = other->GetComponent<SoftBodyComponent>();
	}
}

void SoftBodyComponent::OnDelete() {
	for (SpringConstraint* s : springs)
		PhysicsEngine::getInstance().UnRegisterConstraint(s);
	springs.clear();

	for (int i = 0; i < MassAggregate.size(); i++)
	{
		ObjectManager::getInstance().RemoveObject(MassAggregate[i]);
	}

	MassAggregate.clear();

	TransformComponent* tc = parent->GetComponent<TransformComponent>();
	if (tc) tc->RemoveTransformCallback(transformCallbackID);
}

void SoftBodyComponent::UpdateMassAggregate() {
	if (updatingFromParent) return;
	updatingFromParent = true;

	RenderComponent* rc = parent->GetComponent<RenderComponent>();
	TransformComponent* tc = parent->GetComponent<TransformComponent>();

	for (int i = 0; i < rc->points.size(); i++)
	{
		glm::vec3 p = glm::vec3(rc->points[i][0], rc->points[i][1], 0.0f);
		MassAggregate[i]->GetComponent<TransformComponent>()->UpdateWorldPosition(tc->ProjectToWorld(p));
	}

	MassAggregate[MassAggregate.size() - 1]->GetComponent<TransformComponent>()->UpdateWorldPosition(tc->GetWorldPosition());
	updatingFromParent = false;
}

void SoftBodyComponent::UpdatePoint(int index) {
	if (updatingFromParent) return;
	if (MassAggregate.size() == 0) return;
	if (index > (int)MassAggregate.size() - 1 || index < 0) return;

	PointMass* pm = MassAggregate[index];
	RenderComponent* rc = parent->GetComponent<RenderComponent>();
	TransformComponent* tc = parent->GetComponent<TransformComponent>();
	std::vector<float> verts = rc->Vertices;

	glm::vec3 worldP = pm->GetComponent<TransformComponent>()->GetWorldPosition();
	glm::vec3 localP = tc->ProjectToWorld(worldP, true);

	if (!pm->isCenter) {
		verts[index * 5] = localP.x;
		verts[index * 5 + 1] = localP.y;
		rc->UpdateShape(verts, rc->Triangulate(verts));
	}
	else if (std::holds_alternative<CircleShape>(rc->currentShape)) {
		CircleShape& shape = std::get<CircleShape>(rc->currentShape);
		int centerOffset = (int)verts.size() - 5;
		verts[centerOffset] = localP.x;
		verts[centerOffset + 1] = localP.y;
		rc->UpdateShape(verts, rc->TriangulateCircle(shape.segments));
	}
}

void SoftBodyComponent::BuildMassAggregate() {
	MassAggregate.clear();
	RenderComponent* rc = parent->GetComponent<RenderComponent>();
	TransformComponent* tc = parent->GetComponent<TransformComponent>();

	for (int i = 0; i < rc->points.size(); i++)
	{
		glm::vec3 p = glm::vec3(rc->points[i][0], rc->points[i][1], 0.0f);
		std::unique_ptr<PointMass> pm = std::make_unique<PointMass>(Shader("vertex.txt", "fragment.txt"), this, tc->ProjectToWorld(p), i, false);
		MassAggregate.push_back(pm.get());
		ObjectManager::getInstance().allObjects.push_back(std::move(pm));
	}

	std::unique_ptr<PointMass> pm = std::make_unique<PointMass>(Shader("vertex.txt", "fragment.txt"), this, tc->GetWorldPosition(), MassAggregate.size(), true);
	PointMass* centerPM = pm.get();
	MassAggregate.push_back(centerPM);
	ObjectManager::getInstance().allObjects.push_back(std::move(pm));

	int edgeCount = MassAggregate.size() - 1; // excludes center

	// Edge springs (perimeter)
	for (int i = 0; i < edgeCount; i++)
	{
		PointMass* pmA = MassAggregate[i];
		PointMass* pmB = MassAggregate[(i + 1) % edgeCount];

		glm::vec3 posA = pmA->GetComponent<TransformComponent>()->GetWorldPosition();
		glm::vec3 posB = pmB->GetComponent<TransformComponent>()->GetWorldPosition();
		float restLength = glm::length(posB - posA);

		SpringConstraint* spring = new SpringConstraint(pmA, pmB, glm::vec3(0), glm::vec3(0), restLength, stiffness, damping);
		spring->canDrawConstraint = false;
		PhysicsEngine::getInstance().RegisterConstraint(spring);
		springs.push_back(spring);
	}

	// Spoke springs (center to each vertex)
	for (int i = 0; i < edgeCount; i++)
	{
		PointMass* pmV = MassAggregate[i];

		glm::vec3 posV = pmV->GetComponent<TransformComponent>()->GetWorldPosition();
		glm::vec3 posC = centerPM->GetComponent<TransformComponent>()->GetWorldPosition();
		float restLength = glm::length(posV - posC);

		SpringConstraint* spring = new SpringConstraint(pmV, centerPM, glm::vec3(0), glm::vec3(0), restLength, stiffness, damping);
		spring->canDrawConstraint = false;
		PhysicsEngine::getInstance().RegisterConstraint(spring);
		springs.push_back(spring);
	}
}