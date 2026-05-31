#define GLM_ENABLE_EXPERIMENTAL
#pragma once
#include "Polygon.h"
#include "Shader.h"
#include "Object.h"
#include "RenderComponent.h"
#include "TransformComponent.h"
#include "VertexComponent.h"
#include "PhysicsComponent.h"
#include "MouseInteractComponent.h"
#include <glad/glad.h>
#include <iostream>
#include <vector>
#include <string>


class Polygon : public Object
{
public:
	Polygon(std::vector<float> vertices, Shader shader, std::vector<std::string> textures);
	Polygon() = default;

	void InitializeDefaultComponents();
private:
	std::vector<float> m_vertices;
	std::vector<std::string> m_textures;

};