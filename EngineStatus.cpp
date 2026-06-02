#include "EngineStatus.h"

EngineStatus::EngineStatus(std::string main) : EditorWindow(main) {
	EngineManager::getInstance().AddInteractModeChangedEvent([this]() {this->OnInteractModeChanged();});
	OnInteractModeChanged();
}

void EngineStatus::ProcessWindow() {
	if (hidden) return;

	ImGui::SetNextWindowPos(ImVec2(1510, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(400, 130), ImGuiCond_FirstUseEver);

	ImGui::Begin(name.c_str());

	std::string fpsText = std::to_string(EngineManager::getInstance().fps) + " FPS";
	ImGui::Text(fpsText.c_str());
	ImGui::Text(InteractModeText.c_str());
	ImGui::Text("Physics: ");
	ImGui::SameLine();
	if (EngineManager::getInstance().EnginePhysicsMode == EngineManager::PhysicsMode::Pause) {
		if (ImGui::Button("Run")) {
			EngineManager::getInstance().SwitchPhysicsMode(EngineManager::PhysicsMode::Simulate);
		}
	}
	else if (EngineManager::getInstance().EnginePhysicsMode == EngineManager::PhysicsMode::Simulate){
		if (ImGui::Button("Pause")) {
			EngineManager::getInstance().SwitchPhysicsMode(EngineManager::PhysicsMode::Pause);
		}
	}

	ImGui::End();
}

void EngineStatus::OnInteractModeChanged() {
	if (EngineManager::getInstance().EngineInteractMode == EngineManager::InteractMode::AddVertex) {
		InteractModeText = "Interact Mode: ADD VERTEX";
	}
	if (EngineManager::getInstance().EngineInteractMode == EngineManager::InteractMode::EditorSelect) {
		InteractModeText = "Interact Mode: MOUSE INTERACT";
	}
}