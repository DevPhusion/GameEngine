#define GLM_ENABLE_EXPERIMENTAL
#pragma once
#include "Polygon.h"
#include "Shader.h"
#include "stb_image.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/constants.hpp>
#include <glm/vec2.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <iostream>
#include <vector>
#include <string>


class Polygon
{
public:
	Polygon(std::vector<float> vertices, Shader shader, std::vector<std::string> textures);
	Polygon() = default;

	void Draw();
	void UpdateBuffer();
	void AddVertex(std::vector<float> vertex);
	void InsertVertex(unsigned int index, std::vector<float> vertex);
	void SetVertices(std::vector<float> vertices);
	void RemoveVertex(unsigned int index);
	std::vector<float> Vertices;
	std::vector<float> GetVertices();
private:
	std::vector<unsigned int> Indices;
	Shader shader;
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;

	std::vector<unsigned int> Triangulate(std::vector<float> vertices);
};