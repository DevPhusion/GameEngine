#pragma once
#include "Component.h"
#include "Shader.h" 
#include "stb_image.h"
class RenderComponent:public Component
{
public:
	RenderComponent(std::vector<float> vertices, std::vector<unsigned int> indices, Shader shader, std::vector<std::string> textures);
	RenderComponent() = default;
	std::vector<float> Vertices;
	std::vector<unsigned int> Indices;

	void UpdateShape(std::vector<float> vertices, std::vector<unsigned int> indices);
	void Draw();

private:
	Shader shader;
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;
};

