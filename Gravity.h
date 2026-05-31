#pragma once
#include "ForceGenerator.h"
class Gravity : public ForceGenerator
{
public:
	Gravity(glm::vec3 gravity);

	glm::vec3 gravity; // accleration due to gravity

	virtual void updateForce(Object* object, float delta);
};

