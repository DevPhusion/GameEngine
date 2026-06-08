#pragma once
#include "Object.h"
#include "RenderComponent.h"
#include "TransformComponent.h"
#include "CableComponent.h"
class Cable : public Object
{
public:
	Cable(Shader shader, float maxLength, float restitution);
	Cable() = default;

	void UpdateVertex();
	virtual void Process(float delta);
};

