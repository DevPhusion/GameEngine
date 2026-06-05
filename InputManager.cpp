#include "InputManager.h"

float InputManager::glX = 0.0f;
float InputManager::glY = 0.0f;
bool InputManager::mouseLeftHold = false;
bool InputManager::mouseRightHold = false;

std::unordered_map<int, std::function<void(int, int, int)>> InputManager::MouseButtonCalls = {};
std::unordered_map <int, std::function<void(double, double)>> InputManager::CursorPositionCalls = {};
std::unordered_map <int, std::function<void(int, int, int, int)>> InputManager::KeyButtonCalls = {};
std::unordered_map <int, std::function<void(double, double)>> InputManager::MouseScrollCalls = {};

std::unordered_map<int, bool> InputManager::keys = {};

void InputManager::Setup(GLFWwindow* window) {
	this->window = window;
	glfwSetMouseButtonCallback(this->window, OnMouseButton);
	glfwSetCursorPosCallback(this->window, OnCursorPosition);
	glfwSetKeyCallback(this->window, OnKeyButton);
	glfwSetScrollCallback(this->window, OnMouseScroll);

	glfwSetCharCallback(this->window, ImGui_ImplGlfw_CharCallback);
}

int InputManager::SetMouseButtonCallback(std::function<void(int, int, int)> func) {
	CurrentMouseButtonID += 1;
	MouseButtonCalls[CurrentMouseButtonID] = func;
	return CurrentMouseButtonID;
}

int InputManager::SetCursorPositionCallback(std::function<void(double, double)> func) {
	CurrentCursorPositionID += 1;
	CursorPositionCalls[CurrentCursorPositionID] = func;
	return CurrentCursorPositionID;
}

int InputManager::SetKeyButtonCallback(std::function<void(int, int, int, int)> func) {
	CurrentKeyButtonID += 1;
	KeyButtonCalls[CurrentKeyButtonID] = func;
	return CurrentKeyButtonID;
}

int InputManager::SetMouseScrollCallback(std::function<void(double, double)> func) {
	CurrentMouseScrollID += 1;
	MouseScrollCalls[CurrentMouseScrollID] = func;
	return CurrentMouseScrollID;
}

void InputManager::RemoveMouseButtonCallback(int ID) {
	MouseButtonCalls.erase(ID);
}

void InputManager::RemoveCursorPositionCallback(int ID) {
	CursorPositionCalls.erase(ID);
}

void InputManager::RemoveKeyButtonCallback(int ID) {
	KeyButtonCalls.erase(ID);
}

void InputManager::RemoveMouseScrollCallback(int ID) {
	MouseScrollCalls.erase(ID);
}

void InputManager::OnCursorPosition(GLFWwindow* window, double xpos, double ypos) {
	ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);

	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);
	int windowWidth, windowHeight;
	glfwGetWindowSize(window, &windowWidth, &windowHeight);
	glX = (2.0f * static_cast<float>(mouseX) / windowWidth) - 1.0f;
	glY = 1.0f - (2.0f * static_cast<float>(mouseY) / windowHeight);

	glX = glX * EngineManager::getInstance().aspectRatio;

	for (const auto& [id, func] : CursorPositionCalls) {
		func(xpos, ypos);
	}

}

void InputManager::OnMouseButton(GLFWwindow* window, int button, int action, int mods) {
	ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		mouseLeftHold = true;
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		mouseLeftHold = false;
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		mouseRightHold = true;
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
		mouseRightHold = false;
	}

	if (EditorManager::getInstance().WindowHovered) {
		return;
	}

	for (const auto& [id, func] : MouseButtonCalls) {
		func(button, action, mods);
	}
}

void InputManager::OnKeyButton(GLFWwindow* window, int key, int scancode, int action, int mods) {
	ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);

	if (ImGui::GetIO().WantTextInput) {
		return;
	}

	if (action == GLFW_PRESS) {
		keys[key] = true;
	}
	else if (action == GLFW_RELEASE) {
		keys[key] = false;
	}
	
	if (EditorManager::getInstance().WindowTyped) {
		return;
	}

	for (const auto& [id, func] : KeyButtonCalls) {
		func(key, scancode, action, mods);
	}
}

void InputManager::OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset) {
	ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);

	if (EditorManager::getInstance().WindowHovered) {
		return;
	}

	for (const auto& [id, func] : MouseScrollCalls) {
		func(xoffset, yoffset);
	}
}