#include "EngineManager.h"

void EngineManager::Setup(GLFWwindow* window) {
	InputManager::getInstance().SetKeyButtonCallback([this](int key, int scancode, int action, int mods) {this->SwitchInteractMode(key, scancode, action, mods);});
	int windowWidth, windowHeight;
	glfwGetWindowSize(window, &windowWidth, &windowHeight);
	this->windowWidth = (float)windowWidth;
	this->windowHeight = (float)windowHeight;
	this->aspectRatio = this->windowWidth / this->windowHeight;
	frameCount = 0;
}

void EngineManager::ProcessEngine(float delta) {
	frameCount++;
	time += delta;

	if (time >= 1) {
		fps = frameCount / time;
		frameCount = 0;
		time = 0;
	}
}

void EngineManager::SwitchInteractMode(int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_E && action == GLFW_PRESS) {
		if (EngineInteractMode == InteractMode::AddVertex) {
			EngineInteractMode = InteractMode::EditorSelect;
		}
		else if (EngineInteractMode == InteractMode::EditorSelect) {
			EngineInteractMode = InteractMode::AddVertex;
		}

		for (int i = 0; i < InteractModeChangedEvents.size(); i++)
		{
			InteractModeChangedEvents[i]();
		}
	}
}

void EngineManager::SwitchPhysicsMode() {
	if (EnginePhysicsMode == PhysicsMode::Pause) {
		EnginePhysicsMode = PhysicsMode::Simulate;
	}
	else if (EnginePhysicsMode == PhysicsMode::Simulate) {
		EnginePhysicsMode = PhysicsMode::Pause;
	}

	for (int i = 0; i < PhysicsModeChangedEvents.size(); i++)
	{
		PhysicsModeChangedEvents[i]();
	}
}

void EngineManager::AddInteractModeChangedEvent(std::function<void()> func) {
	InteractModeChangedEvents.push_back(func);
}

void EngineManager::AddPhysicsModeChangedEvent(std::function<void()> func) {
	PhysicsModeChangedEvents.push_back(func);
}
