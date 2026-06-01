#include "Hierarchy.h"
#include "ObjectManager.h"

Hierarchy::Hierarchy(std::string name) {
	this->name = name;
}

void Hierarchy::ProcessWindow() {
	ImGui::Begin(name.c_str());

	ImGuiTreeNodeFlags root_flags = ImGuiTreeNodeFlags_OpenOnArrow |
		ImGuiTreeNodeFlags_OpenOnDoubleClick |
		ImGuiTreeNodeFlags_SpanAvailWidth;
	
	if (ImGui::TreeNodeEx("Root", root_flags)) {
		std::vector<std::unique_ptr<Object>>* obj = &(ObjectManager::getInstance().allObjects);
		for (int i = 0; i < obj->size(); i++)
		{
			if (!(*obj)[i]->hidden) {
				ImGuiTreeNodeFlags item_flags = ImGuiTreeNodeFlags_Leaf |
					ImGuiTreeNodeFlags_NoTreePushOnOpen |
					ImGuiTreeNodeFlags_SpanAvailWidth;

				if (EditorManager::getInstance().selectedObject == (*obj)[i].get()) {
					item_flags |= ImGuiTreeNodeFlags_Selected;
				}
				else if ((*obj)[i]->HasComponent<VertexComponent>()) {
					(*obj)[i]->GetComponent<VertexComponent>()->SetEnabled(false);
				}


				std::string name = "Object " + std::to_string(i);
				ImGui::TreeNodeEx(name.c_str(), item_flags);
				if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
					(*obj)[i]->GetComponent<MouseInteractComponent>()->SetSelectedPolygon((*obj)[i].get(), true);
				}

			}
		}
		ImGui::TreePop();
	}

	ImGui::End();
}
