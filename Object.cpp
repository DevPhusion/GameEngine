#include "Object.h"

Object::Object(Shader shader) {
	this->shader = shader;
}

void Object::AddComponent(Component* component) {
	components.push_back(component);
}
