#include "InputManager.h"

float InputManager::glX = 0.0f;
float InputManager::glY = 0.0f;
bool InputManager::mouseHold = false;
bool InputManager::vertexEditMode = false;

std::vector <std::function<void(int, int, int)>> InputManager::MouseButtonCalls = {};
std::vector <std::function<void(double, double)>> InputManager::CursorPositionCalls = {};
std::vector <std::function<void(int, int, int, int)>> InputManager::KeyButtonCalls = {};
std::vector <std::function<void(double, double)>> InputManager::MouseScrollCalls = {};

std::unordered_map<int, bool> InputManager::keys = {};

void InputManager::Setup(GLFWwindow* window) {
	this->window = window;
	glfwSetMouseButtonCallback(this->window, OnMouseButton);
	glfwSetCursorPosCallback(this->window, OnCursorPosition);
	glfwSetKeyCallback(this->window, OnKeyButton);
	glfwSetScrollCallback(this->window, OnMouseScroll);
}

void InputManager::SetMouseButtonCallback(std::function<void(int, int, int)> func) {
	MouseButtonCalls.push_back(func);
}

void InputManager::SetCursorPositionCallback(std::function<void(double, double)> func) {
	CursorPositionCalls.push_back(func);
	
}

void InputManager::SetKeyButtonCallback(std::function<void(int, int, int, int)> func) {
	KeyButtonCalls.push_back(func);
}

void InputManager::SetMouseScrollCallback(std::function<void(double, double)> func) {
	MouseScrollCalls.push_back(func);
}

void InputManager::OnCursorPosition(GLFWwindow* window, double xpos, double ypos) {
	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);
	int windowWidth, windowHeight;
	glfwGetWindowSize(window, &windowWidth, &windowHeight);
	glX = (2.0f * static_cast<float>(mouseX) / windowWidth) - 1.0f;
	glY = 1.0f - (2.0f * static_cast<float>(mouseY) / windowHeight);

	for (int i = 0; i < CursorPositionCalls.size(); i++)
	{
		CursorPositionCalls[i](xpos, ypos);
	}
}

void InputManager::OnMouseButton(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		mouseHold = true;
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		mouseHold = false;
	}

	for (int i = 0; i < MouseButtonCalls.size(); i++)
	{
		MouseButtonCalls[i](button, action, mods);
	}
}

void InputManager::OnKeyButton(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_E && action == GLFW_PRESS) {
		vertexEditMode = !vertexEditMode;
	}
	
	if (action == GLFW_PRESS) {
		keys[key] = true;
	}
	else if (action == GLFW_RELEASE) {
		keys[key] = false;
	}
	

	for (int i = 0; i < KeyButtonCalls.size(); i++)
	{
		KeyButtonCalls[i](key, scancode, action, mods);
	}
}

void InputManager::OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset) {
	for (int i = 0; i < MouseScrollCalls.size(); i++)
	{
		MouseScrollCalls[i](xoffset, yoffset);
	}
}