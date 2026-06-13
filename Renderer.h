#pragma once
#include "Shader.h"
#include "Object.h"
#include "RenderComponent.h"
#include "TransformComponent.h"
#include "PhysicsEngine.h"
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include <vector>
#include<iostream>
class Renderer
{
public:
	Renderer(std::vector<std::unique_ptr<Object>>* objects);
	void Draw();
private:
	std::vector<std::unique_ptr<Object>>* allObjects;
};

