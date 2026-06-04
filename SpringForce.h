#pragma once
#include "ForceGenerator.h"
#include <algorithm>
class SpringForce : public ForceGenerator
{
public:
	SpringForce(Object* thisObject, Object* otherObject, float springConstant, float damping, float restLength);

	Object* thisObject;
	Object* otherObject;
	float springConstant;
	float damping;
	float restLength;

	virtual void updateForce(Object* object, float delta);
	virtual void processDisplay(int index);
};

