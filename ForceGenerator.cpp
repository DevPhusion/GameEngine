#include "ForceGenerator.h"

void ForceGenerator::setDisplayFunc(std::shared_ptr<std::function<void()>> func) {
	displayFunc = func;
}