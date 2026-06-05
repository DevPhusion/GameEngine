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
	RenderComponent(Object* parent, std::vector<float> vertices, Shader shader, std::string texture_path);
	RenderComponent() = default;
	std::vector<float> Vertices;
	std::vector<unsigned int> Indices;
	std::vector<std::vector<float>> points;
	std::vector<std::vector<std::vector<float>>> edges;
	std::string texture_path;

	virtual void OnDelete();
	virtual void ProcessInspectorUI();
	std::vector<unsigned int> Triangulate(std::vector<float> vertices);
	glm::vec3 GetCenter();
	bool IsInsideShape(glm::vec3 point);
	void UpdateShape(std::vector<float> vertices, std::vector<unsigned int> indices);
	void Draw();

private:
	Shader shader;
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;
};

