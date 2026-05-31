#include "MouseInteractComponent.h"

bool MouseInteractComponent::ObjectSelected = false;

MouseInteractComponent::MouseInteractComponent(Object* parent, bool physicsInteract) : Component(parent) {
	Name = "Mouse Interact Component";
	this->physicsInteract = physicsInteract;
	InputManager::getInstance().SetMouseButtonCallback([this](int button, int action, int mods) {this->FindSelectedPolygon(button, action, mods);});
	InputManager::getInstance().SetCursorPositionCallback([this](double xpos, double ypos) {this->DragPolygon(xpos, ypos);});
	EngineManager::getInstance().AddPhysicsModeChangedEvent([this]() {this->OnPhysicsModeChanged();});
}

void MouseInteractComponent::ProcessInspectorUI() {

}

void MouseInteractComponent::FindSelectedPolygon(int button, int action, int mods) {
	if (EngineManager::getInstance().EngineInteractMode != EngineManager::InteractMode::EditorSelect) {
		return;
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {

		if (ObjectSelected) {
			if (parent->HasComponent<VertexComponent>()) {
				if (parent->GetComponent<VertexComponent>()->GetSelectedVertex() != -1) {
					return;
				}
				parent->GetComponent<VertexComponent>()->SetEnabled(false);
			}
			return;
		}

		glm::vec3 mousePos = parent->GetComponent<TransformComponent>()->GetTransformedPoint(glm::vec3(InputManager::glX, InputManager::glY, 0), true);
		if (parent->GetComponent<RenderComponent>()->IsInsideShape(mousePos)) {
			Selected = true;
			ObjectSelected = true;
			
			if (physicsInteract && EngineManager::getInstance().EnginePhysicsMode == EngineManager::PhysicsMode::Simulate) {
				if (mouseDragForce != nullptr) {
					PhysicsEngine::getInstance().UnRegisterForce(parent, mouseDragForce);
				}

				mouseDragForce = new MouseDrag(150.0f, 24.5f);
				PhysicsEngine::getInstance().RegisterForce(parent, mouseDragForce);
			}
			else if (EngineManager::getInstance().EnginePhysicsMode == EngineManager::PhysicsMode::Pause) {
				EditorManager::getInstance().SetSelectedObject(parent);
				if (parent->HasComponent<VertexComponent>()) {
					parent->GetComponent<VertexComponent>()->SetEnabled(true);
				}
			}
		}
		else {
			if (parent->HasComponent<VertexComponent>()) {
				if (parent->GetComponent<VertexComponent>()->GetSelectedVertex() != -1) {
					return;
				}
				parent->GetComponent<VertexComponent>()->SetEnabled(false);
			}
			EditorManager::getInstance().SetSelectedObject(nullptr);
		}

	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		if (!Selected && physicsInteract && EngineManager::getInstance().EnginePhysicsMode == EngineManager::PhysicsMode::Simulate) {
			if (mouseDragForce != nullptr) {
				PhysicsEngine::getInstance().UnRegisterForce(parent, mouseDragForce);
			}

			mouseDragForce = new MouseDrag(150.0f, 24.5f);
			PhysicsEngine::getInstance().RegisterForce(parent, mouseDragForce);
		}
		Selected = true;
	}
}

void MouseInteractComponent::DragPolygon(double xpos, double ypos) {
	if (InputManager::mouseLeftHold || InputManager::mouseRightHold) 
	{
		if (parent->HasComponent<VertexComponent>()) {
			if (parent->GetComponent<VertexComponent>()->selectedIndex != -1) {
				return;
			}
		}

		if (Selected && EngineManager::getInstance().EnginePhysicsMode == EngineManager::PhysicsMode::Pause) {
			TransformComponent* trans = parent->GetComponent<TransformComponent>();
			// project to model space
			glm::vec3 modelPos = trans->GetTransformedPoint(glm::vec3(InputManager::glX, InputManager::glY, 0), true);

			//project to world space
			glm::vec3 worldPos = trans->ProjectToWorld(modelPos);
			trans->UpdateWorldPosition(worldPos);
		}
	}
	else {
		Selected = false;
		ObjectSelected = false;
		if (mouseDragForce != nullptr && physicsInteract) {
			PhysicsEngine::getInstance().UnRegisterForce(parent, mouseDragForce);
			mouseDragForce = nullptr;
		}
	}
}

void MouseInteractComponent::OnPhysicsModeChanged() {
	if (parent->HasComponent<VertexComponent>() && EngineManager::getInstance().EnginePhysicsMode != EngineManager::PhysicsMode::Pause) {
		parent->GetComponent<VertexComponent>()->SetEnabled(false);
	}
}