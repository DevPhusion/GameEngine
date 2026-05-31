#pragma once
#include "EditorWindow.h"
#include "RenderComponent.h"
class Inspector : public EditorWindow
{
public:
	Inspector(std::string name);

	virtual void ProcessWindow();
};

