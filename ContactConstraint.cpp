#include "ContactConstraint.h"

ContactConstraint::ContactConstraint(Object* objectA, Object* objectB, glm::vec3 attachPointA, glm::vec3 attachPointB, 
	glm::vec3 normal, float penetration, float restitution, float staticFriction, float dynamicFriction) :
	Constraint(objectA, objectB, attachPointA, attachPointB) {
	this->normal = normal;
	this->penetration = penetration;
	this->restitution = restitution;
	this->staticFriction = staticFriction;
	this->dynamicFriction = dynamicFriction;
}

void ContactConstraint::Prepare(std::vector<SolverRow>& rows, float delta) {
	TransformComponent* tcA = objectA->GetComponent<TransformComponent>();
	TransformComponent* tcB = objectB->GetComponent<TransformComponent>();
	PhysicsComponent* pcA = objectA->GetComponent<PhysicsComponent>();
	PhysicsComponent* pcB = objectB->GetComponent<PhysicsComponent>();

	glm::vec3 rA = attachPointA - tcA->GetWorldPosition();
	glm::vec3 rB = attachPointB - tcB->GetWorldPosition();

	this->normalRowOffsetIndex = static_cast<int>(rows.size());

	//Normal

	SolverRow nRow = SolverRow();
	JacobianRow nJac = JacobianRow();
	nJac.linearA = glm::vec3(normal.x, normal.y, 0.0f);
	nJac.linearB = glm::vec3(-normal.x, -normal.y, 0.0f);
	nJac.angularA = (rA.x * normal.y - rA.y * normal.x);
	nJac.angularB = -(rB.x * normal.y - rB.y * normal.x);
	nRow.jacobian = nJac;

	float kN = 0.0f;
	if (pcA) kN += pcA->inverseMass * glm::length2(nJac.linearA) + pcA->inverseInertia * (nJac.angularA * nJac.angularA);
	if (pcB) kN += pcB->inverseMass * glm::length2(nJac.linearB) + pcB->inverseInertia * (nJac.angularB * nJac.angularB);
	nRow.effectiveMass = (kN > 0.0f) ? 1.0f / kN : 0.0f;

	float relativeVelocity = 0.0f;
	if (pcA) relativeVelocity += glm::dot(nJac.linearA, pcA->velocity) + nJac.angularA * pcA->angularVelocity;
	if (pcB) relativeVelocity += glm::dot(nJac.linearB, pcB->velocity) + nJac.angularB * pcB->angularVelocity;

	float restitutionBias = 0.0f;
	if (relativeVelocity < -bounceThreshold) {
		restitutionBias = -restitution * relativeVelocity;
	}

	float contactBeta = 0.1f;
	float slop = 0.01f;
	float recoveryError = std::max(0.0f, penetration - slop);
	float positionBias = (contactBeta / delta) * recoveryError;

	float maxRecoveryVelocity = 2.0f;
	positionBias = std::min(positionBias, maxRecoveryVelocity);
	nRow.bias = restitutionBias + positionBias;

	nRow.minLambda = 0.0f; 
	nRow.maxLambda = INFINITY;
	nRow.parentConstraint = this;
	nRow.objectA = objectA; nRow.objectB = objectB;
	rows.push_back(nRow);

	// Friction

	SolverRow fRow = SolverRow();
	JacobianRow fJac = JacobianRow();
	glm::vec3 tangent = glm::vec3(-normal.y, normal.x, 0.0f);
	fJac.linearA = glm::vec3(tangent.x, tangent.y, 0.0f);
	fJac.linearB = glm::vec3(-tangent.x, -tangent.y, 0.0f);
	fJac.angularA = (rA.x * tangent.y - rA.y * tangent.x);
	fJac.angularB = -(rB.x * tangent.y - rB.y * tangent.x);
	fRow.jacobian = fJac;

	float kF = 0.0f;
	if (pcA) kF += pcA->inverseMass * glm::length2(fJac.linearA) + pcA->inverseInertia * (fJac.angularA * fJac.angularA);
	if (pcB) kF += pcB->inverseMass * glm::length2(fJac.linearB) + pcB->inverseInertia * (fJac.angularB * fJac.angularB);
	fRow.effectiveMass = (kF > 0.0f) ? 1.0f / kF : 0.0f;

	fRow.bias = 0.0f;
	fRow.minLambda = -INFINITY; 
	fRow.maxLambda = INFINITY;
	fRow.parentConstraint = this;
	fRow.objectA = objectA; fRow.objectB = objectB;
	rows.push_back(fRow);
}

void ContactConstraint::PostIterationClamp(std::vector<SolverRow>& allRows, int myRowIndex, int velocityIteration) {
	if (myRowIndex == normalRowOffsetIndex) {
		allRows[myRowIndex].lambda = std::max(0.0f, allRows[myRowIndex].lambda);
		return;
	}

	auto& frictionRow = allRows[myRowIndex];
	float normalLambda = allRows[normalRowOffsetIndex].lambda;

	float maxStatic = this->staticFriction * normalLambda;
	float maxDynamic = this->dynamicFriction * normalLambda;

	if (std::abs(frictionRow.lambda) > maxStatic) {
		frictionRow.lambda = glm::clamp(frictionRow.lambda, -maxDynamic, maxDynamic);
	}
	else {
		frictionRow.lambda = glm::clamp(frictionRow.lambda, -maxStatic, maxStatic);
	}
}