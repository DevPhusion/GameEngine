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
		AddObject,
		VertexSelect,
		MouseSelect,
	};

	InteractMode EngineInteractMode = AddObject;

	std::vector<std::function<void()>> InteractModeChangedEvents;

	void Setup();
	void SwitchMode(int key, int scancode, int action, int mods);
	void AddInteractModeChangedEvent(std::function<void()> func);
private:
	EngineManager() = default;

};

