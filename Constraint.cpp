#include "Constraint.h"
#include "ObjectManager.h"
#include "RenderComponent.h"
#include "TransformComponent.h"
#include "MouseInteractComponent.h"
#include "ConstraintComponent.h"
#include "EditorManager.h"

Constraint::Constraint(Object* objectA, Object* objectB, glm::vec3 attachPointA, glm::vec3 attachPointB)
{
    SetObjectA(objectA);
    SetObjectB(objectB);

    this->attachPointA = attachPointA;
    this->attachPointB = attachPointB;

    this->constraintDisplay = CreateConstraintDisplay();
    onPhysicsModeChangedCallbackID = EngineManager::getInstance().AddPhysicsModeChangedEvent([this] {OnPhysicsModeChanged();});
}

Object* CreateAttachPointDisplay(const std::string& ownerName, const std::string& suffix,
    Object* targetObject, ObjectManager& om)
{
    constexpr float hs = 0.01f;

    std::vector<float> vertices = {
        -hs, -hs, 0.0f,  0.0f, 0.0f,
        +hs, -hs, 0.0f,  1.0f, 0.0f,
        +hs, +hs, 0.0f,  1.0f, 1.0f,
        -hs, +hs, 0.0f,  0.0f, 1.0f
    };

    auto display = std::make_unique<Object>(Shader("vertex.txt", "fragment.txt"));
    display->name = ownerName + " " + suffix + " Attach Display";
    display->AddComponent(std::make_unique<RenderComponent>(display.get(), vertices, display->shader, "red.jpg"));
    display->GetComponent<RenderComponent>()->z_index = 999;
    display->AddComponent(std::make_unique<TransformComponent>(
        display.get(), display->shader,
        display->GetComponent<RenderComponent>()->GetCenter()));
    display->AddComponent(std::make_unique<MouseInteractComponent>(display.get(), false));

    display->hidden = true;
    display->GetComponent<RenderComponent>()->Enabled = false;
    display->GetComponent<MouseInteractComponent>()->Enabled = false;
    display->GetComponent<MouseInteractComponent>()->Inspectable = false;

    display->GetComponent<TransformComponent>()->UpdateWorldPosition(
        targetObject->GetComponent<TransformComponent>()->GetWorldPosition());

    Object* raw = display.get();
    om.allObjects.push_back(std::move(display));
    return raw;
}

Object* Constraint::CreateConstraintDisplay()
{
    constexpr float hs = 0.01f;

    std::vector<float> vertices = {
        -hs, -hs, 0.0f,  0.0f, 0.0f,
        +hs, -hs, 0.0f,  1.0f, 0.0f,
        +hs, +hs, 0.0f,  1.0f, 1.0f,
        -hs, +hs, 0.0f,  0.0f, 1.0f
    };

    auto display = std::make_unique<Object>(Shader("vertex.txt", "fragment.txt"));
    display->hidden = true;
    display->name = Name + " Constraint display";
    display->AddComponent(std::make_unique<RenderComponent>(display.get(), vertices, display->shader, "floorTiled.png"));
    RenderComponent* rc = display->GetComponent<RenderComponent>();
    rc->z_index = -999;

    if (canDrawConstraint) {
        rc->SetEnabled(true);
    }
    else {
        rc->SetEnabled(false);
    }

    display->AddComponent(std::make_unique<TransformComponent>(
        display.get(), display->shader,
        rc->GetCenter()));

    Object* raw = display.get();
    ObjectManager::getInstance().allObjects.push_back(std::move(display));
    return raw;
}

void Constraint::EnsureDisplayA()
{
    if (attachDisplayA != nullptr || objectA == nullptr) return;
    attachDisplayA = CreateAttachPointDisplay(Name, "A", objectA,
        ObjectManager::getInstance());
    attachDisplayA->GetComponent<TransformComponent>()->AddTransformCallback(
        [this]() { OnDisplayAMoved(); });
    posSetA = false;
}

void Constraint::EnsureDisplayB()
{
    if (attachDisplayB != nullptr || objectB == nullptr) return;
    attachDisplayB = CreateAttachPointDisplay(Name, "B", objectB,
        ObjectManager::getInstance());
    attachDisplayB->GetComponent<TransformComponent>()->AddTransformCallback(
        [this]() { OnDisplayBMoved(); });
    posSetB = false;
}

void Constraint::DestroyDisplayA()
{
    if (attachDisplayA == nullptr) return;
    attachDisplayA->GetComponent<RenderComponent>()->Enabled = false;
    attachDisplayA->GetComponent<MouseInteractComponent>()->Enabled = false;
    ObjectManager::getInstance().RemoveObject(attachDisplayA);
    attachDisplayA = nullptr;
}

void Constraint::DestroyDisplayB()
{
    if (attachDisplayB == nullptr) return;
    attachDisplayB->GetComponent<RenderComponent>()->Enabled = false;
    attachDisplayB->GetComponent<MouseInteractComponent>()->Enabled = false;
    ObjectManager::getInstance().RemoveObject(attachDisplayB);
    attachDisplayB = nullptr;
}

