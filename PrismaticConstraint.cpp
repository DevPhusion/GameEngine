#include "PrismaticConstraint.h"

PrismaticConstraint::PrismaticConstraint(Object* objectA, Object* objectB, glm::vec3 attachPointA, glm::vec3 attachPointB, glm::vec3 dir) :
    Constraint(objectA, objectB, attachPointA, attachPointB) {
    this->dir = dir;
    this->Name = "Weld Constraint";
}

void PrismaticConstraint::Prepare(std::vector<SolverRow>& rows, float delta) {
    if (objectA == nullptr || objectB == nullptr) {
        return;
    }

    TransformComponent* tcA = objectA->GetComponent<TransformComponent>();
    TransformComponent* tcB = objectB->GetComponent<TransformComponent>();
    RigidBodyComponent* pcA = objectA->GetComponent<RigidBodyComponent>();
    RigidBodyComponent* pcB = objectB->GetComponent<RigidBodyComponent>();

    glm::vec3 globalPointA = tcA->ProjectToWorld(attachPointA);
    glm::vec3 globalPointB = tcB->ProjectToWorld(attachPointB);

    glm::vec3 rA = globalPointA - tcA->GetWorldPosition();
    glm::vec3 rB = globalPointB - tcB->GetWorldPosition();

    JacobianRow jacobianLinear, jacobianTheta;
    SolverRow rowLinear, rowTheta;

    glm::vec3 t = glm::vec3(-dir.y, dir.x, 0.0f);
    
    jacobianLinear.linearA = t;
    jacobianLinear.linearB = -t;
    jacobianLinear.angularA = rA.x * t.y - rA.y * t.x;
    jacobianLinear.angularB = -(rB.x * t.y - rB.y * t.x);

    jacobianTheta.linearA = glm::vec3(0);
    jacobianTheta.linearB = glm::vec3(0);
    jacobianTheta.angularA = 1;
    jacobianTheta.angularB = -1;

    float klinear = 0.0f;
    float ktheta = 0.0f;
    if (pcA) {
        klinear += pcA->inverseMass + glm::length2(jacobianLinear.linearA) * pcA->inverseInertia;
        ktheta += pcA->inverseInertia;
    }
    if (pcB) {
        klinear += pcB->inverseMass + glm::length2(jacobianLinear.linearB) * pcB->inverseInertia;
        ktheta += pcB->inverseInertia;
    }

    float biasLinear = (beta * glm::dot(dir, t)) / delta;
    float biasTheta = tcB->rotation - tcA->rotation;

    rowLinear.jacobian = jacobianLinear;
    rowLinear.effectiveMass = (klinear > 0.0f) ? 1.0f / klinear : 0.0f;
    rowLinear.bias = biasLinear;
    rowLinear.maxLambda = INFINITY;
    rowLinear.minLambda = -INFINITY;
    rowLinear.objectA = objectA;
    rowLinear.objectB = objectB;
    rowLinear.parentConstraint = this;
    rowLinear.lambda = cacheLambda[0];
    rowLinear.softnessCFM = 0.0f;

    rowTheta.jacobian = jacobianTheta;
    rowTheta.effectiveMass = (ktheta > 0.0f) ? 1.0f / ktheta : 0.0f;
    rowTheta.bias = biasTheta;
    rowTheta.maxLambda = INFINITY;
    rowTheta.minLambda = -INFINITY;
    rowTheta.objectA = objectA;
    rowTheta.objectB = objectB;
    rowTheta.parentConstraint = this;
    rowTheta.lambda = cacheLambda[1];
    rowTheta.softnessCFM = 0.0f;

    linearRowOffset = static_cast<int>(rows.size());
    rows.push_back(rowLinear);
    thetaRowOffset = static_cast<int>(rows.size());
    rows.push_back(rowTheta);
}

void PrismaticConstraint::PostSolve(std::vector<SolverRow>& allRows) {
    cacheLambda[0] = allRows[linearRowOffset].lambda;
    cacheLambda[1] = allRows[thetaRowOffset].lambda;
}

void PrismaticConstraint::SetObjectA(Object* obj) {
    Constraint::SetObjectA(obj);

    if (objectA != nullptr && objectB != nullptr) {
        glm::vec3 pA = objectA->GetComponent<TransformComponent>()->GetWorldPosition();
        glm::vec3 pB = objectB->GetComponent<TransformComponent>()->GetWorldPosition();
        this->dir = pB - pA;
    }
}

