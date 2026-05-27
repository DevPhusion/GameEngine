#pragma once
#include "Component.h"
#include "Shader.h"
#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <functional>
class TransformComponent : public Component
{
public:
	TransformComponent(Shader shader, glm::vec3 rotation_center);
	TransformComponent() = default;

	glm::mat4 fixedTransformed = glm::mat4(1.0f);
	glm::mat4 transform = glm::mat4(1.0f); // transformation matrix

	glm::vec3 rotation_center = glm::vec3(0);
	glm::vec3 position = glm::vec3(0);
	glm::vec3 size = glm::vec3(1);
	std::function<void()> transformCallback;
	float rotation = 0;

	glm::vec2 GetTransformedPoint(glm::vec2 point, bool inverseTransform = false);
	glm::vec2 GetWorldPosition();
	void SetTransform(glm::mat4 transform);
	void SetRotationCenter(glm::vec3 rotation_center);
	void Translate(glm::vec3 translation);
	void Rotate(float angle);
	void Scale(glm::vec3 scale);
	void SetTransformCallback(std::function<void()> func);
	void ProcessTransform();
private:
	Shader shader;
};

