#pragma once
#include "ForceGenerator.h"
class Drag : public ForceGenerator
{
public:
	Drag(float k1, float k2);

	//Drag coefficient
	float k1; 
	float k2;

	virtual void updateForce(Object object, float delta);
};

