#pragma once
#include "Shader.h"
#include "Object.h"
#include "RenderComponent.h"
#include "TransformComponent.h"
#include "PhysicsComponent.h"
#include "MouseInteractComponent.h"
#include "CollisionComponent.h"
#include "ConstraintComponent.h"

class Circle : public Object
{
public:
	Circle(Shader shader, std::string texture_path);
	Circle() = default;

	virtual void Process(float delta);
};

