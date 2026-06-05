#include "ObjectManager.h"

void ObjectManager::ProcessObjects(float delta) {
	for (int i = 0; i < allObjects.size(); i++)
	{
		allObjects[i]->Process(delta);
	}
}

void ObjectManager::AddObject() {
	std::unique_ptr<Object> obj = std::make_unique<Object>(Shader("vertex.txt", "fragment.txt"));

	float sizeY = 0.01f;
	float sizeX = 0.01f;

	std::vector<float> vertices = {
		- sizeX, - sizeY, 0.0f, 0.0f, 0.0f,
		+ sizeX, - sizeY, 0.0f, 1.0f, 0.0f,
		+ sizeX, + sizeY, 0.0f, 1.0f, 1.0f,
		- sizeX, + sizeY, 0.0f, 0.0f, 1.0f
	};

	obj->AddComponent(std::make_unique<RenderComponent>(obj.get(), vertices, obj.get()->shader, "floorTiled.png"));
	obj->AddComponent(std::make_unique<TransformComponent>(obj.get(), obj.get()->shader, obj.get()->GetComponent<RenderComponent>()->GetCenter()));
	obj->AddComponent(std::make_unique<MouseInteractComponent>(obj.get(), false));

	allObjects.push_back(std::move(obj));
}

void ObjectManager::AddPolygon() {
	if (vertexPoints.size() < 3) {
		std::cout << "Invalid polygon" << std::endl;
		return;
	}

	std::unique_ptr<Polygon> poly = std::make_unique<Polygon>(vertices, Shader("vertex.txt", "fragment.txt"), "floorTiled.png");

	auto* vc = poly->GetComponent<VertexComponent>();
	auto* tc = poly->GetComponent<TransformComponent>();
	auto* pc = poly->GetComponent<PhysicsComponent>();

	pc->inverseMass = 1;
	PhysicsEngine::getInstance().RegisterForce(poly.get(), new Gravity(-9.8f));
	PhysicsEngine::getInstance().RegisterForce(poly.get(), new Drag(0.0f, 0.002f));

	vc->SetVertexPoints(vertexPoints);
	tc->SetOriginTransform(Camera::getInstance().viewMatrixInverse);
	allObjects.push_back(std::move(poly));
	vertices.clear();
	vertexPoints.clear();
}

void ObjectManager::AddPolygonVertex() {
	if (EngineManager::getInstance().EngineInteractMode == EngineManager::InteractMode::AddVertex) {
		vertices.push_back(InputManager::glX);
		vertices.push_back(InputManager::glY);
		vertices.push_back(0.0f); // Z coordinate
		vertices.push_back(InputManager::glX); // U
		vertices.push_back(InputManager::glY); // V

		std::unique_ptr<VertexPoint> pointIndicator = std::make_unique<VertexPoint>(InputManager::glX, InputManager::glY, Shader("vertex.txt", "fragment.txt"));
		pointIndicator->hidden = true;
		vertexPoints.push_back(pointIndicator.get());
		allObjects.push_back(std::move(pointIndicator));
	}
}

void ObjectManager::AddSpring() {
	std::unique_ptr<Spring> spring = std::make_unique<Spring>(Shader("vertex.txt", "fragment.txt"), 150.0f, 15.0f, 5.0f);
	allObjects.push_back(std::move(spring));
}

void ObjectManager::RemoveObject(Object* obj) {
	for (int i = 0; i < allObjects.size(); i++)
	{
		if (allObjects[i].get() == obj) {
			if (obj->HasComponent<VertexComponent>()) {
				std::vector<VertexPoint*> points = obj->GetComponent<VertexComponent>()->vertexPoints;
				obj->OnDelete();
				allObjects.erase(allObjects.begin() + i);
				for (int j = 0; j < points.size(); j++)
				{
					RemoveObject(points[j]);
				}
			}
			else {
				obj->OnDelete();
				allObjects.erase(allObjects.begin() + i);
			}
		}
	}
}