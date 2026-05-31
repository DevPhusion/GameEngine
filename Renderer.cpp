#include "Renderer.h"

Renderer::Renderer(std::vector<std::unique_ptr<Object>>* objects) {
	this->allObjects = objects;
}

void Renderer::Draw() {
	for (int i = 0; i < this->allObjects->size(); i++)
	{
		if ((*allObjects)[i]->HasComponent<RenderComponent>()) {
			(*allObjects)[i]->GetComponent<RenderComponent>()->Draw();
		}
		if ((*allObjects)[i]->HasComponent<TransformComponent>()) {
			(*allObjects)[i]->GetComponent<TransformComponent>()->ProcessTransform();
		}
	}
}
