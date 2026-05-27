#include "TransformComponent.h"

TransformComponent::TransformComponent(Shader shader, glm::vec3 rotation_center) {
	this->shader = shader;
	this->rotation_center = rotation_center;
}

glm::vec2 TransformComponent::GetWorldPosition() {
	glm::vec4 center = glm::vec4(rotation_center.x, rotation_center.y, rotation_center.z, 1.0f);
	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::translate(trans, rotation_center); // Translate to 0,0
	trans = glm::translate(trans, (position + Camera::getInstance().cameraPos) * Camera::getInstance().cameraZoom);
	trans = glm::translate(trans, -rotation_center); // Translate back to original pos
	glm::vec4 transformedCenter = trans * center;
	return glm::vec2(transformedCenter.x, transformedCenter.y);
}

glm::vec2 TransformComponent::GetTransformedPoint(glm::vec2 point, bool inverseTransform) {
	glm::vec4 originalPoint = glm::vec4(point.x, point.y, 1.0f, 1.0f);

	glm::mat4 trans = fixedTransformed;
	trans = glm::translate(trans, rotation_center); // Translate to 0,0
	trans = glm::translate(trans, position);
	trans = glm::rotate(trans, rotation, glm::vec3(0, 0, 1));
	trans = glm::scale(trans, size);
	trans = glm::translate(trans, -rotation_center); // Translate back to original pos

	glm::vec4 transformedPoint = glm::vec4(1);

	if (inverseTransform) {
		transformedPoint =  glm::inverse(trans) * Camera::getInstance().viewMatrixInverse * originalPoint;
	}
	else {
		transformedPoint = Camera::getInstance().viewMatrix * trans * originalPoint;
	}
	
	return glm::vec2(transformedPoint.x, transformedPoint.y);
}

void TransformComponent::SetRotationCenter(glm::vec3 rotation_center) {
	this->rotation_center = rotation_center;
}

void TransformComponent::SetTransform(glm::mat4 transform) {
	this->fixedTransformed = transform;
	if (transformCallback != nullptr) {
		transformCallback();
	}
}

void TransformComponent::Translate(glm::vec3 translation) {
	position = translation;
	if (transformCallback != nullptr) {
		transformCallback();
	}
}

void TransformComponent::Rotate(float angle)
{
	rotation = angle;
	if (transformCallback != nullptr) {
		transformCallback();
	}
}

void TransformComponent::Scale(glm::vec3 scale) {
	size = scale;
	if (transformCallback != nullptr) {
		transformCallback();
	}
}

void TransformComponent::SetTransformCallback(std::function<void()> func) {
	this->transformCallback = func;
}

void TransformComponent::ProcessTransform() {
	if (!Enabled) {
		return;
	}

	this->transform = fixedTransformed;
	this->transform = glm::translate(this->transform, rotation_center); // Translate to 0,0

	this->transform = glm::translate(this->transform, position);
	this->transform = glm::rotate(this->transform, rotation, glm::vec3(0, 0, 1));
	this->transform = glm::scale(this->transform, size);

	this->transform = glm::translate(this->transform, -rotation_center); // Translate back to original pos
	this->shader.setMat4D("transform", this->transform);
	this->shader.setMat4D("view", Camera::getInstance().viewMatrix);
}
