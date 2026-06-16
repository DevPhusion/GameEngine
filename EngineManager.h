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

	InteractMode EngineInteractMode = EditorSelect;
	PhysicsMode EnginePhysicsMode = Pause;

	float fps;
	float windowWidth;
	float windowHeight;
	float aspectRatio;
	bool debugMode = false;
	std::unordered_map<int, std::function<void()>> InteractModeChangedEvents;
	std::unordered_map<int, std::function<void()>> PhysicsModeChangedEvents;

	void Setup(GLFWwindow* window);
	void ProcessEngine(float delta);
	void SwitchInteractMode(InteractMode mode);
	void SwitchPhysicsMode(PhysicsMode mode);
	int AddInteractModeChangedEvent(std::function<void()> func);
	int AddPhysicsModeChangedEvent(std::function<void()> func);
	void RemovePhysicsModeChangedEvent(int ID);
	void RemoveInteractModeChangedEvent(int ID);
private:
	int CurrentInteractModeChangedID = -1;
	int CurrentPhysicsModeChangedID = -1;
	float time;
	float frameCount;
	EngineManager() = default;

};

