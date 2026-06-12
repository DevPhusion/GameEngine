#pragma once
#include "Object.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp> 

struct BoundingCircle {
public:
	BoundingCircle() = default;
	BoundingCircle(const glm::vec3 center, float radius);
	//Bounding sphere to bound two smaller sphere
	BoundingCircle(const BoundingCircle& one, const BoundingCircle& two);

	glm::vec3 center;
	float radius;

	bool overlaps(const BoundingCircle* other) const;
	float getSize() const {
		return radius;
	}

	float getGrowth(const BoundingCircle& other) const;
};

