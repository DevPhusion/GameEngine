#include "VertexComponent.h"
#include "Polygon.h"

bool VertexComponent::vertexSelected = false;

VertexComponent::VertexComponent(std::shared_ptr<Polygon> parent) {
	this->parent = parent;
}

void VertexComponent::SetEnabled(bool enabled) {
	Enabled = enabled;

	if (!enabled) {
		selectedIndex = -1;
	}

	for (int i = 0; i < vertexPoints.size(); i++)
	{
		vertexPoints[i].GetComponent<RenderComponent>()->SetEnabled(enabled);
	}
}

void VertexComponent::SetVertexPoints(std::vector<VertexPoint> vertexPoints) {
	this->vertexPoints = vertexPoints;
	InputManager::getInstance().SetMouseButtonCallback([this](int button, int action, int mods) { this->FindSelectedPoint(button, action, mods); });
	InputManager::getInstance().SetCursorPositionCallback([this](double xpos, double ypos) { this->DragPoint(xpos, ypos); });
	for (int i = 0; i < vertexPoints.size(); i++)
	{
		vertexPoints[i].GetComponent<TransformComponent>()->SetEnabled(true);
	}
	this->parent->GetComponent<TransformComponent>()->SetTransformCallback([this] {this->UpdateTransform();});
}

void VertexComponent::FindSelectedPoint(int button, int action, int mods) {
	if (!InputManager::vertexEditMode || !Enabled) {
		return;
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		for (int i = 0; i < vertexPoints.size(); i++)
		{
			glm::vec2 center = vertexPoints[i].GetComponent<TransformComponent>()->GetTransformedPoint(glm::vec2(vertexPoints[i].x, vertexPoints[i].y));

			float distance = sqrt(pow(InputManager::glX - center.x, 2) + pow(InputManager::glY - center.y, 2));
			if (distance < 0.05f && !vertexSelected) {
				std::cout << "Vertex found at x: " << vertexPoints[i].x << " y: " << vertexPoints[i].y << std::endl;
				vertexSelected = true;
				selectedIndex = i;
				break;
			}
		}
	}
}

void VertexComponent::DragPoint(double xpos, double ypos) {
	if (InputManager::mouseHold) {
		if (selectedIndex != -1 ) {
			glm::vec2 pos = vertexPoints[selectedIndex].GetComponent<TransformComponent>()->GetTransformedPoint(glm::vec2(InputManager::glX, InputManager::glY), true);

			std::vector<float> newVertices = parent->GetComponent<RenderComponent>()->Vertices;
			newVertices[selectedIndex * 8] = pos.x;
			newVertices[selectedIndex * 8 + 1] = pos.y;
			parent->SetVertices(newVertices);

			vertexPoints[selectedIndex].UpdatePosition(pos.x, pos.y);
		}
	}
	else {
		vertexSelected = false;
		selectedIndex = -1;
	}
}

void VertexComponent::UpdateTransform() {
	TransformComponent* parentTransform = this->parent->GetComponent<TransformComponent>();

	for (int i = 0; i < vertexPoints.size(); i++)
	{
		TransformComponent* vertexTransform = vertexPoints[i].GetComponent<TransformComponent>();
		vertexTransform->SetRotationCenter(parentTransform->rotation_center);
		vertexTransform->Translate(parentTransform->position);
		vertexTransform->Rotate(parentTransform->rotation);
		vertexTransform->Scale(parentTransform->size);
	}
}