#pragma once
#include "Object.h"

class SoftBodyComponent;

class PointMass
{
public:
	PointMass(Shader shader, SoftBodyComponent* sb, glm::vec3 point, int index, bool isCenter);
	PointMass() = default;

	SoftBodyComponent* sb;

	int index;
	bool isCenter;
	glm::vec3 worldPos;

	void Process(float delta);
};

