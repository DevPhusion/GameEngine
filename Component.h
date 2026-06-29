#pragma once
#include <string>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

class Object;

class Component
{
public:
	Component(Object* parent);
	Component() = default;

	Object* parent;
	bool Enabled = true;

	bool CanDisable = true;
	bool CanRemove = true;
	bool Hidden = false;

	std::string Name;
	virtual void SetEnabled(bool enabled);
	virtual void CopyTo(Object* other) = 0;
	virtual void ProcessInspectorUI() = 0;
	virtual void OnDelete() = 0;
};

