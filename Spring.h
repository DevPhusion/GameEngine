#pragma once
#include "Object.h"
#include "RenderComponent.h"
#include "TransformComponent.h"
#include "SpringComponent.h"
class Spring : public Object
{
public:
	Spring(Shader shader, float springConstant, float damping, float restLength);
	Spring() = default;

	void UpdateVertex();
	virtual void Process(float delta);
};

