#include "Inspector.h"
#include "EditorManager.h"
#include "MouseInteractComponent.h"

Inspector::Inspector(std::string main) : EditorWindow(main) {

}

void Inspector::ProcessWindow() {
	if (hidden) return;

	ImGui::SetNextWindowPos(ImVec2(1510, 150), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(400, 880), ImGuiCond_FirstUseEver);

	ImGui::Begin(name.c_str());
	
	if (EditorManager::getInstance().selectedObject != nullptr) {
		Object* selected = EditorManager::getInstance().selectedObject;

		char objectNameBuffer[256];

		#if defined(_MSC_VER)
				strcpy_s(objectNameBuffer, selected->name.c_str());
		#else
				strncpy(objectNameBuffer, selected->name.c_str(), sizeof(selected_item_name) - 1);
		#endif

		ImGui::Text("Name ");
		ImGui::SameLine();

		if (ImGui::InputText("##ObjectName", objectNameBuffer, 128)) {
			selected->name = std::string(objectNameBuffer);
		}

		for (int i = 0; i < selected->components.size(); i++)
		{
			ImGuiTreeNodeFlags root_flags = ImGuiTreeNodeFlags_OpenOnArrow |
				ImGuiTreeNodeFlags_OpenOnDoubleClick |
				ImGuiTreeNodeFlags_SpanAvailWidth |
				ImGuiTreeNodeFlags_DefaultOpen;

			if (ImGui::TreeNodeEx(selected->components[i]->Name.c_str(), root_flags)) {
				ImGui::Text("Enable ");
				ImGui::SameLine();

				if (ImGui::Checkbox("##", &selected->components[i]->Enabled)) {
					selected->components[i]->SetEnabled(selected->components[i]->Enabled);
				}

				EditorManager::getInstance().selectedObject->components[i]->ProcessInspectorUI();
				ImGui::TreePop();
			}

		}
	}
	
	ImGui::End();
	
}