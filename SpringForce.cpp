#include "SpringForce.h"
#include "ObjectManager.h"

SpringForce::SpringForce(Object* thisObject, Object* otherObject, float springConstant, float damping, float restLength) {
	this->thisObject = thisObject;
	this->otherObject = otherObject;
	this->springConstant = springConstant;
	this->damping = damping;
	this->restLength = restLength;
}


void SpringForce::updateForce(Object* object, float delta) {
	if (otherObject == nullptr) {
		return;
	}

	if (!otherObject->HasComponent<TransformComponent>()) {
		return;
	}

	glm::vec3 thisPos = object->GetComponent<TransformComponent>()->GetWorldPosition();
	glm::vec3 otherPos = otherObject->GetComponent<TransformComponent>()->GetWorldPosition();
	glm::vec3 thisVel = object->GetComponent<PhysicsComponent>()->velocity;
	glm::vec3 otherVel = glm::vec3(0);

	if (otherObject->HasComponent<PhysicsComponent>()) {
		otherVel = otherObject->GetComponent<PhysicsComponent>()->velocity;
	}

	glm::vec3 d = thisPos - otherPos;
	glm::vec3 dir = glm::normalize(d);
	float length = glm::length(d);

	glm::vec3 springForce = -springConstant * (length - restLength) * dir;

	Force = springForce - damping * (thisVel - otherVel);
	
	object->GetComponent<PhysicsComponent>()->AddForce(Force);
}

void SpringForce::processDisplay(int index) {
	ImGuiTreeNodeFlags root_flags = ImGuiTreeNodeFlags_OpenOnArrow |
		ImGuiTreeNodeFlags_OpenOnDoubleClick |
		ImGuiTreeNodeFlags_SpanAvailWidth |
		ImGuiTreeNodeFlags_DefaultOpen;
	if (ImGui::TreeNodeEx((void*)(intptr_t)index, root_flags, "Spring", index)) {
		ImGui::Text("Linked Object ");
		char selected_item_name[128] = "None (Click to choose...)";

		if (otherObject != nullptr) {
			#if defined(_MSC_VER)
				strcpy_s(selected_item_name, otherObject->name.c_str());
			#else
				strncpy(selected_item_name, otherObject->name.c_str(), sizeof(selected_item_name) - 1);
			#endif
		}


		ImGui::InputText("##Field", selected_item_name, IM_ARRAYSIZE(selected_item_name), ImGuiInputTextFlags_ReadOnly);

		if (ImGui::IsItemHovered()) {
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
		}

		if (ImGui::IsItemClicked()) {
			ImGui::OpenPopup("Object selection");
		}

		if (ImGui::BeginPopup("Object selection")) {
			ImGui::TextDisabled("## Select Linked Object");
			ImGui::Separator();

			for (int i = 0; i < ObjectManager::getInstance().allObjects.size(); i++)
			{
				if (ObjectManager::getInstance().allObjects[i]->hidden || ObjectManager::getInstance().allObjects[i].get() == thisObject) continue;

				std::string objName = ObjectManager::getInstance().allObjects[i]->name;
				 if (ImGui::Selectable(objName.c_str())) {
					 otherObject = ObjectManager::getInstance().allObjects[i].get();
					#if defined(_MSC_VER)
						strcpy_s(selected_item_name, objName.c_str());
					#else
						strncpy(selected_item_name, objName.c_str(), sizeof(selected_item_name) - 1);
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
		ImGui::Text("Spring Force ");
		ImGui::SameLine();
		float force[] = { Force.x, Force.y };
		ImGui::InputFloat2("## Spring Force", force, "%.3f N", ImGuiInputTextFlags_ReadOnly);
		ImGui::TreePop();
	}
}