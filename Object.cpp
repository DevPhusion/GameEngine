#include "Object.h"

Object::Object(Shader shader) {
	this->shader = shader;
}

void Object::AddComponent(Component* component) {
	components.push_back(component);
}

std::shared_ptr<Object> Object::GetPointer() {
	if (ptr == nullptr) {
		ptr = std::make_shared<Object>(*this);
	}
	return ptr;
}
