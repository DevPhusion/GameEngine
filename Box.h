#pragma once
#include "Shader.h"
#include "Object.h"
#include "RenderComponent.h"
#include "TransformComponent.h"
#include "RigidBodyComponent.h"
#include "MouseInteractComponent.h"
#include "CollisionComponent.h"
#include "ConstraintComponent.h"

class Box : public Object
{
public:
	Box(Shader shader, std::string texture_path);
	Box() = default;

	virtual void Process(float delta);
};

