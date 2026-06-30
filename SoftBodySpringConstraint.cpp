#include "SoftBodySpringConstraint.h"

SoftBodySpringConstraint::SoftBodySpringConstraint(PhysicsBody objectA, PhysicsBody objectB, glm::vec3 attachPointA, glm::vec3 attachPointB,
	float length, float stiffness, float damping) {
	this->objectA = objectA;
	this->objectB = objectB;
	this->attachPointA = attachPointA;
	this->attachPointB = attachPointB;
	this->length = length;
	this->stiffness = stiffness;
	this->damping = damping;
	this->Name = "Distance Constraint";
}

void SoftBodySpringConstraint::Prepare(std::vector<SolverRow>& rows, float delta) {
	JacobianRow jacobian = JacobianRow();
	SolverRow row = SolverRow();

	glm::vec3 globalPointA = *objectA.transformMatrix * glm::vec4(attachPointA, 1);
	glm::vec3 globalPointB = *objectB.transformMatrix * glm::vec4(attachPointB, 1);

	glm::vec3 d = globalPointB - globalPointA;
	float currentDistance = glm::length(d);
	glm::vec3 d_hat = (currentDistance > 0.00001f) ? d / currentDistance : glm::vec3(0.0f);
	glm::vec3 rA = globalPointA - *objectA.position;
	glm::vec3 rB = globalPointB - *objectB.position;

	jacobian.linearA = glm::vec3(d_hat.x, d_hat.y, 0.0f);
	jacobian.linearB = glm::vec3(-d_hat.x, -d_hat.y, 0.0f);
	jacobian.angularA = (rA.x * d_hat.y - rA.y * d_hat.x);
	jacobian.angularB = -(rB.x * d_hat.y - rB.y * d_hat.x);

	row.jacobian = jacobian;

	float k = 0.0f;

	if (objectA.invMass != nullptr && objectA.invInertia != nullptr) {
		k += *objectA.invMass * glm::length2(jacobian.linearA) + *objectA.invInertia * (jacobian.angularA * jacobian.angularA);
	}
	if (objectB.invMass != nullptr && objectB.invInertia != nullptr) {
		k += *objectB.invMass * glm::length2(jacobian.linearB) + *objectB.invInertia * (jacobian.angularB * jacobian.angularB);
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

void SoftBodySpringConstraint::ProcessInspectorUI(Object* parent) {
	return;
}

void SoftBodySpringConstraint::ProcessConstraintDisplay() {
	return;
}