#include "DistanceConstraint.h"

DistanceConstraint::DistanceConstraint(Object* objectA, Object* objectB, glm::vec3 attachPointA, glm::vec3 attachPointB, 
	float distance, bool extendable, bool retractable) : 
	Constraint(objectA, objectB, attachPointA, attachPointB) {
	this->distance = distance;
	this->extendable = extendable;
	this->retractable = retractable;
	this->Name = "Distance Constraint";
}

void DistanceConstraint::Prepare(std::vector<SolverRow>& rows, float delta) {
	if (objectA == nullptr || objectB == nullptr) {
		return;
	}

	JacobianRow jacobian = JacobianRow();
	SolverRow row = SolverRow();

	TransformComponent* tcA = objectA->GetComponent<TransformComponent>();
	TransformComponent* tcB = objectB->GetComponent<TransformComponent>();
	PhysicsComponent* pcA = objectA->GetComponent<PhysicsComponent>();
	PhysicsComponent* pcB = objectB->GetComponent<PhysicsComponent>();

	glm::vec3 globalPointA = tcA->ProjectToWorld(attachPointA);
	glm::vec3 globalPointB = tcB->ProjectToWorld(attachPointB);

	glm::vec3 d = globalPointB - globalPointA;
	float currentDistance = glm::length(d);
	glm::vec3 d_hat = (currentDistance > 0.00001f) ? d / currentDistance : glm::vec3(0.0f);
	glm::vec3 rA = globalPointA - tcA->GetWorldPosition();
	glm::vec3 rB = globalPointB - tcB->GetWorldPosition();

	jacobian.linearA = glm::vec3(d_hat.x, d_hat.y, 0.0f);
	jacobian.linearB = glm::vec3(-d_hat.x, -d_hat.y, 0.0f);
	jacobian.angularA = (rA.x * d_hat.y - rA.y * d_hat.x);
	jacobian.angularB = -(rB.x * d_hat.y - rB.y * d_hat.x);

	row.jacobian = jacobian;
	
	float k = 0.0f;

	if (pcA != nullptr) {
		k += pcA->inverseMass * glm::length2(jacobian.linearA) + pcA->inverseInertia * (jacobian.angularA * jacobian.angularA);
	}
	if (pcB != nullptr) {
		k += pcB->inverseMass * glm::length2(jacobian.linearB) + pcB->inverseInertia * (jacobian.angularB * jacobian.angularB);
	}

	row.effectiveMass = (k > 0.0f) ? 1.0f / k : 0.0f;

	float error = currentDistance - distance;
	float rawBias = (beta / delta) * error;

	row.bias = rawBias;

	row.objectA = objectA;
	row.objectB = objectB;

	row.maxLambda = INFINITY;
	row.minLambda = -INFINITY;

	if (retractable) {
		row.minLambda = 0;
	}
	if (extendable) {
		row.maxLambda = 0;
	}

	row.lambda = cacheLambda;
	row.parentConstraint = this;

	rows.push_back(row);
}

void DistanceConstraint::ProcessInspectorUI(Object* parent) {
	Constraint::ProcessInspectorUI(parent);

	ImGui::Text("Distance ");
	ImGui::SameLine();
	ImGui::InputFloat("##Distance", &distance, 0.0f, 0.0f, "%.3f m");

	ImGui::Text("Retractable ");
	ImGui::SameLine();
	ImGui::Checkbox("##Retractable ", &retractable);

	ImGui::Text("Extendable ");
	ImGui::SameLine();
	ImGui::Checkbox("##Extendable ", &extendable);
}

void DistanceConstraint::ProcessConstraintDisplay() {
	RenderComponent* rc = constraintDisplay->GetComponent<RenderComponent>();
	TransformComponent* tc = constraintDisplay->GetComponent<TransformComponent>();
	if (objectA == nullptr || objectB == nullptr || !canDrawConstraint) {
		rc->SetEnabled(false);
		return;
	}

	glm::vec2 topVert = glm::vec2(0);
	glm::vec2 botVert = glm::vec2(0);

	glm::vec3 top = objectA->GetComponent<TransformComponent>()->GetTransformedPoint(attachPointA);
	topVert = tc->GetTransformedPoint(top, true);
	//Project world -> screen
	glm::vec3 bot = objectB->GetComponent<TransformComponent>()->GetTransformedPoint(attachPointB);
	botVert = tc->GetTransformedPoint(bot, true);
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

	rc->UpdateShape(vertices, indices);

	if (objectA != nullptr && objectB != nullptr) {
		rc->SetEnabled(true);
	}
	else {
		rc->SetEnabled(false);
	}
}