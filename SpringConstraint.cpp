#include "SpringConstraint.h"

SpringConstraint::SpringConstraint(Object* objectA, Object* objectB, glm::vec3 attachPointA, glm::vec3 attachPointB,
	float length, float stiffness, float damping) :
	Constraint(objectA, objectB, attachPointA, attachPointB) {
	this->length = length;
	this->stiffness = stiffness;
	this->damping = damping;
	this->Name = "Distance Constraint";
}

void SpringConstraint::Prepare(std::vector<SolverRow>& rows, float delta) {
	if (objectA == nullptr || objectB == nullptr) {
		return;
	}

	JacobianRow jacobian = JacobianRow();
	SolverRow row = SolverRow();

	TransformComponent* tcA = objectA->GetComponent<TransformComponent>();
	TransformComponent* tcB = objectB->GetComponent<TransformComponent>();
	RigidBodyComponent* pcA = objectA->GetComponent<RigidBodyComponent>();
	RigidBodyComponent* pcB = objectB->GetComponent<RigidBodyComponent>();

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

	float softnessCFM = 0.0f;
	float finalBeta = beta;

	if (stiffness > 0.0f && k > 0.0f) {
		softnessCFM = 1.0f / (delta * (stiffness + delta * damping));
		finalBeta = delta * stiffness * softnessCFM;

		k += softnessCFM;
	}

	row.effectiveMass = (k > 0.0f) ? 1.0f / k : 0.0f;
	row.softnessCFM = softnessCFM;

	float error = currentDistance - length;
	float rawBias = (finalBeta / delta) * error;

	float maxRecoveryVelocity = 5.0f;
	if (stiffness > 0.0f) {
		row.bias = rawBias;
	}
	else {
		row.bias = glm::clamp(rawBias, -maxRecoveryVelocity, maxRecoveryVelocity);
	}

	row.objectA = objectA;
	row.objectB = objectB;

	row.maxLambda = INFINITY;
	row.minLambda = -INFINITY;

	row.lambda = cacheLambda;
	row.parentConstraint = this;

	rows.push_back(row);
}

void SpringConstraint::ProcessInspectorUI(Object* parent) {
	Constraint::ProcessInspectorUI(parent);

	ImGui::Text("Rest length ");
	ImGui::SameLine();
	ImGui::InputFloat("##Distance", &length, 0.0f, 0.0f, "%.3f m");

	ImGui::Text("Stiffness ");
	ImGui::SameLine();
	ImGui::InputFloat("##Stiffness", &stiffness, 0.0f, 0.0f, "%.3f N/m");


	ImGui::Text("Damping ");
	ImGui::SameLine();
	ImGui::InputFloat("##Damping", &damping, 0.0f, 0.0f, "%.3f Ns/m");
}

void GenerateSegment(glm::vec2 start, glm::vec2 end, float thickness,
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

void SpringConstraint::ProcessConstraintDisplay() {
	Object* topObject = objectA;
	Object* bottomObject = objectB;

	RenderComponent* rc = constraintDisplay->GetComponent<RenderComponent>();
	TransformComponent* tc = constraintDisplay->GetComponent<TransformComponent>();

	if (topObject == nullptr || bottomObject == nullptr || !canDrawConstraint) {
		rc->SetEnabled(false);
		return;
	}

	glm::vec2 topVert = glm::vec2(0);
	glm::vec2 botVert = glm::vec2(0);

	// Project world -> screen
	glm::vec3 top = objectA->GetComponent<TransformComponent>()->GetTransformedPoint(attachPointA);
	topVert = tc->GetTransformedPoint(top, true);
	//Project world -> screen
	glm::vec3 bot = objectB->GetComponent<TransformComponent>()->GetTransformedPoint(attachPointB);
	botVert = tc->GetTransformedPoint(bot, true);
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


	rc->UpdateShape(vertices, indices);

	if (objectA != nullptr && objectB != nullptr) {
		rc->SetEnabled(true);
	}
	else {
		rc->SetEnabled(false);
	}
}