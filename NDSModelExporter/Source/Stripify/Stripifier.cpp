#include "Stripify/Stripify.h"
#include "Model.h"
#include <algorithm>

bool Stripify::Strip(const std::vector<Vertex>& vertices)
{
	// TODO index buffer

	// Connect triangles
	std::vector<AdjecentTriangle> triangles;
	createAdjecentTriangles(vertices, triangles);

	// Create all triangle strips
	AdjecentTriangle* startingTriangle;
	do
	{
		// TODO mark new strip as visted
		// TODO add new strip to strips
		// TODO add strip with 1 triangle to triangles
		startingTriangle = getBestStripStartingTriangle(triangles);
		std::vector<AdjecentTriangle*> triangleStrip = getTriangleStrip(*startingTriangle);
	} while (startingTriangle != nullptr);

	return true;
}

void Stripify::createAdjecentTriangles(const std::vector<Vertex>& vertices, std::vector<AdjecentTriangle>& o_adjecentTriangles)
{
	o_adjecentTriangles.reserve(vertices.size() / 3);

	// Get all triangles
	for (int i = 0; i < vertices.size() / 3; i++)
	{
		o_adjecentTriangles.emplace_back(vertices[i + 0], vertices[i + 1], vertices[i + 2]);
		AdjecentTriangle& triangle = o_adjecentTriangles.back();

		// Find adjecent triangles
		for (int j = 0; j < (vertices.size() / 3) - 1; j++)
		{
			AdjecentTriangle& otherTriangle = o_adjecentTriangles[j];
			int triangleEdgeIdx = -1;
			int otherTriangleEdgeIdx = -1;

			// Connect triangles if they share an edge
			if (hasSharedEdge(triangle, otherTriangle, triangleEdgeIdx, otherTriangleEdgeIdx))
			{
				triangle.adjecentTriangles[triangleEdgeIdx] = &otherTriangle;
				otherTriangle.adjecentTriangles[otherTriangleEdgeIdx] = &triangle;
			}
		}
	}
}

bool Stripify::hasSharedEdge(const AdjecentTriangle& tri1, const AdjecentTriangle& tri2, int& o_tri1EdgeIdx, int& o_tri2EdgeIdx)
{
	for (int i = 0; i < 3; i++)
	{
		// Get tri1 edge
		const Vertex* tri1Vertex1 = tri1.vertices[i];
		const Vertex* tri1Vertex2 = tri1.vertices[(i + 1) % 3];

		for (int j = 0; j < 3; j++)
		{
			// Get tri2 edge
			const Vertex* tri2Vertex1 = tri2.vertices[i];
			const Vertex* tri2Vertex2 = tri2.vertices[(i + 1) % 3];

			if (tri1Vertex1 == tri2Vertex2 && tri1Vertex2 == tri2Vertex1)
			{
				o_tri1EdgeIdx = i;
				o_tri2EdgeIdx = j;
				return true;
			}
		}
	}
	return false;
}

AdjecentTriangle* Stripify::getBestStripStartingTriangle(std::vector<AdjecentTriangle>& triangles)
{
	AdjecentTriangle* leastAdjecentTriangles = nullptr;
	int adjecentTrianglesCount = 4;

	// Get the triangle with the least amount of adjecent triangles
	for (auto& triangle : triangles)
	{
		if (triangle.getAdjecentTriangleCount() < adjecentTrianglesCount && triangle.isStripTriangle == false)
		{
			leastAdjecentTriangles = &triangle;
			adjecentTrianglesCount = triangle.getAdjecentTriangleCount();
			if (adjecentTrianglesCount == 1)
				break;
		}
	}

	return leastAdjecentTriangles;
}