void Constraint::OnObjectATransformChanged()
{
    if (objectA == nullptr || attachDisplayA == nullptr) return;
    glm::vec3 world = objectA->GetComponent<TransformComponent>()->ProjectToWorld(attachPointA);
    attachDisplayA->GetComponent<TransformComponent>()->UpdateWorldPosition(world);
    ProcessConstraintDisplay();
}

void Constraint::OnObjectBTransformChanged()
{
    if (objectB == nullptr || attachDisplayB == nullptr) return;
    glm::vec3 world = objectB->GetComponent<TransformComponent>()->ProjectToWorld(attachPointB);
    attachDisplayB->GetComponent<TransformComponent>()->UpdateWorldPosition(world);
    ProcessConstraintDisplay();
}

void Constraint::OnDisplayAMoved()
{
    if (objectA == nullptr || attachDisplayA == nullptr) return;
    glm::vec3 world = attachDisplayA->GetComponent<TransformComponent>()->GetWorldPosition();
    attachPointA = objectA->GetComponent<TransformComponent>()->ProjectToWorld(world, true);
}

void Constraint::OnDisplayBMoved()
{
    if (objectB == nullptr || attachDisplayB == nullptr) return;
    glm::vec3 world = attachDisplayB->GetComponent<TransformComponent>()->GetWorldPosition();
    attachPointB = objectB->GetComponent<TransformComponent>()->ProjectToWorld(world, true);
}

void Constraint::OnPhysicsModeChanged() {
    if (EngineManager::getInstance().EnginePhysicsMode == EngineManager::PhysicsMode::Simulate) {
        if (attachDisplayA) {
            attachDisplayA->GetComponent<RenderComponent>()->SetEnabled(false);
        }
        if (attachDisplayB) {
            attachDisplayB->GetComponent<RenderComponent>()->SetEnabled(false);
        }
    }
}

void Constraint::RemoveMirrorFromObjectB()
{
    if (objectB == nullptr) return;
    ConstraintComponent* cc = objectB->GetComponent<ConstraintComponent>();
    if (cc == nullptr) return;

    auto& mv = cc->mirroredConstraints;
    mv.erase(std::remove(mv.begin(), mv.end(), this), mv.end());
}

void Constraint::SetObjectA(Object* obj)
{
    if (objectA != nullptr)
    {
        objectA->RemoveOnDeleteCallback(onDeleteCallbackIdA);
        objectA->GetComponent<TransformComponent>()->RemoveTransformCallback(onTransformCallbackIdA);
        if (attachDisplayA) {
            attachDisplayA->GetComponent<RenderComponent>()->Enabled = false;
            attachDisplayA->GetComponent<MouseInteractComponent>()->Enabled = false;
        }
    }

    objectA = obj;
    if (obj == nullptr) return;

    onDeleteCallbackIdA = obj->AddOnDeleteCallback([this]() { SetObjectA(nullptr); });
    onTransformCallbackIdA = obj->GetComponent<TransformComponent>()->AddTransformCallback(
        [this]() { OnObjectATransformChanged(); });

    attachPointA = obj->GetComponent<RenderComponent>()->GetCenter();
    EnsureDisplayA();
    posSetA = false;
}

void Constraint::SetObjectB(Object* obj)
{
    if (objectB != nullptr)
    {
        RemoveMirrorFromObjectB();

        objectB->RemoveOnDeleteCallback(onDeleteCallbackIdB);
        objectB->GetComponent<TransformComponent>()->RemoveTransformCallback(onTransformCallbackIdB);
        if (attachDisplayB) {
            attachDisplayB->GetComponent<RenderComponent>()->Enabled = false;
            attachDisplayB->GetComponent<MouseInteractComponent>()->Enabled = false;
        }
    }

    objectB = obj;
    if (obj == nullptr) return;

    ConstraintComponent* cc = obj->GetComponent<ConstraintComponent>();
    if (cc == nullptr) {
        auto newCC = std::make_unique<ConstraintComponent>(obj);
        newCC->mirroredConstraints.push_back(this);
        obj->AddComponent(std::move(newCC));
    }
    else {
        cc->mirroredConstraints.push_back(this);
    }

    onDeleteCallbackIdB = obj->AddOnDeleteCallback([this]() { SetObjectB(nullptr); });
    onTransformCallbackIdB = obj->GetComponent<TransformComponent>()->AddTransformCallback(
        [this]() { OnObjectBTransformChanged(); });

    attachPointB = obj->GetComponent<RenderComponent>()->GetCenter();
    EnsureDisplayB();
    posSetB = false;

    ProcessConstraintDisplay();
}

void Constraint::Unregister()
{
    RemoveMirrorFromObjectB();

    DestroyDisplayA();
    DestroyDisplayB();
    SetObjectA(nullptr);
    SetObjectB(nullptr);

    ObjectManager::getInstance().RemoveObject(constraintDisplay);
    EngineManager::getInstance().RemovePhysicsModeChangedEvent(onPhysicsModeChangedCallbackID);
    constraintDisplay = nullptr;
}

