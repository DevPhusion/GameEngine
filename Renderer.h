#pragma once
#include "Polygon.h"
#include "Shader.h"
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include <vector>
#include<iostream>
class Renderer
{
public:
	Renderer(std::vector<Polygon> polygon);
	std::vector<Polygon> poly;
	void Draw();
	void AddPolygon(Polygon polygon);
};

