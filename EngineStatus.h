#pragma once
#include "EditorWindow.h"
#include "EngineManager.h"
class EngineStatus : public EditorWindow
{
public:
	EngineStatus(std::string name);
	
	virtual void ProcessWindow();

	void OnInteractModeChanged();
};

