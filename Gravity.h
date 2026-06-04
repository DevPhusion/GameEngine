#pragma once
#include "ForceGenerator.h"
class Gravity : public ForceGenerator
{
public:
	Gravity(float gravity);

	float gravity; // accleration due to gravity

	virtual void updateForce(Object* object, float delta);
	virtual void processDisplay(int index);
};

