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
		//std::cout << "Point: (" << points[points.size() - 1][0] << ", " << points[points.size() - 1][1] << ") with index " << points[points.size() - 1][2] << std::endl;
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

void Polygon::UpdateBuffer() {
	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(float), Vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(unsigned int), Indices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Polygon::SetVertices(std::vector<float> vertices) {
	Vertices = vertices;
	Indices = Triangulate(vertices);
	UpdateBuffer();
}

void Polygon::InsertVertex(unsigned int index, std::vector<float> vertex) {
	Vertices.insert(Vertices.begin() + index * 8, vertex.begin(), vertex.end());
	Indices = Triangulate(Vertices);
	UpdateBuffer();
}

void Polygon::RemoveVertex(unsigned int index) {
	Vertices.erase(Vertices.begin() + index * 8, Vertices.begin() + index * 8 + 8);
	Indices = Triangulate(Vertices);
	UpdateBuffer();
}

void Polygon::AddVertex(std::vector<float> vertex) {
	for (int i = 0; i < vertex.size(); i++)
	{
		Vertices.push_back(vertex[i]);
	}
	Indices = Triangulate(Vertices);
	UpdateBuffer();
}

std::vector<float> Polygon::GetVertices() {
	return Vertices;
}

Polygon::Polygon(std::vector<float> vertices, Shader shader, std::vector<std::string> textures) {
	Vertices = vertices;
	Indices = Triangulate(vertices);
	this->shader = shader;

	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(float), Vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);


	glGenBuffers(1, &this->EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(unsigned int), Indices.data(), GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Setup texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Load textures
	for(int i = 0; i < textures.size(); i++)
	{
		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(textures[i].c_str(), &width, &height, &nrChannels, 0);
		unsigned int texture;
		glGenTextures(1, &texture);
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, texture);
	
		if (data) {
			if (nrChannels == 3) {
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			}
			else if (nrChannels == 4) {
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			}
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else {
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);
	}

	

}

void Polygon::Draw() {
	this->shader.use();
	shader.setSampler2D("texture1", 0);
	shader.setSampler2D("texture2", 1);
	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}