#include "ObjectLinkComponent.h"
#include "ObjectManager.h"
 
ObjectLinkComponent::ObjectLinkComponent(Object* parent) : Component(parent) {

}

void ObjectLinkComponent::ObjectSelectUI() {
	ImGui::Text("Linked Object Top ");
	char selected_item_name_top[128] = "None (Click to choose...)";

	if (topObject != nullptr) {
#if defined(_MSC_VER)
		strcpy_s(selected_item_name_top, topObject->name.c_str());
#else
		strncpy(selected_item_name_top, topObject->name.c_str(), sizeof(selected_item_name_top) - 1);
#endif
	}


	ImGui::InputText("##Object Select field top", selected_item_name_top, IM_ARRAYSIZE(selected_item_name_top), ImGuiInputTextFlags_ReadOnly);

	if (ImGui::IsItemHovered()) {
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
	}

	if (ImGui::IsItemClicked()) {
		ImGui::OpenPopup("Object selection top");
	}

	if (ImGui::BeginPopup("Object selection top")) {
		ImGui::TextDisabled("## Select Linked Object Top");
		ImGui::Separator();

		for (int i = 0; i < ObjectManager::getInstance().allObjects.size(); i++)
		{
			Object* currentObject = ObjectManager::getInstance().allObjects[i].get();
			if (currentObject->hidden || currentObject == parent || currentObject == bottomObject) continue;

			std::string objName = ObjectManager::getInstance().allObjects[i]->name;
			if (ImGui::Selectable(objName.c_str())) {
				if (topObject != nullptr) {
					RemoveTopObject();
				}
				AddTopObject(ObjectManager::getInstance().allObjects[i].get());
#if defined(_MSC_VER)
				strcpy_s(selected_item_name_top, objName.c_str());
#else
				strncpy(selected_item_name_top, objName.c_str(), sizeof(selected_item_name_top) - 1);
#endif
			}
		}

		ImGui::EndPopup();
	}


	ImGui::Text("Linked Object Bottom ");
	char selected_item_name_bot[128] = "None (Click to choose...)";

	if (bottomObject != nullptr) {
#if defined(_MSC_VER)
		strcpy_s(selected_item_name_bot, bottomObject->name.c_str());
#else
		strncpy(selected_item_name_bot, bottomObject->name.c_str(), sizeof(selected_item_name_bot) - 1);
#endif
	}


	ImGui::InputText("##Object Select field bottom", selected_item_name_bot, IM_ARRAYSIZE(selected_item_name_bot), ImGuiInputTextFlags_ReadOnly);

	if (ImGui::IsItemHovered()) {
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
	}

	if (ImGui::IsItemClicked()) {
		ImGui::OpenPopup("Object selection bottom");
	}

	if (ImGui::BeginPopup("Object selection bottom")) {
		ImGui::TextDisabled("## Select Linked Object Bottom");
		ImGui::Separator();

		for (int i = 0; i < ObjectManager::getInstance().allObjects.size(); i++)
		{
			Object* currentObject = ObjectManager::getInstance().allObjects[i].get();
			if (currentObject->hidden || currentObject == parent || currentObject == topObject) continue;

			std::string objName = ObjectManager::getInstance().allObjects[i]->name;
			if (ImGui::Selectable(objName.c_str())) {
				if (bottomObject != nullptr) {
					RemoveBottomObject();
				}
				AddBottomObject(ObjectManager::getInstance().allObjects[i].get());
#if defined(_MSC_VER)
				strcpy_s(selected_item_name_bot, objName.c_str());
#else
				strncpy(selected_item_name_bot, objName.c_str(), sizeof(selected_item_name_bot) - 1);
#endif
			}
		}

		ImGui::EndPopup();
	}
}

void ObjectLinkComponent::OnDelete() {
	if (topObject != nullptr) {
		RemoveTopObject();
	}
	if (bottomObject != nullptr) {
		RemoveBottomObject();
	}
}

void ObjectLinkComponent::AddTopObject(Object* obj) {
	topObject = obj;
	TopObjectOnDeleteID = topObject->AddOnDeleteCallback([this]() {this->RemoveTopObject();});
}

void ObjectLinkComponent::AddBottomObject(Object* obj) {
	bottomObject = obj;
	BotObjectOnDeleteID = bottomObject->AddOnDeleteCallback([this]() {this->RemoveBottomObject();});
}

void ObjectLinkComponent::RemoveTopObject() {
	if (topObject == nullptr) return;
	topObject->RemoveOnDeleteCallback(TopObjectOnDeleteID);
	topObject = nullptr;
}

void ObjectLinkComponent::RemoveBottomObject() {
	if (bottomObject == nullptr) return;
	bottomObject->RemoveOnDeleteCallback(BotObjectOnDeleteID);
	bottomObject = nullptr;
}