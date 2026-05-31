#include "EditorManager.h"

void EditorManager::Setup(GLFWwindow* window) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, false);
	ImGui_ImplOpenGL3_Init("#version 330");

	Windows.push_back(new Inspector("Inspector"));
	Windows.push_back(new EngineStatus("Status"));
}

void EditorManager::SetSelectedObject(Object* object) {
	this->selectedObject = object;
}

void EditorManager::ProcessEditor() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGuiIO& io = ImGui::GetIO();
	WindowHovered = io.WantCaptureMouse;
	WindowTyped = io.WantCaptureKeyboard;

	for (int i = 0; i < Windows.size(); i++)
	{
		Windows[i]->ProcessWindow();
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}