#pragma once
#include "Model.h"

class AdjecentTriangle
{
public:
	// Stored in clockwise order
	unsigned int vertexIdx[3];
	bool isUsed = false;

private:
	// Sorted by the amount of adjecent triangles
	std::vector<AdjecentTriangle*> connectedTriangles;

public:
	AdjecentTriangle(unsigned int vertex1Idx, unsigned int vertex2Idx, unsigned int vertex3Idx)
	{
		vertexIdx[0] = vertex1Idx;
		vertexIdx[1] = vertex2Idx;
		vertexIdx[2] = vertex3Idx;
	}

	// Helpers for adding/getting adjecent triangles. Making sure sorting is maintained.
	int getAdjecentTriangleCount() const { return connectedTriangles.size(); }

	AdjecentTriangle* getAdjecentTriangle(int idx) { return connectedTriangles[idx]; }

	void addAdjecentTriangle(AdjecentTriangle& triangle)
	{
		// Sort triangles based on connectivity
		auto it = std::lower_bound(connectedTriangles.begin(), connectedTriangles.end(), &triangle, 
									[](AdjecentTriangle* a, AdjecentTriangle* b)
									{ return a->getAdjecentTriangleCount() < b->getAdjecentTriangleCount();	});
		connectedTriangles.insert(it, &triangle);
	}
};