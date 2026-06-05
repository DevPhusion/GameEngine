#include "EngineManager.h"

void EngineManager::Setup(GLFWwindow* window) {
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

void EngineManager::SwitchInteractMode(InteractMode mode) {
	EngineInteractMode = mode;

	for (const auto& [id, func] : InteractModeChangedEvents) {
		func();
	}
}

void EngineManager::SwitchPhysicsMode(PhysicsMode mode) {
	EnginePhysicsMode = mode;

	for (const auto& [id, func] : PhysicsModeChangedEvents) {
		func();
	}
}

int EngineManager::AddInteractModeChangedEvent(std::function<void()> func) {
	CurrentInteractModeChangedID += 1;
	InteractModeChangedEvents[CurrentInteractModeChangedID] = func;
	return CurrentInteractModeChangedID;
}

int EngineManager::AddPhysicsModeChangedEvent(std::function<void()> func) {
	CurrentPhysicsModeChangedID += 1;
	PhysicsModeChangedEvents[CurrentPhysicsModeChangedID] = func;
	return CurrentPhysicsModeChangedID;
}

void EngineManager::RemoveInteractModeChangedEvent(int ID) {
	InteractModeChangedEvents.erase(ID);
}

void EngineManager::RemovePhysicsModeChangedEvent(int ID) {
	PhysicsModeChangedEvents.erase(ID);
}