void PrismaticConstraint::SetObjectB(Object* obj) {
    Constraint::SetObjectB(obj);

    if (objectA != nullptr && objectB != nullptr) {
        glm::vec3 pA = objectA->GetComponent<TransformComponent>()->GetWorldPosition();
        glm::vec3 pB = objectB->GetComponent<TransformComponent>()->GetWorldPosition();
        this->dir = pB - pA;
    }
}


void PrismaticConstraint::ProcessInspectorUI(Object* parent) {
    Constraint::ProcessInspectorUI(parent);

    if (objectA && objectB) {
        ImGui::Text("Locked direction ");
        ImGui::BeginDisabled();
        float d[2] = { dir.x, dir.y };
        ImGui::InputFloat2("##Locked direction", d);
        ImGui::EndDisabled();
        if (ImGui::Button("Re-lock direction")) {
            glm::vec3 pA = objectA->GetComponent<TransformComponent>()->GetWorldPosition();
            glm::vec3 pB = objectB->GetComponent<TransformComponent>()->GetWorldPosition();
            this->dir = pB - pA;
        }
    }
}

void PrismaticConstraint::ProcessConstraintDisplay() {
    RenderComponent* rc = constraintDisplay->GetComponent<RenderComponent>();
    TransformComponent* tc = constraintDisplay->GetComponent<TransformComponent>();

    if (objectA == nullptr || objectB == nullptr || !canDrawConstraint) {
        rc->SetEnabled(false);
        return;
    }

    glm::vec3 top = objectA->GetComponent<TransformComponent>()->GetTransformedPoint(attachPointA);
    glm::vec3 bot = objectB->GetComponent<TransformComponent>()->GetTransformedPoint(attachPointB);

    glm::vec2 topVert = tc->GetTransformedPoint(top, true);
    glm::vec2 botVert = tc->GetTransformedPoint(bot, true);

    glm::vec2 segment = botVert - topVert;
    float totalLength = glm::length(segment);
    if (totalLength < 0.0001f) return;

    glm::vec2 segDir = segment / totalLength;
    float nx = -segDir.y;
    float ny = segDir.x;

    const float thickness = 0.01f;
    const float dashLength = 0.05f;
    const float gapLength = 0.03f;
    const float halfT = thickness * 0.5f;
    const float offsetX = nx * halfT;
    const float offsetY = ny * halfT;

    std::vector<float>        vertices;
    std::vector<unsigned int> indices;

    float travelled = 0.0f;

    while (travelled + dashLength <= totalLength) {
        glm::vec2 dashStart = topVert + segDir * travelled;
        glm::vec2 dashEnd = topVert + segDir * (travelled + dashLength);

        unsigned int base = static_cast<unsigned int>(vertices.size() / 5);

        vertices.insert(vertices.end(), { dashStart.x + offsetX, dashStart.y + offsetY, 0.0f, 0.0f, 0.0f });
        vertices.insert(vertices.end(), { dashEnd.x + offsetX, dashEnd.y + offsetY, 0.0f, 1.0f, 0.0f });
        vertices.insert(vertices.end(), { dashStart.x - offsetX, dashStart.y - offsetY, 0.0f, 0.0f, 1.0f });
        vertices.insert(vertices.end(), { dashEnd.x - offsetX, dashEnd.y - offsetY, 0.0f, 1.0f, 1.0f });

        indices.push_back(base + 0);
        indices.push_back(base + 2);
        indices.push_back(base + 1);

        indices.push_back(base + 1);
        indices.push_back(base + 2);
        indices.push_back(base + 3);

        travelled += dashLength + gapLength;
    }

    if (travelled < totalLength) {
        glm::vec2 dashStart = topVert + segDir * travelled;
        glm::vec2 dashEnd = botVert;

        unsigned int base = static_cast<unsigned int>(vertices.size() / 5);

        vertices.insert(vertices.end(), { dashStart.x + offsetX, dashStart.y + offsetY, 0.0f, 0.0f, 0.0f });
        vertices.insert(vertices.end(), { dashEnd.x + offsetX, dashEnd.y + offsetY, 0.0f, 1.0f, 0.0f });
        vertices.insert(vertices.end(), { dashStart.x - offsetX, dashStart.y - offsetY, 0.0f, 0.0f, 1.0f });
        vertices.insert(vertices.end(), { dashEnd.x - offsetX, dashEnd.y - offsetY, 0.0f, 1.0f, 1.0f });

        indices.push_back(base + 0);
        indices.push_back(base + 2);
        indices.push_back(base + 1);

        indices.push_back(base + 1);
        indices.push_back(base + 2);
        indices.push_back(base + 3);
    }

    rc->UpdateShape(vertices, indices);
    rc->SetEnabled(true);
}