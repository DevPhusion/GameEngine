#pragma once
#include "Polygon.h"
class ObjectManager
{
public:
	std::vector<std::shared_ptr<Object>> allObjects;

	std::shared_ptr<Polygon> CreatePolygon(std::vector<float> vertices, Shader shader, std::vector<std::string> textures);
};

