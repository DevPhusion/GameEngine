#include "Object.h"

Object::Object(Shader shader) {
	this->shader = shader;
}

void Object::AddComponent(std::unique_ptr<Component> component) {
	components.push_back(std::move(component));
}