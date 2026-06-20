#include "Constraint.h"

Constraint::Constraint(Object* objectA, Object* objectB, glm::vec3 attachPointA, glm::vec3 attachPointB) {
	this->objectA = objectA;
	this->objectB = objectB;
	this->attachPointA = attachPointA;
	this->attachPointB = attachPointB;
}