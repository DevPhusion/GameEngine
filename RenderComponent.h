#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include "Component.h"
#include "Shader.h" 
#include "stb_image.h"
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <glm/gtx/vector_angle.hpp>
class RenderComponent:public Component
{
public:
	RenderComponent(std::vector<float> vertices, Shader shader, std::vector<std::string> textures);
	RenderComponent() = default;
	std::vector<float> Vertices;
	std::vector<unsigned int> Indices;
	std::vector<std::vector<float>> points;
	std::vector<std::vector<std::vector<float>>> edges;

	std::vector<unsigned int> Triangulate(std::vector<float> vertices);
	bool IsInsideShape(glm::vec3 point);
	void UpdateShape(std::vector<float> vertices, std::vector<unsigned int> indices);
	void Draw();

private:
	Shader shader;
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;
};

