#include "BoundingCircle.h"

BoundingCircle::BoundingCircle(const glm::vec3 center, float radius) {
	this->center = center;
	this->radius = radius;
}

BoundingCircle::BoundingCircle(const BoundingCircle& one, const BoundingCircle& two) {
	center = (one.center + two.center) * 0.5f;

	float dist = glm::distance(one.center, two.center);
	radius = (dist + one.radius + two.radius) * 0.5f;
}

float BoundingCircle::getGrowth(const BoundingCircle& other) const {
	float dist = glm::distance(center, other.center);
	float newRadius = (dist + radius + other.radius) * 0.5f;
	return newRadius - radius;
}

bool BoundingCircle::overlaps(const BoundingCircle* other) const {
	float distanceSquared = distance2(center, other->center);
	return distanceSquared < (radius + other->radius) * (radius + other->radius);
}