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
	InputManager::getInstance().SetKeyButtonCallback(keyPressed, 999);
	InputManager::getInstance().SetMouseButtonCallback(cursorPressedCallback, 999);

	EngineManager::getInstance().Setup(window);
	PhysicsEngine::getInstance().Setup(&ObjectManager::getInstance().allObjects);

	Camera::getInstance().Setup();
	renderer.SetupGrid();

	float prev_t = 0;
	float physicsAccumulator = 0.0f;
	const float PHYSICS_STEP = 1.0f / 60.0f;

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float now = glfwGetTime();
		float delta = now - prev_t;
		prev_t = now;
		if (delta > 0.1f) delta = 0.1f;
		physicsAccumulator += delta;
		Camera::getInstance().ProcessCamera(delta);
		while (physicsAccumulator >= PHYSICS_STEP) {
			PhysicsEngine::getInstance().ProcessPhysics(PHYSICS_STEP);
			physicsAccumulator -= PHYSICS_STEP;
		}
		EngineManager::getInstance().ProcessEngine(delta);
		ObjectManager::getInstance().ProcessObjects(delta);
		prev_t = glfwGetTime();

		glad_glClearColor(0.235f, 0.239f, 0.216f, 1.0f);
		//glad_glClearColor(1, 1, 1, 1);
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