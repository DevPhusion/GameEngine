#include "Object.h"

Object::Object(Shader shader) {
	this->shader = shader;
}

void Object::AddComponent(std::unique_ptr<Component> component) {
	components.push_back(std::move(component));
}

int Object::AddOnDeleteCallback(std::function<void()> func) {
	CurrentOnRemoveID += 1;
	OnDeleteCallbacks[CurrentOnRemoveID] = func;
	return CurrentOnRemoveID;
}

void Object::RemoveOnDeleteCallback(int ID) {
	OnDeleteCallbacks.erase(ID);
}