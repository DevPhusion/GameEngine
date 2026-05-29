#include "MouseInteractComponent.h"

MouseInteractComponent::MouseInteractComponent(Object parent, bool physicsInteract) {
	this->parent = parent;
	this->physicsInteract = physicsInteract;
	InputManager::getInstance().SetMouseButtonCallback([this](int button, int action, int mods) {this->FindSelectedPolygon(button, action, mods);});
	InputManager::getInstance().SetCursorPositionCallback([this](double xpos, double ypos) {this->DragPolygon(xpos, ypos);});
}

void MouseInteractComponent::FindSelectedPolygon(int button, int action, int mods) {
	if (EngineManager::getInstance().EngineInteractMode != EngineManager::InteractMode::MouseSelect) {
		return;
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		glm::vec3 mousePos = parent.GetComponent<TransformComponent>()->GetTransformedPoint(glm::vec3(InputManager::glX, InputManager::glY, 0), true);
		if (parent.GetComponent<RenderComponent>()->IsInsideShape(mousePos)) {
			Selected = true;
			
			if (physicsInteract) {
				mouseDragForce = new MouseDrag(150.0f, 24.5f);
				PhysicsEngine::getInstance().RegisterForce(parent, mouseDragForce);
			}
		}
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (action == GLFW_PRESS) {
			Selected = true;

			if (physicsInteract) {
				mouseDragForce = new MouseDrag(150.0f, 24.5f);
				PhysicsEngine::getInstance().RegisterForce(parent, mouseDragForce);
			}
		}
		else if (action == GLFW_RELEASE) {
			Selected = false;
			if (mouseDragForce != nullptr && physicsInteract) {
				PhysicsEngine::getInstance().UnRegisterForce(parent, mouseDragForce);
				mouseDragForce = nullptr;
			}
		}
	}
}

void MouseInteractComponent::DragPolygon(double xpos, double ypos) {
	if (InputManager::mouseHold) {
		if (Selected && !physicsInteract) {
			TransformComponent* trans = parent.GetComponent<TransformComponent>();
			// project to model space
			glm::vec3 modelPos = trans->GetTransformedPoint(glm::vec3(InputManager::glX, InputManager::glY, 0), true);

			//project to world space
			glm::vec3 worldPos = trans->ProjectToWorld(modelPos);
			trans->UpdateWorldPosition(worldPos);
		}
	}
	else {
		Selected = false;
		if (mouseDragForce != nullptr && physicsInteract) {
			PhysicsEngine::getInstance().UnRegisterForce(parent, mouseDragForce);
			mouseDragForce = nullptr;
		}
	}
}