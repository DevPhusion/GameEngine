#define GLM_ENABLE_EXPERIMENTAL
#pragma once
#include "Polygon.h"
#include "Shader.h"
#include "Object.h"
#include "RenderComponent.h"
#include "TransformComponent.h"
#include "VertexComponent.h"
#include "PhysicsComponent.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/constants.hpp>
#include <glm/vec2.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <iostream>
#include <vector>
#include <string>


class Polygon : public Object
{
public:
	Polygon(std::vector<float> vertices, Shader shader, std::vector<std::string> textures);
	Polygon() = default;

	void SetVertices(std::vector<float> vertices);
	glm::vec3 GetCenter();
private:
	std::vector<unsigned int> Triangulate(std::vector<float> vertices);
};