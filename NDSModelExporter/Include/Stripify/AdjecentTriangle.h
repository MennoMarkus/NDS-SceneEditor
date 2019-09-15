#pragma once
#include "Model.h"

struct AdjecentTriangle
{
	// Stored in clockwise order
	const Vertex* vertices[3];
	// Stored in clockwise order
	// Eg adjecentTriangles[0] is between vertex[0] and vertex[1]
	AdjecentTriangle* adjecentTriangles[3] = { nullptr, nullptr, nullptr };

	bool isStripTriangle = false;

	AdjecentTriangle(const Vertex& vertex1, const Vertex& vertex2, const Vertex& vertex3)
	{
		vertices[0] = &vertex1;
		vertices[1] = &vertex2;
		vertices[2] = &vertex3;
	}

	int getAdjecentTriangleCount() const { return (adjecentTriangles[0] == nullptr) + (adjecentTriangles[1] == nullptr) + (adjecentTriangles[2] == nullptr); }
};