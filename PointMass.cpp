#include "PointMass.h"
#include "SoftBodyComponent.h"

PointMass::PointMass(Shader shader, SoftBodyComponent* sb, glm::vec3 point, int index, bool isCenter) {
	this->sb = sb;
	this->index = index;
	this->isCenter = isCenter;
}

void PointMass::Process(float delta) {

}