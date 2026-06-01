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

	std::string name;
	bool hidden = false;
	std::vector<std::unique_ptr<Component>> components = {};
	Shader shader;

	template <AllowedTypes T>
	T* GetComponent() {
		for (int i = 0; i < components.size(); i++)
		{
			if (typeid(*components[i]) == typeid(T)) {
				return dynamic_cast<T*>(components[i].get());
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

	template <AllowedTypes T>
	void RemoveComponent() {
		for (int i = 0; i < components.size(); i++)
		{
			if (typeid(*components[i]) == typeid(T)) {
				components.erase(components.begin() + i);
				return;
			}
		}
	}
	
	void AddComponent(std::unique_ptr<Component> component);
};

