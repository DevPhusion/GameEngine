#pragma once
#include "Object.h"
#include "Shader.h"
#include "RenderComponent.h"
#include "TransformComponent.h"
#include <memory>
class VertexPoint:public Object
{

public:
	VertexPoint(float x, float y, Shader shader);
	VertexPoint() = default;

	float x;
	float y;

	void UpdatePosition(float x, float y);
};