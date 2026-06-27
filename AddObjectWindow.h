#pragma once
#include "EditorWindow.h"
#include <vector>

class AddObjectWindow : public EditorWindow
{
public:
	std::vector<std::string> ObjectTypes = {"Object", "Box", "Circle", "Polygon"};
	std::string SelectedType = "";

	AddObjectWindow(std::string name);
	virtual void ProcessWindow();
};

