#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include "Component.h"
#include "Shader.h" 
#include "stb_image.h"
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <glm/gtx/vector_angle.hpp>
#include "imgui/ImGuiFileDialog.h"
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
	int z_index; // ordering when drawing

	virtual void OnDelete();
	virtual void ProcessInspectorUI();
	std::vector<unsigned int> Triangulate(std::vector<float> vertices);
	float GetArea();
	glm::vec3 GetCenter();
	void SetTexture(std::string texture_path);
	bool IsInsideShape(glm::vec3 point);
	void UpdateShape(std::vector<float> vertices, std::vector<unsigned int> indices);
	void Draw();

private:
	bool initialized = false;
	Shader shader;
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;
	unsigned int TextureID;
};

