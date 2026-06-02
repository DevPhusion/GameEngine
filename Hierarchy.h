#pragma once
#include "EditorWindow.h"
#include "AddObjectWindow.h"

class Hierarchy : public EditorWindow
{
public:
	Hierarchy(std::string name);

	AddObjectWindow* addObjectWindow = nullptr;

	bool IsRenaming = false;

	virtual void ProcessWindow();
};

