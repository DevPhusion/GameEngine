#include "Renderer.h"

Renderer::Renderer(std::vector<std::unique_ptr<Object>>* objects) {
	this->allObjects = objects;
}

void Renderer::Draw() {
    glm::vec2 camPos = glm::vec2(Camera::getInstance().cameraPos.x, Camera::getInstance().cameraPos.y);
    
    glm::vec2 screenSize = glm::vec2(EngineManager::getInstance().windowWidth, EngineManager::getInstance().windowHeight); // Match your window sizes
    float zoom = Camera::getInstance().cameraZoom;

    backgroundGrid.Draw(camPos, screenSize, zoom);

    std::vector<Object*> renderQueue;
    for (size_t i = 0; i < this->allObjects->size(); i++) {
        if ((*allObjects)[i]->HasComponent<RenderComponent>()) {
            renderQueue.push_back((*allObjects)[i].get());
        }
    }

    std::sort(renderQueue.begin(), renderQueue.end(), [](Object* a, Object* b) {
        float zA = 0.0f;
        float zB = 0.0f;

        if (a->HasComponent<RenderComponent>()) {
            zA = a->GetComponent<RenderComponent>()->z_index;
        }
        if (b->HasComponent<RenderComponent>()) {
            zB = b->GetComponent<RenderComponent>()->z_index;
        }

        return zA < zB; 
    });

    for (Object* obj : renderQueue) {
        obj->GetComponent<RenderComponent>()->Draw();
        if (obj->HasComponent<TransformComponent>()) {
           obj->GetComponent<TransformComponent>()->ProcessTransform();
        }
        SoftBodyComponent* sb = obj->GetComponent<SoftBodyComponent>();
        if (sb) {
            for (int i = 0; i < sb->MassAggregate.size(); i++)
            {
                sb->MassAggregate[i]->ProcessTransform();
            }
        }
    }

    if (EngineManager::getInstance().debugMode) {
        BAHNode<BoundingCircle>* bvhRoot = &PhysicsEngine::getInstance().root;

        glLineWidth(2.0f);
        bvhRoot->DrawBoundingArea();

        for (int i = 0; i < PhysicsEngine::getInstance().allContactPoints.size(); i++)
        {
            DebugPoint point = DebugPoint();
            point.DrawPoint(PhysicsEngine::getInstance().allContactPoints[i].point, 15, Shader("vertex.txt", "fragment.txt"));
        }

        for (int i = 0; i < (*allObjects).size(); i++)
        {
            SoftBodyComponent* sb = (*allObjects)[i]->GetComponent<SoftBodyComponent>();
			if (sb) {
				for (int j = 0; j < sb->MassAggregate.size(); j++)
				{
					sb->MassAggregate[j]->DrawDebug();
				}
			}
        }

        glLineWidth(1.0f);
    }

}
