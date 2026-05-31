#include "Inspector.h"
#include "EditorManager.h"
#include "MouseInteractComponent.h"

Inspector::Inspector(std::string main) : EditorWindow(main) {

}

void Inspector::ProcessWindow() {
	ImGui::Begin(name.c_str());
	
	
	if (EditorManager::getInstance().selectedObject != nullptr) {
		Object* selected = EditorManager::getInstance().selectedObject;
		for (int i = 0; i < selected->components.size(); i++)
		{
			if (ImGui::TreeNode(selected->components[i]->Name.c_str())) {
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