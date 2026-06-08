#include "Renderer.h"

Renderer::Renderer(std::vector<std::unique_ptr<Object>>* objects) {
	this->allObjects = objects;
}

void Renderer::Draw() {
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
    }
}
