#include "RevoluteConstraint.h"

RevoluteConstraint::RevoluteConstraint(Object* objectA, Object* objectB, glm::vec3 attachPointA, glm::vec3 attachPointB) :
	Constraint(objectA, objectB, attachPointA, attachPointB) {

}

void RevoluteConstraint::Prepare(std::vector<SolverRow>& rows, float delta) {
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
	jacobianX.linearA = glm::vec3(1.0f, 0.0f, 0.0f);
	jacobianX.linearB = glm::vec3(-1.0f, 0.0f, 0.0f);
	jacobianX.angularA = -rA.y;
	jacobianX.angularB = rB.y;
	rowX.jacobian = jacobianX; 

	JacobianRow jacobianY = JacobianRow();
	SolverRow rowY = SolverRow();
	jacobianY.linearA = glm::vec3(0.0f, 1.0f, 0.0f);
	jacobianY.linearB = glm::vec3(0.0f, -1.0f, 0.0f);
	jacobianY.angularA = rA.x;
	jacobianY.angularB = -rB.x;
	rowY.jacobian = jacobianY; 

	float invMassA = (pcA != nullptr) ? pcA->inverseMass : 0.0f;
	float invInertiaA = (pcA != nullptr) ? pcA->inverseInertia : 0.0f;
	float invMassB = (pcB != nullptr) ? pcB->inverseMass : 0.0f;
	float invInertiaB = (pcB != nullptr) ? pcB->inverseInertia : 0.0f;

	float kX = invMassA + invInertiaA * jacobianX.angularA * jacobianX.angularA +
		invMassB + invInertiaB * jacobianX.angularB * jacobianX.angularB;

	float kY = invMassA + invInertiaA * jacobianY.angularA * jacobianY.angularA +
		invMassB + invInertiaB * jacobianY.angularB * jacobianY.angularB;

	rowX.effectiveMass = (kX > 0.0f) ? 1.0f / kX : 0.0f;
	rowY.effectiveMass = (kY > 0.0f) ? 1.0f / kY : 0.0f;

	glm::vec3 positionError = globalPointB - globalPointA;

	float biasX = (beta / delta) * positionError.x;
	float biasY = (beta / delta) * positionError.y;

	rowX.bias = biasX;
	rowX.maxLambda = INFINITY;
	rowX.minLambda = -INFINITY;
	rowX.objectA = objectA;
	rowX.objectB = objectB;
	rowX.parentConstraint = this;

	rowY.bias = biasY;
	rowY.maxLambda = INFINITY;
	rowY.minLambda = -INFINITY;
	rowY.objectA = objectA;
	rowY.objectB = objectB;
	rowY.parentConstraint = this;

	rows.push_back(rowX);
	rows.push_back(rowY);
}
	