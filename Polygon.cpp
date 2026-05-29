#include "Polygon.h"

Polygon::Polygon(std::vector<float> vertices, Shader shader, std::vector<std::string> textures) : Object(shader) {
	AddComponent(new RenderComponent(vertices, shader, textures));
	AddComponent(new TransformComponent(shader, GetCenter()));
	AddComponent(new VertexComponent(GetPointer()));
	AddComponent(new PhysicsComponent(GetPointer()));
	AddComponent(new MouseInteractComponent(*this, true));
}

glm::vec3 Polygon::GetCenter() {
	float A = 0;
	float C_x = 0;
	float C_y = 0;
	std::vector<std::vector<float>> points = GetComponent<RenderComponent>()->points;
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
