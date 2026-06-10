#pragma once
#include "ForceGenerator.h"
#include <algorithm>
class SpringForce : public ForceGenerator
{
public:
	SpringForce(Object* thisObject, Object* otherObject, 
		glm::vec3 thisConnectionPoint, glm::vec3 otherConnectionPoint, 
		float springConstant, float damping,  float restLength, 
		float angularSpringConstant, float angularDamping, float restAngle);

	Object* thisObject;
	Object* otherObject;
	glm::vec3 thisConnectionPoint;
	glm::vec3 otherConnectionPoint;
	
	float springConstant;
	float damping;
	float restLength;

	float angularSpringConstant;
	float angularDamping;
	float restAngle;

	virtual void updateForce(Object* object, float delta);
	virtual void processDisplay(int index);
};