std::vector<AdjecentTriangle*> Stripify::getTriangleStrip(const AdjecentTriangle& startingTriangle)
{
	std::vector<AdjecentTriangle*> triangleStrips[3];

	// Create a triangle strip for each edge of the starting triangle
	for (int i = 0; i < 3; i++)
	{
		// Get the edge direction to extend backwards from
		int backwardsEdge = i;
		AdjecentTriangle* adjecentTriangle1 = startingTriangle.adjecentTriangles[(backwardsEdge + 1) % 3];
		AdjecentTriangle* adjecentTriangle2 = startingTriangle.adjecentTriangles[(backwardsEdge + 2) % 3];
		if (adjecentTriangle1 != nullptr && adjecentTriangle2 != nullptr)
		{
			// Pick the triangle with the least amount of adjecent triagles
			if (adjecentTriangle1->getAdjecentTriangleCount() < adjecentTriangle2->getAdjecentTriangleCount())
				backwardsEdge = (backwardsEdge + 1) % 3;
			else
				backwardsEdge = (backwardsEdge + 2) % 3;
		}
		else if (adjecentTriangle1 != nullptr)
			backwardsEdge = (backwardsEdge + 1) % 3;
		else if (adjecentTriangle2 != nullptr)
			backwardsEdge = (backwardsEdge + 2) % 3;

		extendTrinagleStrip(startingTriangle, i, triangleStrips[i]); // Extend forwards
		std::reverse(triangleStrips[i].begin(), triangleStrips[i].end());
		extendTrinagleStrip(startingTriangle, backwardsEdge, triangleStrips[i]); // Extend backwards
	}

	// Pick the largest triangle strip
	int triangleStripsSize[3] = { triangleStrips[0].size(), triangleStrips[1].size(), triangleStrips[2].size() };
	int longestSize = std::max(triangleStripsSize[0], std::max(triangleStripsSize[1], triangleStripsSize[2]));

	std::vector<AdjecentTriangle*> returnStrip;
	if      (triangleStripsSize[0] == longestSize) returnStrip = triangleStrips[0];
	else if (triangleStripsSize[1] == longestSize) returnStrip = triangleStrips[1];
	else                                           returnStrip = triangleStrips[2];

	// Mark strip triangles as part of this strip
	for (auto& triangle : returnStrip)
		triangle->isStripTriangle = true;

	return returnStrip;
}

void Stripify::extendTrinagleStrip(const AdjecentTriangle& startingTriangle, int startingEdge, std::vector<AdjecentTriangle*>& o_triangleStrip)
{
	const AdjecentTriangle* currentTriangle = &startingTriangle;
	AdjecentTriangle* adjecentTriangle1;
	AdjecentTriangle* adjecentTriangle2;
	int fromEdgeIdx = 0;

	do
	{
		// Pick the adjecent triangles as new candidates for the strip
		adjecentTriangle1 = currentTriangle->adjecentTriangles[(fromEdgeIdx + 1) % 3];
		adjecentTriangle2 = currentTriangle->adjecentTriangles[(fromEdgeIdx + 2) % 3];

		// Remove the candidate triangle if it has already been visted
		if (adjecentTriangle1 != nullptr)
		{
			if (std::find(o_triangleStrip.begin(), o_triangleStrip.end(), adjecentTriangle1) == o_triangleStrip.end() ||
				adjecentTriangle1->isStripTriangle)
				adjecentTriangle1 = nullptr;
		}
		if (adjecentTriangle2 != nullptr)
		{
			if (std::find(o_triangleStrip.begin(), o_triangleStrip.end(), adjecentTriangle2) == o_triangleStrip.end() ||
				adjecentTriangle2->isStripTriangle)
				adjecentTriangle2 = nullptr;
		}

		// Add a candidate triangle to the strip
		if (adjecentTriangle1 != nullptr && adjecentTriangle2 != nullptr)
		{
			// Pick the triangle with the least amount of adjecent triagles
			if (adjecentTriangle1->getAdjecentTriangleCount() < adjecentTriangle2->getAdjecentTriangleCount())
				o_triangleStrip.push_back(adjecentTriangle1);
			else
				o_triangleStrip.push_back(adjecentTriangle2);
		}
		else if (adjecentTriangle1 != nullptr)
			o_triangleStrip.push_back(adjecentTriangle1);
		else if (adjecentTriangle2 != nullptr)
			o_triangleStrip.push_back(adjecentTriangle2);

		if (o_triangleStrip.size() > 0)
		{
			// Find the current triangle in the new adjecent triangles edges to get the new edge idx
			for (int i = 0; i < 3; i++)
			{
				if (o_triangleStrip.back()->adjecentTriangles[i] == currentTriangle)
				{
					fromEdgeIdx = i;
					break;
				}
			}
			currentTriangle = o_triangleStrip.back();
		}
	} while (!(adjecentTriangle1 == nullptr && adjecentTriangle2 == nullptr));
}
