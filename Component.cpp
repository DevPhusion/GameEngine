#include "Component.h"
#include "Object.h"

Component::Component(Object* parent) {
	this->parent = parent;
}

void Component::SetEnabled(bool enabled) {
	Enabled = enabled;
}