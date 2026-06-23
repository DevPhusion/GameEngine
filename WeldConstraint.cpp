#include "WeldConstraint.h"

WeldConstraint::WeldConstraint(Object* objectA, Object* objectB, glm::vec3 attachPointA, glm::vec3 attachPointB, float angularOffset) :
	Constraint(objectA, objectB, attachPointA, attachPointB) {
	this->angularOffset = angularOffset;
}

void WeldConstraint::Prepare(std::vector<SolverRow>& rows, float delta) {
	if (objectA == nullptr || objectB == nullptr) {
		return;
	}

	TransformComponent* tcA = objectA->GetComponent<TransformComponent>();
	TransformComponent* tcB = objectB->GetComponent<TransformComponent>();
	PhysicsComponent* pcA = objectA->GetComponent<PhysicsComponent>();
	PhysicsComponent* pcB = objectB->GetComponent<PhysicsComponent>();

	glm::vec3 globalPointA = tcA->ProjectToWorld(attachPointA);
	glm::vec3 globalPointB = tcB->ProjectToWorld(attachPointB);

	glm::vec3 rA = globalPointA - tcA->GetWorldPosition();
	glm::vec3 rB = globalPointB - tcB->GetWorldPosition();

	JacobianRow jacobianX = JacobianRow();
	SolverRow rowX = SolverRow();
	JacobianRow jacobianY = JacobianRow();
	SolverRow rowY = SolverRow();
	JacobianRow jacobianTheta = JacobianRow();
	SolverRow rowTheta = SolverRow();
	jacobianX.linearA = glm::vec3(1.0f, 0.0f, 0.0f);
	jacobianX.linearB = glm::vec3(-1.0f, 0.0f, 0.0f);
	jacobianX.angularA = -rA.y;
	jacobianX.angularB = rB.y;


	jacobianY.linearA = glm::vec3(0.0f, 1.0f, 0.0f);
	jacobianY.linearB = glm::vec3(0.0f, -1.0f, 0.0f);
	jacobianY.angularA = rA.x;
	jacobianY.angularB = -rB.x;

	jacobianTheta.linearA = glm::vec3(0);
	jacobianTheta.linearB = glm::vec3(0);
	jacobianTheta.angularA = 1;
	jacobianTheta.angularB = -1;

	float invMassA = (pcA != nullptr) ? pcA->inverseMass : 0.0f;
	float invInertiaA = (pcA != nullptr) ? pcA->inverseInertia : 0.0f;
	float invMassB = (pcB != nullptr) ? pcB->inverseMass : 0.0f;
	float invInertiaB = (pcB != nullptr) ? pcB->inverseInertia : 0.0f;
	float thetaA = tcA->rotation;
	float thetaB = tcB->rotation;

	float kX = invMassA + invInertiaA * jacobianX.angularA * jacobianX.angularA +
		invMassB + invInertiaB * jacobianX.angularB * jacobianX.angularB;

	float kY = invMassA + invInertiaA * jacobianY.angularA * jacobianY.angularA +
		invMassB + invInertiaB * jacobianY.angularB * jacobianY.angularB;

	float kTheta = invInertiaA + invInertiaB;


	glm::vec3 positionError = globalPointB - globalPointA;
	float angleError = thetaB - thetaA - angularOffset;

	float biasX = (beta / delta) * positionError.x;
	float biasY = (beta / delta) * positionError.y;
	float biasTheta = (beta / delta) * angleError;

	rowX.jacobian = jacobianX;
	rowX.effectiveMass = (kX > 0.0f) ? 1.0f / kX : 0.0f;
	rowX.bias = biasX;
	rowX.maxLambda = INFINITY;
	rowX.minLambda = -INFINITY;
	rowX.objectA = objectA;
	rowX.objectB = objectB;
	rowX.parentConstraint = this;
	rowX.lambda = cacheLambda[0];

	rowY.jacobian = jacobianY;
	rowY.effectiveMass = (kY > 0.0f) ? 1.0f / kY : 0.0f;
	rowY.bias = biasY;
	rowY.maxLambda = INFINITY;
	rowY.minLambda = -INFINITY;
	rowY.objectA = objectA;
	rowY.objectB = objectB;
	rowY.parentConstraint = this;
	rowY.lambda = cacheLambda[1];

	rowTheta.jacobian = jacobianTheta;
	rowTheta.effectiveMass = (kTheta > 0.0f) ? 1.0f / kTheta : 0.0f;
	rowTheta.bias = biasTheta;
	rowTheta.maxLambda = INFINITY;
	rowTheta.minLambda = -INFINITY;
	rowTheta.objectA = objectA;
	rowTheta.objectB = objectB;
	rowTheta.parentConstraint = this;
	rowTheta.lambda = cacheLambda[2];

	xRowOffset = static_cast<int>(rows.size());
	rows.push_back(rowX);
	yRowOffset = static_cast<int>(rows.size());
	rows.push_back(rowY);
	thetaRowOffset = static_cast<int>(rows.size());
	rows.push_back(rowTheta);
}

void WeldConstraint::PostSolve(std::vector<SolverRow>& allRows) {
	cacheLambda[0] = allRows[xRowOffset].lambda;
	cacheLambda[1] = allRows[yRowOffset].lambda;
	cacheLambda[2] = allRows[thetaRowOffset].lambda;
}