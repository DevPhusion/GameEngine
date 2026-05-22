#include "Renderer.h"

Renderer::Renderer(std::vector<Polygon> polygon) {
	poly = polygon;
}

void Renderer::AddPolygon(Polygon polygon) {
	poly.push_back(polygon);
}

void Renderer::Draw() {
	for (int i = 0; i < poly.size(); i++)
	{
		poly[i].Draw();
	}
}
