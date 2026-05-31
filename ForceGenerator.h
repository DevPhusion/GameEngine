#pragma once
#include "Object.h"
#include "PhysicsComponent.h"
class ForceGenerator
{
public:
	virtual void updateForce(Object* object, float delta) = 0;
};

