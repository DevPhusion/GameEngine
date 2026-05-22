#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include <glm/glm.hpp>
#include<iostream>
#include "Renderer.h"
#include "Shader.h"
#include "VertexPoint.h"

std::vector<float> vertices;
std::vector<VertexPoint> vertexPoints; // temporary storage for vertex points before they are assigned to a polygon
std::vector<VertexPoint> allVertexPoints; // all vertex points
Renderer renderer = Renderer(std::vector<Polygon> {});
VertexPoint* currentVertexPoint = nullptr;
int vertexIndex = 0;
bool mouseHeld = false;
bool vertexEditMode = false;

float glX, glY;

static void cursorPressedCallback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		mouseHeld = true;

		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);
		int windowWidth, windowHeight;
		glfwGetWindowSize(window, &windowWidth, &windowHeight);
		glX = (2.0f * static_cast<float>(mouseX) / windowWidth) - 1.0f;
		glY = 1.0f - (2.0f * static_cast<float>(mouseY) / windowHeight);

		if (vertexEditMode) {
			for (int i = 0; i < allVertexPoints.size(); i++)
			{
				float distance = sqrt(pow(glX - allVertexPoints[i].x, 2) + pow(glY - allVertexPoints[i].y, 2));
				if (distance < 0.05f) {
					currentVertexPoint = &allVertexPoints[i];
					std::cout << "Selected vertex at index " << i << " with position (" << allVertexPoints[i].x << ", " << allVertexPoints[i].y << ")" << std::endl;
					break;
				}
			}
		}
		else {
			vertices.push_back(glX);
			vertices.push_back(glY);
			vertices.push_back(0.0f); // Z coordinate
			vertices.push_back(1.0f); // R
			vertices.push_back(1.0f); // G
			vertices.push_back(1.0f); // B
			vertices.push_back(glX); // U
			vertices.push_back(glY); // V

			VertexPoint pointIndicator = VertexPoint(glX, glY, Shader("vertex.txt", "fragment.txt"), vertexIndex, &renderer);
			vertexIndex += 1;
			vertexPoints.push_back(pointIndicator);
		}
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		mouseHeld = false;
		currentVertexPoint = nullptr;
	}
}

static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
	int windowWidth, windowHeight;
	glfwGetWindowSize(window, &windowWidth, &windowHeight);
	glX = (2.0f * static_cast<float>(xpos) / windowWidth) - 1.0f;
	glY = 1.0f - (2.0f * static_cast<float>(ypos) / windowHeight);
}

static void keyPressed(GLFWwindow * window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		Polygon poly = Polygon(vertices, Shader("vertex.txt", "fragment.txt"), std::vector<std::string> {"wall.jpg"});
		std::shared_ptr <Polygon> polyPtr = std::make_shared<Polygon>(poly);
		renderer.AddPolygon(poly);
		vertices.clear();
		for (int i = 0; i < vertexPoints.size(); i++)
		{
			vertexPoints[i].SetParentPolygon(polyPtr);
			allVertexPoints.push_back(vertexPoints[i]);
		}
		vertexPoints.clear();
		vertexIndex = 0;
	}
	if (key == GLFW_KEY_E && action == GLFW_PRESS) {
		vertexEditMode = !vertexEditMode;
		if (vertexEditMode == false) {
			currentVertexPoint = nullptr;
		}
		std::cout << "Vertex edit mode: " << vertexEditMode << std::endl;
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

	glfwSetMouseButtonCallback(window, cursorPressedCallback);
	glfwSetCursorPosCallback(window, cursorPositionCallback);
	glfwSetKeyCallback(window, keyPressed);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		if (vertexEditMode && currentVertexPoint != nullptr) {
			currentVertexPoint->UpdatePosition(glX, glY);
		}

		glad_glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		renderer.Draw();

		glfwSwapBuffers(window);

	}

	return 0;
}