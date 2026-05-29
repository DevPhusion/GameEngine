#include "EngineManager.h"

void EngineManager::Setup() {
	InputManager::getInstance().SetKeyButtonCallback([this](int key, int scancode, int action, int mods) {this->SwitchMode(key, scancode, action, mods);});
}

void EngineManager::SwitchMode(int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_E && action == GLFW_PRESS) {
		EngineInteractMode = (InteractMode)((EngineInteractMode + 1) % (sizeof(InteractMode) - 1));

		for (int i = 0; i < InteractModeChangedEvents.size(); i++)
		{
			InteractModeChangedEvents[i]();
		}

		if (EngineInteractMode == InteractMode::AddObject) {
			std::cout << "Add object mode" << std::endl;
		}
		if (EngineInteractMode == InteractMode::VertexSelect) {
			std::cout << "Vertex select mode" << std::endl;
		}
		if (EngineInteractMode == InteractMode::MouseSelect) {
			std::cout << "Mouse select mode" << std::endl;
		}
	}
}

void EngineManager::AddInteractModeChangedEvent(std::function<void()> func) {
	InteractModeChangedEvents.push_back(func);
}
