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
#include "Gravity.h"
#include "Drag.h"
#include "Shader.h"
#include "VertexPoint.h"
#include "MouseDrag.h"

std::vector<float> vertices;
std::vector<std::unique_ptr<Object>> allObjects;
std::vector<VertexPoint*> vertexPoints; // temporary storage for vertex points before they are assigned to a polygon
Renderer renderer = Renderer(&allObjects);

void cursorPressedCallback(int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		if (EngineManager::getInstance().EngineInteractMode == EngineManager::InteractMode::AddVertex) {
			vertices.push_back(InputManager::glX);
			vertices.push_back(InputManager::glY);
			vertices.push_back(0.0f); // Z coordinate
			vertices.push_back(InputManager::glX); // U
			vertices.push_back(InputManager::glY); // V

			std::unique_ptr<VertexPoint> pointIndicator = std::make_unique<VertexPoint>(InputManager::glX, InputManager::glY, Shader("vertex.txt", "fragment.txt"));
			vertexPoints.push_back(pointIndicator.get());
			allObjects.push_back(std::move(pointIndicator));
		}
	}
}

void keyPressed(int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_P && action == GLFW_PRESS && EngineManager::getInstance().EngineInteractMode == EngineManager::InteractMode::AddVertex) {
		if (vertexPoints.size() < 3) {
			std::cout << "Invalid polygon" << std::endl;
			return;
		}

		std::unique_ptr<Polygon> poly = std::make_unique<Polygon>(vertices, Shader("vertex.txt", "fragment.txt"), std::vector<std::string> {"floorTiled.png"});

		auto* vc = poly->GetComponent<VertexComponent>();
		auto* tc = poly->GetComponent<TransformComponent>();
		auto* pc = poly->GetComponent<PhysicsComponent>();

		pc->inverseMass = 1;
		PhysicsEngine::getInstance().RegisterForce(poly.get(), new Gravity(glm::vec3(0, -9.8f, 0)));
		PhysicsEngine::getInstance().RegisterForce(poly.get(), new Drag(0.0f, 0.002f));

		vc->SetVertexPoints(vertexPoints);
		tc->SetOriginTransform(Camera::getInstance().viewMatrixInverse);
		allObjects.push_back(std::move(poly));
		vertices.clear();
		vertexPoints.clear();
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
	PhysicsEngine::getInstance().Setup(&allObjects);

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