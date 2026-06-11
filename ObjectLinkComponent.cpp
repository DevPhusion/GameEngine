#include "ObjectLinkComponent.h"
#include "ObjectManager.h"
 
ObjectLinkComponent::ObjectLinkComponent(Object* parent) : Component(parent) {
	EngineManager::getInstance().AddPhysicsModeChangedEvent([this]() {this->OnPhysicsModeChange();});
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

	if (topConnectDisplay != nullptr && ImGui::Checkbox("Use Object's Center ## Top", &TopUseCenter)) {
		if (TopUseCenter) {
			topConnectDisplay->GetComponent<TransformComponent>()->UpdateWorldPosition(topObject->GetComponent<TransformComponent>()->GetWorldPosition());
			topConnectDisplay->GetComponent<RenderComponent>()->Enabled = false;
		}
	}

	if (!TopUseCenter && topConnectDisplay != nullptr && topConnectDisplay->GetComponent<RenderComponent>()->Enabled == false) {
		if (ImGui::Button("Change Connection Point ## top")) {
			if (!PosSetTop) {
				topConnectDisplay->GetComponent<TransformComponent>()->UpdateWorldPosition(topObject->GetComponent<TransformComponent>()->GetWorldPosition());
				PosSetTop = true;
			}
			topConnectDisplay->GetComponent<MouseInteractComponent>()->Enabled = true;
			topConnectDisplay->GetComponent<RenderComponent>()->Enabled = true;
		}
	}
	else if (!TopUseCenter && topConnectDisplay != nullptr) {
		if (ImGui::Button("Confirm ## top")) {
			topConnectDisplay->GetComponent<MouseInteractComponent>()->Enabled = false;
			topConnectDisplay->GetComponent<RenderComponent>()->Enabled = false;
		}
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

	if (bottomConnectDisplay != nullptr && ImGui::Checkbox("Use Object's Center ## Bot", &BotUseCenter)) {
		if (BotUseCenter) {
			bottomConnectDisplay->GetComponent<TransformComponent>()->UpdateWorldPosition(bottomObject->GetComponent<TransformComponent>()->GetWorldPosition());
			bottomConnectDisplay->GetComponent<RenderComponent>()->Enabled = false;
		}
	}

	if (!BotUseCenter && bottomConnectDisplay != nullptr && bottomConnectDisplay->GetComponent<RenderComponent>()->Enabled == false) {
		if (ImGui::Button("Change Connection Point ## bottom")) {
			if (!PosSetBot) {
				PosSetBot = true;
				bottomConnectDisplay->GetComponent<TransformComponent>()->UpdateWorldPosition(bottomObject->GetComponent<TransformComponent>()->GetWorldPosition());
			}
			bottomConnectDisplay->GetComponent<MouseInteractComponent>()->Enabled = true;
			bottomConnectDisplay->GetComponent<RenderComponent>()->Enabled = true;
		}
	}
	else if (!BotUseCenter && bottomConnectDisplay != nullptr) {
		if (ImGui::Button("Confirm ## bottom")) {
			bottomConnectDisplay->GetComponent<MouseInteractComponent>()->Enabled = false;
			bottomConnectDisplay->GetComponent<RenderComponent>()->Enabled = false;
		}
	}
}

void ObjectLinkComponent::OnDelete() {
	if (topObject != nullptr) {
		RemoveTopObject();
		ObjectManager::getInstance().RemoveObject(topConnectDisplay);
		
	}
	if (bottomObject != nullptr) {
		RemoveBottomObject();
		ObjectManager::getInstance().RemoveObject(bottomConnectDisplay);
	}
}

void ObjectLinkComponent::AddTopObject(Object* obj) {
	topObject = obj;
	TopObjectOnDeleteID = topObject->AddOnDeleteCallback([this]() {this->RemoveTopObject();});

	topConnectPoint = obj->GetComponent<RenderComponent>()->GetCenter();
	TopObjectOnTransformID = obj->GetComponent<TransformComponent>()->AddTransformCallback([this]() {this->OnTopObjectUpdatePos();});
	if (topConnectDisplay == nullptr) {
		float sizeY = 0.01f;
		float sizeX = 0.01f;

		std::vector<float> vertices = {
			-sizeX, -sizeY, 0.0f, 0.0f, 0.0f,
			+sizeX, -sizeY, 0.0f, 1.0f, 0.0f,
			+sizeX, +sizeY, 0.0f, 1.0f, 1.0f,
			-sizeX, +sizeY, 0.0f, 0.0f, 1.0f
		};

		std::unique_ptr<Object> display = std::make_unique<Object>(Shader("vertex.txt", "fragment.txt"));
		std::string name = parent->name + " Top Connect Display";
		display->name = name.c_str();
		display->AddComponent(std::make_unique<RenderComponent>(display.get(), vertices, display.get()->shader, "red.jpg"));
		display->GetComponent<RenderComponent>()->z_index = 999;
		display->AddComponent(std::make_unique<TransformComponent>(display.get(), display.get()->shader, display.get()->GetComponent<RenderComponent>()->GetCenter()));
		display->AddComponent(std::make_unique<MouseInteractComponent>(display.get(), false));
		display->hidden = true;
		display->GetComponent<RenderComponent>()->Enabled = false;
		display->GetComponent<MouseInteractComponent>()->Enabled = false;
		display->GetComponent<TransformComponent>()->UpdateWorldPosition(obj->GetComponent<TransformComponent>()->GetWorldPosition());
		display->GetComponent<TransformComponent>()->AddTransformCallback([this]() {this->OnTopDisplayUpdatePos();});
		display->GetComponent<MouseInteractComponent>()->Inspectable = false;
		topConnectDisplay = display.get();
		ObjectManager::getInstance().allObjects.push_back(std::move(display));
	}

	PosSetTop = false;
}

void ObjectLinkComponent::AddBottomObject(Object* obj) {
	bottomObject = obj;
	BotObjectOnDeleteID = bottomObject->AddOnDeleteCallback([this]() {this->RemoveBottomObject();});

	bottomConnectPoint = obj->GetComponent<RenderComponent>()->GetCenter();
	BotObjectOnTransformID = obj->GetComponent<TransformComponent>()->AddTransformCallback([this]() {this->OnBottomObjectUpdatePos();});
		
	if (bottomConnectDisplay == nullptr) {
		float sizeY = 0.01f;
		float sizeX = 0.01f;

		std::vector<float> vertices = {
			-sizeX, -sizeY, 0.0f, 0.0f, 0.0f,
			+sizeX, -sizeY, 0.0f, 1.0f, 0.0f,
			+sizeX, +sizeY, 0.0f, 1.0f, 1.0f,
			-sizeX, +sizeY, 0.0f, 0.0f, 1.0f
		};

		std::unique_ptr<Object> display = std::make_unique<Object>(Shader("vertex.txt", "fragment.txt"));
		std::string name = parent->name + " Bottom Connect Display";
		display->name = name.c_str();
		display->AddComponent(std::make_unique<RenderComponent>(display.get(), vertices, display.get()->shader, "red.jpg"));
		display->GetComponent<RenderComponent>()->z_index = 999;
		display->AddComponent(std::make_unique<TransformComponent>(display.get(), display.get()->shader, display.get()->GetComponent<RenderComponent>()->GetCenter()));
		display->AddComponent(std::make_unique<MouseInteractComponent>(display.get(), false));
		display->hidden = true;
		display->GetComponent<RenderComponent>()->Enabled = false;
		display->GetComponent<MouseInteractComponent>()->Enabled = false;
		display->GetComponent<TransformComponent>()->UpdateWorldPosition(obj->GetComponent<TransformComponent>()->GetWorldPosition());
		display->GetComponent<TransformComponent>()->AddTransformCallback([this]() {this->OnBottomDisplayUpdatePos();});
		display->GetComponent<MouseInteractComponent>()->Inspectable = false;
		bottomConnectDisplay = display.get();
		ObjectManager::getInstance().allObjects.push_back(std::move(display));
	}

	PosSetBot = false;
}

void ObjectLinkComponent::RemoveTopObject() {
	if (topObject == nullptr) return;
	topObject->RemoveOnDeleteCallback(TopObjectOnDeleteID);
	topObject->GetComponent<TransformComponent>()->RemoveTransformCallback(TopObjectOnTransformID);
	topConnectDisplay->GetComponent<RenderComponent>()->Enabled = false;
	topConnectDisplay->GetComponent<MouseInteractComponent>()->Enabled = false;
	topObject = nullptr;
}

void ObjectLinkComponent::RemoveBottomObject() {
	if (bottomObject == nullptr) return;
	bottomObject->RemoveOnDeleteCallback(BotObjectOnDeleteID);
	bottomObject->GetComponent<TransformComponent>()->RemoveTransformCallback(TopObjectOnTransformID);
	bottomConnectDisplay->GetComponent<RenderComponent>()->Enabled = false;
	bottomConnectDisplay->GetComponent<MouseInteractComponent>()->Enabled = false;
	bottomObject = nullptr;
}

void ObjectLinkComponent::OnTopObjectUpdatePos() {
	glm::vec3 worldPos = topObject->GetComponent<TransformComponent>()->ProjectToWorld(topConnectPoint);
	topConnectDisplay->GetComponent<TransformComponent>()->UpdateWorldPosition(worldPos);
}

void ObjectLinkComponent::OnBottomObjectUpdatePos() {
	glm::vec3 worldPos = bottomObject->GetComponent<TransformComponent>()->ProjectToWorld(bottomConnectPoint);
 	bottomConnectDisplay->GetComponent<TransformComponent>()->UpdateWorldPosition(worldPos);
}

void ObjectLinkComponent::OnTopDisplayUpdatePos() {
	glm::vec3 worldPos = topConnectDisplay->GetComponent<TransformComponent>()->GetWorldPosition();
	glm::vec3 localPos = topObject->GetComponent<TransformComponent>()->ProjectToWorld(worldPos, true);

	topConnectPoint = localPos;
}

void ObjectLinkComponent::OnBottomDisplayUpdatePos() {
	glm::vec3 worldPos = bottomConnectDisplay->GetComponent<TransformComponent>()->GetWorldPosition();
	glm::vec3 localPos = bottomObject->GetComponent<TransformComponent>()->ProjectToWorld(worldPos, true);

	bottomConnectPoint = localPos;
}

void ObjectLinkComponent::OnPhysicsModeChange() {
	if (EngineManager::getInstance().EnginePhysicsMode == EngineManager::PhysicsMode::Simulate) {
		if (topConnectDisplay != nullptr) {
			topConnectDisplay->GetComponent<RenderComponent>()->Enabled = false;
		}
		if (bottomConnectDisplay != nullptr) {
			bottomConnectDisplay->GetComponent<RenderComponent>()->Enabled = false;
		}
	}
}