#include "SpringComponent.h"

SpringComponent::SpringComponent(Object* parent, float springConstant, float damping, float restLength) : Component(parent) {
	Name = "Spring Component";
	this->springConstant = springConstant;
	this->damping = damping;
	this->restLength = restLength;
}

void SpringComponent::ProcessInspectorUI() {
	ImGuiTreeNodeFlags root_flags = ImGuiTreeNodeFlags_OpenOnArrow |
		ImGuiTreeNodeFlags_OpenOnDoubleClick |
		ImGuiTreeNodeFlags_SpanAvailWidth |
		ImGuiTreeNodeFlags_DefaultOpen;
	if (ImGui::TreeNodeEx("Spring", root_flags)) {
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
					RemoveTopObject(bottomObject);
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
					RemoveBottomObject(bottomObject);
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

		ImGui::Text("Spring Constant ");
		ImGui::SameLine();
		ImGui::InputFloat("## Spring Constant", &springConstant, 0.0f, 0.0f, "%.3f N/m");
		ImGui::Text("Damping ");
		ImGui::SameLine();
		ImGui::InputFloat("## Damping", &damping, 0.0f, 0.0f, "%.3f Ns/m");
		ImGui::Text("Rest Length ");
		ImGui::SameLine();
		ImGui::InputFloat("## Rest Length", &restLength, 0.0f, 0.0f, "%.3f m");

		if (springForceBot != nullptr) {
			springForceBot->springConstant = springConstant;
			springForceBot->damping = damping;
			springForceBot->restLength = restLength;
		}

		if (springForceTop != nullptr) {
			springForceTop->springConstant = springConstant;
			springForceTop->damping = damping;
			springForceTop->restLength = restLength;
		}

		ImGui::TreePop();
	}
}

void SpringComponent::AddTopObject(Object* object) {
	topObject = object;


	if (springForceTop == nullptr) {
		springForceTop = new SpringForce(topObject, bottomObject, springConstant, damping, restLength);
	}
	else {
		springForceTop->thisObject = topObject;
	}

	if (springForceBot != nullptr) {
		springForceBot->otherObject = topObject;
	}

	if (!object->HasComponent<PhysicsComponent>()) return;

	PhysicsEngine::getInstance().RegisterForce(object, springForceTop);
}

void SpringComponent::AddBottomObject(Object* object) {
	bottomObject = object;

	if (springForceBot == nullptr) {
		springForceBot = new SpringForce(bottomObject, topObject, springConstant, damping, restLength);
	}
	else {
		springForceBot->thisObject = bottomObject;
	}

	if (springForceTop != nullptr) {
		springForceTop->otherObject = bottomObject;
	}

	if (!object->HasComponent<PhysicsComponent>()) return;

	PhysicsEngine::getInstance().RegisterForce(object, springForceBot);
}

void SpringComponent::RemoveTopObject(Object* object) {
	if (object == nullptr || !object->HasComponent<PhysicsComponent>() || springForceTop == nullptr) return;

	PhysicsEngine::getInstance().UnRegisterForce(object, springForceTop);
	topObject = nullptr;
}

void SpringComponent::RemoveBottomObject(Object* object) {
	if (object == nullptr || !object->HasComponent<PhysicsComponent>() || springForceBot == nullptr) return;

	PhysicsEngine::getInstance().UnRegisterForce(object, springForceBot);
	
	bottomObject = nullptr;
}