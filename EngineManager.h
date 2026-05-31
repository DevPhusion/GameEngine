#pragma once
#include "InputManager.h"
#include <functional>

class EngineManager
{
public:
	EngineManager(const EngineManager&) = delete;
	void operator=(const EngineManager&) = delete;

	static EngineManager& getInstance() {
		static EngineManager instance;
		return instance;
	}

	enum InteractMode {
		AddVertex,
		EditorSelect,
	};

	enum PhysicsMode {
		Pause,
		Simulate
	};

	InteractMode EngineInteractMode = AddVertex;
	PhysicsMode EnginePhysicsMode = Pause;

	float fps;
	float windowWidth;
	float windowHeight;
	float aspectRatio;
	std::vector<std::function<void()>> InteractModeChangedEvents;
	std::vector<std::function<void()>> PhysicsModeChangedEvents;

	void Setup(GLFWwindow* window);
	void ProcessEngine(float delta);
	void SwitchInteractMode(int key, int scancode, int action, int mods);
	void SwitchPhysicsMode();
	void AddInteractModeChangedEvent(std::function<void()> func);
	void AddPhysicsModeChangedEvent(std::function<void()> func);
private:
	float time;
	float frameCount;
	EngineManager() = default;

};

