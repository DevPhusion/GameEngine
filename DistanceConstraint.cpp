#include "DistanceConstraint.h"

DistanceConstraint::DistanceConstraint(Object* objectA, Object* objectB, glm::vec3 attachPointA, glm::vec3 attachPointB, 
	float distance, float stiffness, float damping, 
	bool extendable, bool retractable) : 
	Constraint(objectA, objectB, attachPointA, attachPointB) {
	this->distance = distance;
	this->stiffness = stiffness;
	this->damping = damping;
	this->extendable = extendable;
	this->retractable = retractable;
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

	float softnessCFM = 0.0f;
	float finalBeta = beta;

	if (stiffness > 0.0f && k > 0.0f) {
		softnessCFM = 1.0f / (delta * (stiffness + delta * damping));
		finalBeta = delta * stiffness * softnessCFM;

		k += softnessCFM;
	}

	row.effectiveMass = (k > 0.0f) ? 1.0f / k : 0.0f;
	row.softnessCFM = softnessCFM;

	float error = currentDistance - distance;
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

	if (retractable) {
		row.minLambda = 0;
	}
	if (extendable) {
		row.maxLambda = 0;
	}

	row.lambda = 0.0f;

	rows.push_back(row);
}