void Constraint::ProcessConstraintDisplay() {
    constraintDisplay->GetComponent<RenderComponent>()->SetEnabled(false);
}

void Constraint::ProcessMirroredUI()
{
    ImGui::Text("Linked to %s", objectA ? objectA->name.c_str() : "(none)");
    if (objectA && ImGui::Button("Go to owner"))
        EditorManager::getInstance().SetSelectedObject(objectA);
}

void Constraint::ProcessInspectorUI(Object* parent)
{
    auto& om = ObjectManager::getInstance();

    if (objectA == nullptr && parent != nullptr)
        SetObjectA(parent);

    EnsureDisplayA();
    EnsureDisplayB();

    auto AttachPointWidget = [&](
        const char* popupId,
        Object* currentObj,
        bool& useCenter,
        Object*& display,
        bool& posSet)
        {
            if (display == nullptr) return;

            if (ImGui::Checkbox((std::string("Use Object Center##") + popupId).c_str(), &useCenter))
            {
                if (useCenter)
                {
                    display->GetComponent<TransformComponent>()->UpdateWorldPosition(
                        currentObj->GetComponent<TransformComponent>()->GetWorldPosition());
                    display->GetComponent<RenderComponent>()->Enabled = false;
                    display->GetComponent<MouseInteractComponent>()->Enabled = false;
                }
            }

            if (!useCenter)
            {
                bool displayVisible = display->GetComponent<RenderComponent>()->Enabled;

                if (!displayVisible)
                {
                    if (ImGui::Button((std::string("Change Attach Point##") + popupId).c_str()))
                    {
                        if (!posSet)
                        {
                            display->GetComponent<TransformComponent>()->UpdateWorldPosition(
                                currentObj->GetComponent<TransformComponent>()->GetWorldPosition());
                            posSet = true;
                        }
                        display->GetComponent<MouseInteractComponent>()->Enabled = true;
                        display->GetComponent<RenderComponent>()->Enabled = true;
                    }
                }
                else
                {
                    if (ImGui::Button((std::string("Confirm##") + popupId).c_str()))
                    {
                        display->GetComponent<MouseInteractComponent>()->Enabled = false;
                        display->GetComponent<RenderComponent>()->Enabled = false;
                    }
                }
            }
        };

    ImGui::Text("Object A (Owner)");
    {
        char nameBuf[128] = "None";
        if (objectA)
        {
#if defined(_MSC_VER)
            strcpy_s(nameBuf, objectA->name.c_str());
#else
            strncpy(nameBuf, objectA->name.c_str(), sizeof(nameBuf) - 1);
#endif
        }
        ImGui::BeginDisabled();
        ImGui::InputText("##objA_locked", nameBuf, IM_ARRAYSIZE(nameBuf), ImGuiInputTextFlags_ReadOnly);
        ImGui::EndDisabled();
    }
    AttachPointWidget("A", objectA, useCenterA, attachDisplayA, posSetA);

    ImGui::Spacing();

    ImGui::Text("Object B");
    {
        char nameBuf[128] = "None (Click to choose...)";
        if (objectB)
        {
#if defined(_MSC_VER)
            strcpy_s(nameBuf, objectB->name.c_str());
#else
            strncpy(nameBuf, objectB->name.c_str(), sizeof(nameBuf) - 1);
#endif
        }

        ImGui::InputText("##sel_objB", nameBuf, IM_ARRAYSIZE(nameBuf), ImGuiInputTextFlags_ReadOnly);

        if (ImGui::IsItemHovered())
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        if (ImGui::IsItemClicked())
            ImGui::OpenPopup("ConstraintPickerB");

        if (ImGui::BeginPopup("ConstraintPickerB"))
        {
            ImGui::TextDisabled("Select Object B");
            ImGui::Separator();

            Object* pendingSelection = nullptr;
            for (auto& objPtr : om.allObjects)
            {
                Object* candidate = objPtr.get();
                if (candidate->hidden || candidate == objectA) continue;

                if (ImGui::Selectable(candidate->name.c_str()))
                {
                    pendingSelection = candidate;
                    ImGui::CloseCurrentPopup();
                }
            }

            ImGui::EndPopup();

            if (pendingSelection != nullptr)
                SetObjectB(pendingSelection);
        }
    }
    AttachPointWidget("B", objectB, useCenterB, attachDisplayB, posSetB);

    ImGui::Spacing();

    ImGui::Text("Beta ");
    ImGui::SameLine();
    ImGui::DragFloat("##beta", &beta, 0.001f, 0.0f, 1.0f);

    ImGui::Text("Draw constraint ");
    ImGui::SameLine();
    if (ImGui::Checkbox("##Draw constraint", &canDrawConstraint)) {
        ProcessConstraintDisplay();
    }
}