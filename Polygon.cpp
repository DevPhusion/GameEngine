#include "Polygon.h"

float calcTriangleArea(std::vector<float> a, std::vector<float> b, std::vector<float> c) {
	return 0.5f * std::abs((a[0] * (b[1] - c[1]) + b[0] * (c[1] - a[1]) + c[0] * (a[1] - b[1])));
}

std::vector<unsigned int> Polygon::Triangulate(std::vector<float> vertices) {
	std::vector<std::vector<float>> points;
	for (int i = 0; i < vertices.size(); i+=5)
	{
		points.push_back(std::vector<float> {
			vertices[i],
			vertices[i + 1],
			float(int(i/8))
		});
		i += 3;
	}
	
	std::vector<unsigned int> indices;

	unsigned int earIndex = 0;

	std::vector<float> ear = points[0];
	std::vector<float> prev = points[points.size() - 1];
	std::vector<float> next = points[1];


	while (points.size() > 3)
	{
		bool ValidEar = true;
		glm::vec2 v1 = glm::vec2(ear[0] - prev[0], ear[1] - prev[1]);
		glm::vec2 v2 = glm::vec2(ear[0] - next[0], ear[1] - next[1]);

		float angle = glm::angle(glm::normalize(v1), glm::normalize(v2));

		for (int i = 0; i < points.size(); i++)
		{
			if (points[i][2] != ear[2] && points[i][2] != prev[2] && points[i][2] != next[2]) {
				float areafull = calcTriangleArea(prev, ear, next);
				float area1 = calcTriangleArea(points[i], prev, ear);
				float area2 = calcTriangleArea(points[i], ear, next);
				float area3 = calcTriangleArea(points[i], prev, next);

				if (abs(area1 + area2 + area3 - areafull) < 0.0001) {
					ValidEar = false;
					break;
				}
			}
		}
		if (angle > glm::pi<float>()) {
			ValidEar = false;
			std::cout << "Angle is concave, not an ear." << std::endl;
		}

		if (ValidEar) {
			indices.push_back((unsigned int)prev[2]);
			indices.push_back((unsigned int)ear[2]);
			indices.push_back((unsigned int)next[2]);

			points.erase(points.begin() + earIndex);
			earIndex = 0;
			ear = points[0];
			prev = points[points.size() - 1];
			next = points[1];
		
		}
		else {
			earIndex++;
			if (earIndex >= points.size()) {
				std::cout << "No valid ear found, polygon might be malformed." << std::endl;
				break;
			}
			ear = points[earIndex];
			prev = points[(earIndex - 1 < 0) ? points.size() - 1 : earIndex - 1];
			next = points[(earIndex + 1 >= points.size()) ? 0 : earIndex + 1];
		}
	}

	indices.push_back((unsigned int)points[2][2]);
	indices.push_back((unsigned int)points[0][2]);
	indices.push_back((unsigned int)points[1][2]);

	return indices;
}

glm::vec3 Polygon::GetCenter() {
	std::vector<float> vertices = GetComponent<RenderComponent>()->Vertices;

	std::vector<std::vector<float>> points;
	for (int i = 0; i < vertices.size(); i += 5)
	{
		points.push_back(std::vector<float> {
			vertices[i],
				vertices[i + 1],
				float(int(i / 8))
		});
		i += 3;
	}

	float A = 0;
	float C_x = 0;
	float C_y = 0;
	int n = points.size();
	for (int i = 0; i < n; i++)
	{
		int j = (i + 1) % n;
		float shoelace = points[i][0] * points[j][1] - points[j][0] * points[i][1];
		A += shoelace;
		C_x += (points[i][0] + points[j][0]) * shoelace;
		C_y += (points[i][1] + points[j][1]) * shoelace;
	}

	A = A / 2.0;
	if (A == 0) {
		std::cout << "Error calculating polygon center";
		return glm::vec3(0, 0, 0);
	}

	C_x = C_x / (6.0 * A);
	C_y = C_y / (6.0 * A);

	return glm::vec3(C_x, C_y, 0);
}

void Polygon::SetVertices(std::vector<float> vertices) {
	RenderComponent* render = GetComponent<RenderComponent>();
	render->UpdateShape(vertices, Triangulate(vertices));
	GetComponent<TransformComponent>()->SetRotationCenter(GetCenter());
}

Polygon::Polygon(std::vector<float> vertices, Shader shader, std::vector<std::string> textures) : Object(shader) {
	AddComponent(new RenderComponent(vertices, Triangulate(vertices), shader, textures));
	AddComponent(new TransformComponent(shader, GetCenter()));
	std::shared_ptr<Polygon> ptr = std::make_shared<Polygon>(*this);
	AddComponent(new VertexComponent(ptr));
	AddComponent(new PhysicsComponent(ptr));

	glm::vec3 center = GetComponent<TransformComponent>()->GetWorldPosition();
	std::cout << "Center: (" << center.x << "," << center.y << ")" << std::endl;
}
