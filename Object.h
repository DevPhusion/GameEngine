#pragma once
#include "Component.h"
#include "ComponentTypeID.h"
#include "Shader.h"
#include <vector>
#include <type_traits>
#include <typeinfo>

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
	std::vector<Component*> componentByType;
	Shader shader;

	template <AllowedTypes T>
	T* GetComponent() {
		size_t id = ComponentTypeID::Get<T>();
		if (id >= componentByType.size()) return nullptr;
		return static_cast<T*>(componentByType[id]);
	}

	template <AllowedTypes T>
	bool HasComponent() {
		return GetComponent<T>() != nullptr;
	}

	void RemoveComponent(int index) {
		size_t id = components[index]->GetTypeID();
		components[index]->OnDelete();
		if (id < componentByType.size() && componentByType[id] == components[index].get()) {
			componentByType[id] = nullptr;
		}
		components.erase(components.begin() + index);
	}

	template <AllowedTypes T>
	void RemoveComponent() {
		size_t id = ComponentTypeID::Get<T>();
		if (id >= componentByType.size()) return;

		Component* target = componentByType[id];
		if (!target) return;

		for (int i = 0; i < components.size(); i++)
		{
			if (components[i].get() == target) {
				components[i]->OnDelete();
				componentByType[id] = nullptr;
				components.erase(components.begin() + i);
				return;
			}
		}
	}
	
	virtual void Process(float delta) {

	}

	virtual void OnDelete() {
		auto safeCallbacks = OnDeleteCallbacks;
		for (const auto& [id, func] : safeCallbacks) {	
			if (func) {
				func();
			}
		}

		for (int i = 0; i < components.size(); i++)
		{
			components[i]->OnDelete();
		}
	}

	void AddComponent(std::unique_ptr<Component> component) {
		size_t id = component->GetTypeID(); 
		if (id >= componentByType.size()) componentByType.resize(id + 1, nullptr);
		componentByType[id] = component.get();
		components.push_back(std::move(component));
	}

	std::unordered_map<int, std::function<void()>> OnDeleteCallbacks;

	int AddOnDeleteCallback(std::function<void()> func);
	void RemoveOnDeleteCallback(int ID);
private:
	int CurrentOnRemoveID = -1;
};

