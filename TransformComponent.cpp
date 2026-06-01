#include "TransformComponent.h"

TransformComponent::TransformComponent(Object* parent, Shader shader, glm::vec3 rotation_center) : Component(parent) {
	Name = "Transform Component";

	this->shader = shader;
	this->rotation_center = rotation_center;
	SetOriginTransform(Camera::getInstance().viewMatrixInverse);
}

void TransformComponent::ProcessInspectorUI() {
	ImGui::Text("Position ");
	ImGui::SameLine();
	float position[] = { GetWorldPosition().x, GetWorldPosition().y };
	if (ImGui::InputFloat2("## Position", position)) {
		UpdateWorldPosition(glm::vec3(position[0], position[1], 0));
	}
	ImGui::Text("Rotation ");
	ImGui::SameLine();
	float rotation = this->rotation;
	if (ImGui::SliderAngle("## Rotation", &rotation, -180.0f, 180.0f)) {
		Rotate(rotation);
	}

	ImGui::Text("Scale ");
	ImGui::SameLine();
	float size[] = { this->size.x, this->size.y };
	if (ImGui::InputFloat2("## Scale", size)) {
		Scale(glm::vec3(size[0], size[1], 1));
	}

}

glm::vec3 TransformComponent::GetWorldPosition() {
	return ProjectToWorld(rotation_center);
}

glm::vec3 TransformComponent::ProjectToWorld(glm::vec3 point) {
	glm::vec4 p = glm::vec4(point.x, point.y, point.z, 1.0f);
	glm::mat4 rotated = glm::rotate(OriginTransform, rotation, glm::vec3(0, 0, 1));
	glm::mat4 scaled = glm::scale(rotated, size);
	glm::vec4 worldPos = scaled * p;
	return glm::vec3(worldPos.x, worldPos.y, 0);
}

void TransformComponent::UpdateWorldPosition(glm::vec3 targetWorldPos) {
	glm::vec4 center = glm::vec4(rotation_center.x, rotation_center.y, rotation_center.z, 1.0f);
	glm::mat4 rotated = glm::rotate(OriginTransform, rotation, glm::vec3(0, 0, 1));
	glm::mat4 scaled = glm::scale(rotated, size);
	glm::vec4 currentWorldPos = scaled * center;

	glm::vec3 currentPosVec3 = glm::vec3(currentWorldPos.x, currentWorldPos.y, currentWorldPos.z);
	glm::vec3 delta = targetWorldPos - currentPosVec3;

	glm::mat4 newOriginTransform = glm::translate(glm::mat4(1.0f), delta) * OriginTransform;
	
	SetOriginTransform(newOriginTransform);
}

glm::vec3 TransformComponent::GetTransformedPoint(glm::vec3 point, bool inverseTransform) {
	glm::vec4 originalPoint = glm::vec4(point.x, point.y, 1.0f, 1.0f);

	glm::mat4 trans = OriginTransform;
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
	
	return glm::vec3(transformedPoint.x, transformedPoint.y, 0.0f);
}

void TransformComponent::SetRotationCenter(glm::vec3 rotation_center) {
	this->rotation_center = rotation_center;
}

void TransformComponent::SetOriginTransform(glm::mat4 transform) {
	this->OriginTransform = transform;
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

	this->transform = OriginTransform;
	this->transform = glm::translate(this->transform, rotation_center); // Translate to 0,0

	this->transform = glm::translate(this->transform, position);
	this->transform = glm::rotate(this->transform, rotation, glm::vec3(0, 0, 1));
	this->transform = glm::scale(this->transform, size);

	this->transform = glm::translate(this->transform, -rotation_center); // Translate back to original pos
	this->shader.setMat4D("transform", this->transform);
	this->shader.setMat4D("view", Camera::getInstance().viewMatrix);
}
