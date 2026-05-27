#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include <glm/glm.hpp>
#include<iostream>
#include "Polygon.h"
#include "InputManager.h"
#include "Renderer.h"
#include "Shader.h"
#include "VertexPoint.h"

std::vector<float> vertices;
std::vector<Object> allObjects;
std::vector<VertexPoint> vertexPoints; // temporary storage for vertex points before they are assigned to a polygon
std::vector<Polygon> allPolygons;
Renderer renderer = Renderer(&allObjects);

void cursorPressedCallback(int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		if (!InputManager::vertexEditMode) {
			vertices.push_back(InputManager::glX);
			vertices.push_back(InputManager::glY);
			vertices.push_back(0.0f); // Z coordinate
			vertices.push_back(1.0f); // R
			vertices.push_back(1.0f); // G
			vertices.push_back(1.0f); // B
			vertices.push_back(InputManager::glX); // U
			vertices.push_back(InputManager::glY); // V

			VertexPoint pointIndicator = VertexPoint(InputManager::glX, InputManager::glY, Shader("vertex.txt", "fragment.txt"));
			vertexPoints.push_back(pointIndicator);
			allObjects.push_back(pointIndicator);
		}
	}
}

void keyPressed(int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		Polygon poly = Polygon(vertices, Shader("vertex.txt", "fragment.txt"), std::vector<std::string> {"wall.jpg"});
		std::shared_ptr <Polygon> polyPtr = std::make_shared<Polygon>(poly);
		allObjects.push_back(poly);
		allPolygons.push_back(poly);
		vertices.clear();
		poly.GetComponent<VertexComponent>()->SetVertexPoints(vertexPoints);
		poly.GetComponent<TransformComponent>()->SetTransform(Camera::getInstance().viewMatrixInverse);
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
	if (key == GLFW_KEY_H && action == GLFW_PRESS) {
		for (int i = 0; i < allPolygons.size(); i++)
		{
			allPolygons[i].GetComponent<VertexComponent>()->SetEnabled(!(allPolygons[i].GetComponent<VertexComponent>()->Enabled));
		}
	}
}

int main() {
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 800, "Game engine", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return 0;
	}
	glfwMakeContextCurrent(window);
	gladLoadGL();

	InputManager::getInstance().Setup(window);
	InputManager::getInstance().SetKeyButtonCallback(keyPressed);
	InputManager::getInstance().SetMouseButtonCallback(cursorPressedCallback);

	Camera::getInstance().Setup();

	float prev_t = 0;

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		
		Camera::getInstance().ProcessCamera(glfwGetTime() - prev_t);
		prev_t = glfwGetTime();

		for (int i = 0; i < allPolygons.size(); i++)
		{
			if (allPolygons[i].HasComponent<TransformComponent>()) {
				TransformComponent* transComp = allPolygons[i].GetComponent<TransformComponent>();
				//transComp->Translate(glm::vec3(0.5, 0, 0));
				transComp->Rotate((float)glfwGetTime() * 0.5f);
				//transComp->Scale(glm::vec3(2, 2, 1));
			}
		}

		glad_glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		renderer.Draw();

		glfwSwapBuffers(window);
	}

	return 0;
}