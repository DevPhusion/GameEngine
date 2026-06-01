#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<iostream>
#include "Polygon.h"
#include "InputManager.h"
#include "Renderer.h"
#include "PhysicsEngine.h"
#include "EngineManager.h"
#include "EditorManager.h"
#include "ObjectManager.h"
#include "Gravity.h"
#include "Drag.h"
#include "Shader.h"
#include "VertexPoint.h"
#include "MouseDrag.h"


Renderer renderer = Renderer(&ObjectManager::getInstance().allObjects);

void cursorPressedCallback(int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		ObjectManager::getInstance().AddPolygonVertex();
	}
}

void keyPressed(int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_P && action == GLFW_PRESS && EngineManager::getInstance().EngineInteractMode == EngineManager::InteractMode::AddVertex) {
		ObjectManager::getInstance().AddPolygon();
	}
	if (key == GLFW_KEY_R) {
		if (action == GLFW_PRESS) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else if (action == GLFW_RELEASE) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}
}

int main() {
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(1920, 1080, "Game engine", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return 0;
	}
	glfwMakeContextCurrent(window);
	gladLoadGL();

	EditorManager::getInstance().Setup(window);

	InputManager::getInstance().Setup(window);
	InputManager::getInstance().SetKeyButtonCallback(keyPressed);
	InputManager::getInstance().SetMouseButtonCallback(cursorPressedCallback);

	EngineManager::getInstance().Setup(window);
	PhysicsEngine::getInstance().Setup(&ObjectManager::getInstance().allObjects);

	Camera::getInstance().Setup();

	float prev_t = 0;

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float delta = glfwGetTime() - prev_t;
		Camera::getInstance().ProcessCamera(delta);
		PhysicsEngine::getInstance().ProcessPhysics(delta);
		EngineManager::getInstance().ProcessEngine(delta);
		prev_t = glfwGetTime();

		glad_glClearColor(0.235f, 0.239f, 0.216f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		renderer.Draw();

		EditorManager::getInstance().ProcessEditor();

		glfwSwapBuffers(window);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}