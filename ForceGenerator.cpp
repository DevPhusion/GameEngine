#include "ForceGenerator.h"

void ForceGenerator::setDisplayFunc(std::shared_ptr<std::function<void(int index)>> func) {
	displayFunc = func;
}