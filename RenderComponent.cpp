#include "RenderComponent.h"

RenderComponent::RenderComponent(Object* parent, std::vector<float> vertices, Shader shader, std::string texture_path) : Component(parent) {
	Name = "Render Component";

	Vertices = vertices;
	Indices = Triangulate(vertices);
	this->shader = shader;

	points.clear();
	edges.clear();
	for (int i = 0; i < vertices.size(); i += 2)
	{
		points.push_back(std::vector<float> {
			vertices[i],
				vertices[i + 1],
				float(int(i / 5))
		});
		i += 3;
	}

	for (int i = 0; i < points.size(); i++)
	{
		std::vector<float> p1 = points[i];
		std::vector<float> p2 = points[(i + 1) % points.size()];
		edges.push_back(std::vector<std::vector<float>> {p1, p2});
	}

	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(float), Vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


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
	SetTexture(texture_path);
}

void RenderComponent::SetTexture(std::string texture_path) {
	this->texture_path = texture_path;
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);

	if (this->TextureID != 0) {
		glDeleteTextures(1, &this->TextureID);
	}

	unsigned char* data = stbi_load(texture_path.c_str(), &width, &height, &nrChannels, 0);
	glGenTextures(1, &this->TextureID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->TextureID);

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


void RenderComponent::Draw() {
	this->shader.use();
	if (!Enabled)
		return;


	if (this->TextureID != 0) {
		glActiveTexture(this->TextureID);
		glBindTexture(GL_TEXTURE_2D, this->TextureID);
	}

	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

float calcTriangleArea(std::vector<float> a, std::vector<float> b, std::vector<float> c) {
	return 0.5f * std::abs((a[0] * (b[1] - c[1]) + b[0] * (c[1] - a[1]) + c[0] * (a[1] - b[1])));
}

std::vector<unsigned int> RenderComponent::Triangulate(std::vector<float> vertices) {
	points.clear();
	for (int i = 0; i < vertices.size(); i += 2)
	{
		points.push_back(std::vector<float> {
			vertices[i],
				vertices[i + 1],
				float(int(i / 5))
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

glm::vec3 RenderComponent::GetCenter() {
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

void RenderComponent::UpdateShape(std::vector<float> vertices, std::vector<unsigned int> indices) {
	Vertices = vertices;
	Indices = indices;

	points.clear();
	edges.clear();
	for (int i = 0; i < vertices.size(); i += 2)
	{
		points.push_back(std::vector<float> {
			vertices[i],
				vertices[i + 1],
				float(int(i / 5))
		});
		i += 3;
	}

	for (int i = 0; i < points.size(); i++)
	{
		std::vector<float> p1 = points[i];
		std::vector<float> p2 = points[(i + 1) % points.size()];
		edges.push_back(std::vector<std::vector<float>> {p1, p2});
	}

	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(float), Vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(unsigned int), Indices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

bool RenderComponent::IsInsideShape(glm::vec3 point) {
	int cnt = 0;
	for (int i = 0; i < edges.size(); i++)
	{
		std::vector<float> point1 = edges[i][0];
		std::vector<float> point2 = edges[i][1];
		bool ycheck = (point.y < point1[1]) != (point.y < point2[1]);
		bool xcheck = point.x < point1[0] + ((point.y - point1[1]) / (point2[1] - point1[1])) * (point2[0] - point1[0]);

		if (xcheck && ycheck) {
			cnt += 1;
		}
	}

	return cnt % 2 == 1;
}

void RenderComponent::ProcessInspectorUI() {
	ImGui::Text("Texture ");
	char selected_texture_path[128] = "None (Click to choose...)";

	if (texture_path != "") {
#if defined(_MSC_VER)
		strcpy_s(selected_texture_path, texture_path.c_str());
#else
		strncpy(selected_texture_path, texture_path.c_str(), sizeof(selected_item_name_top) - 1);
#endif
	}


	ImGui::InputText("##Texture path select field", selected_texture_path, IM_ARRAYSIZE(selected_texture_path), ImGuiInputTextFlags_ReadOnly);

	if (ImGui::IsItemHovered()) {
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
	}

	if (ImGui::IsItemClicked()) {
		IGFD::FileDialogConfig config;
		config.path = ".";
		config.countSelectionMax = 1;
		ImGuiFileDialog::Instance()->OpenDialog("Choose Texture Window", "Choose Texture", ".png,.jpeg", config);
	}

	if (ImGuiFileDialog::Instance()->Display("Choose Texture Window", 32, ImVec2(100, 200))) {
		if (ImGuiFileDialog::Instance()->IsOk()) {
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
			SetTexture(filePathName);
		}

		ImGuiFileDialog::Instance()->Close();
	}
}

void RenderComponent::OnDelete() {

}
