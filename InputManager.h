#pragma once
#include "Object.h"
#include "EditorManager.h"
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<functional>
#include<unordered_map>
class InputManager
{
public:
	InputManager(const InputManager&) = delete;
	void operator=(const InputManager&) = delete;

	static InputManager& getInstance() {
		static InputManager instance;
		return instance;
	}

	GLFWwindow* window;
	static float glX;
	static float glY;
	static bool mouseLeftHold;
	static bool mouseRightHold;

	static std::vector <std::function<void(int, int, int)>> MouseButtonCalls;
	static std::vector <std::function<void(double, double)>> CursorPositionCalls;
	static std::vector <std::function<void(int, int, int, int)>> KeyButtonCalls;
	static std::vector <std::function<void(double, double)>> MouseScrollCalls;
	static std::unordered_map<int, bool> keys;

	void Setup(GLFWwindow* window);
	void SetMouseButtonCallback(std::function<void(int, int, int)> func);
	void SetCursorPositionCallback(std::function<void(double, double)> func);
	void SetKeyButtonCallback(std::function<void(int, int, int, int)> func);
	void SetMouseScrollCallback(std::function<void(double, double)> func);
private:
	InputManager() {};
	static void OnCursorPosition(GLFWwindow* window, double xpos, double ypos);
	static void OnMouseButton(GLFWwindow* window, int button, int action, int mods);
	static void OnKeyButton(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset);
};

