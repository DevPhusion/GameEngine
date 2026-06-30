#include "Object.h"

Object::Object(Shader shader) {
	this->shader = shader;
}

int Object::AddOnDeleteCallback(std::function<void()> func) {
	CurrentOnRemoveID += 1;
	OnDeleteCallbacks[CurrentOnRemoveID] = func;
	return CurrentOnRemoveID;
}

void Object::RemoveOnDeleteCallback(int ID) {
	OnDeleteCallbacks.erase(ID);
}