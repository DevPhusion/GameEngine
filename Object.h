#pragma once
#include "Component.h"
#include "Shader.h"
#include <vector>
#include <type_traits>


template <typename T>
concept AllowedTypes = std::is_base_of_v<Component, T>;

class Object
{
public:
	Object(Shader shader);
	Object() = default;

	std::vector<Component*> components;
	Shader shader;

	template <AllowedTypes T>
	T* GetComponent() {
		for (int i = 0; i < components.size(); i++)
		{
			if (typeid(*components[i]) == typeid(T)) {
				return dynamic_cast<T*>(components[i]);
			}
		}
		return nullptr;
	}

	template <AllowedTypes T>
	bool HasComponent() {
		for (int i = 0; i < components.size(); i++)
		{
			if (typeid(*components[i]) == typeid(T)) {
				return true;
			}
		}

		return false;
	}
	
	void AddComponent(Component* component);
};

