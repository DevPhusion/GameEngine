#define GLM_ENABLE_EXPERIMENTAL
#pragma once
#include "Shader.h"
#include "Object.h"
#include "RenderComponent.h"
#include "TransformComponent.h"
#include "VertexComponent.h"
#include "RigidBodyComponent.h"
#include "MouseInteractComponent.h"
#include "CollisionComponent.h"
#include "ConstraintComponent.h"
#include <glad/glad.h>
#include <iostream>
#include <vector>
#include <string>


class Polygon : public Object
{
public:
	Polygon(std::vector<float> vertices, Shader shader, std::string texture_path);
	Polygon() = default;

	virtual void Process(float delta);
};