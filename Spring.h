#pragma once
#include "Object.h"
#include "RenderComponent.h"
#include "TransformComponent.h"
#include "SpringComponent.h"
class Spring : public Object
{
public:
	Spring(Shader shader, float springConstant, float damping, float restLength, float angularSpringConstant, float angularDamping, float restAngle);
	Spring() = default;

	void UpdateVertex();
	void GenerateSegment(glm::vec2 start, glm::vec2 end, float thickness,
		std::vector<float>& vertices, std::vector<unsigned int>& indices);
	virtual void Process(float delta);
};

