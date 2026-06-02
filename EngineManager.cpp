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

	for (int i = 0; i < InteractModeChangedEvents.size(); i++)
	{
		InteractModeChangedEvents[i]();
	}
}

void EngineManager::SwitchPhysicsMode(PhysicsMode mode) {
	EnginePhysicsMode = mode;

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
