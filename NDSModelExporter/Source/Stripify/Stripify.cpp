#include "Stripify/Stripify.h"
#include "Model.h"
#include <algorithm>
#include <iostream>

std::vector<Primative> Stripify::Strip(const std::vector<Vertex>& vertices)
{
	// Create index and vertex buffers
	std::vector<unsigned int> indexBuffer;
	std::vector<Vertex> vertexBuffer;
	createIndexBuffer(vertices, indexBuffer, vertexBuffer);
	std::cout << "Indices count: " << indexBuffer.size() << " New vertex count: " << vertexBuffer.size() << " Old vertex count: " << vertices.size() << "\n";

	return Strip(indexBuffer, vertexBuffer);
}

std::vector<Primative> Stripify::Strip(const std::vector<unsigned int>& indexBuffer, const std::vector<Vertex>& vertexBuffer)
{
	std::vector<Primative> trianglesStripPrimatives;

	// Create connectivity graph to map all triangle connections
	std::vector<AdjecentTriangle> triangles = createConnectivityGraph(indexBuffer);
	
	Primative trianglesPrimative(PrimativeType::TRIANGLES);
	AdjecentTriangle* triangle = nullptr;
	do 
	{
		// Get the best starting triangle for a new triangle strip
		triangle = getBestStripTriangle(triangles);
		if (triangle != nullptr)
		{
			std::vector<Vertex> triangleStrip = getBestTriangleStrip(*triangle, vertexBuffer);

			if (triangleStrip.size() == 3)
			{
				// Add the loose triangle to the triangles primative
				triangle->isUsed = true;
				trianglesPrimative.vertices.push_back(vertexBuffer[triangle->vertexIdx[0]]);
				trianglesPrimative.vertices.push_back(vertexBuffer[triangle->vertexIdx[1]]);
				trianglesPrimative.vertices.push_back(vertexBuffer[triangle->vertexIdx[2]]);
			}
			else
			{
				// Add the triangle strip to the list of primatives
				Primative triangleStripPrimative(PrimativeType::TRIANGLE_STRIP);
				triangleStripPrimative.vertices = triangleStrip;
				trianglesStripPrimatives.push_back(triangleStripPrimative);
			}
		}
	} while (triangle != nullptr);

	// Add triangles primative to the list of primatives
	if (trianglesPrimative.vertices.size() > 0)
		trianglesStripPrimatives.push_back(trianglesPrimative);

	return trianglesStripPrimatives;
}

void Stripify::createIndexBuffer(const std::vector<Vertex>& vertices, std::vector<unsigned int>& o_indexBuffer, std::vector<Vertex>& o_vertexBuffer)
{
	for (auto& vertex : vertices)
	{
		const auto& bufferVertexIt = std::find(o_vertexBuffer.begin(), o_vertexBuffer.end(), vertex);
		// If the vertex already excists, add it's index
		if (bufferVertexIt != o_vertexBuffer.end())
		{
			o_indexBuffer.push_back(std::distance(o_vertexBuffer.begin(), bufferVertexIt));
		} // Else add the new vertex and it's index
		else
		{
			o_indexBuffer.push_back(o_vertexBuffer.size());
			o_vertexBuffer.push_back(vertex);
		}
	}
}

std::vector<AdjecentTriangle> Stripify::createConnectivityGraph(const std::vector<unsigned int>& indexBuffer)
{
	std::vector<AdjecentTriangle> triangles;
	triangles.reserve(indexBuffer.size() / 3);

	for (int triangleIdx = 0; triangleIdx < indexBuffer.size() / 3; triangleIdx++)
	{
		// Create new triangle
		triangles.emplace_back(indexBuffer[3 * triangleIdx + 0],
							   indexBuffer[3 * triangleIdx + 1],
							   indexBuffer[3 * triangleIdx + 2]);
		AdjecentTriangle& newTriangle = triangles.back();

		// Find all connected triangles
		for (int i = 0; i < triangles.size() - 1; i++)
		{
			AdjecentTriangle& triangle = triangles[i];
			if (areTrianglesAdjecent(triangle, newTriangle))
			{
				newTriangle.addAdjecentTriangle(triangle);
				triangle.addAdjecentTriangle(newTriangle);
			}
		}

	}

	return triangles;
}

bool Stripify::areTrianglesAdjecent(const AdjecentTriangle& triangle1, const AdjecentTriangle& triangle2)
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			// Check for shared edge. 
			// tri1.A --- tr1.B == tri2.A --- tr2.B ||
			// tri1.A --- tr1.B == tri2.B --- tr2.A
			if ((triangle1.vertexIdx[i] == triangle2.vertexIdx[j] && triangle1.vertexIdx[(i + 1) % 3] == triangle2.vertexIdx[(j + 1) % 3]) ||
				(triangle1.vertexIdx[i] == triangle2.vertexIdx[(j + 1) % 3] && triangle1.vertexIdx[(i + 1) % 3] == triangle2.vertexIdx[j]))
				return true;
		}
	}
	return false;
}

AdjecentTriangle* Stripify::getBestStripTriangle(std::vector<AdjecentTriangle>& triangles)
{
	AdjecentTriangle* result = nullptr;
	int minAdjecentTriangle = std::numeric_limits<int>::max();

	for (auto& triangle : triangles)
	{
		// Find the triangle with the least connections, skipping used triangles
		if (!triangle.isUsed && minAdjecentTriangle > triangle.getAdjecentTriangleCount())
		{
			minAdjecentTriangle = triangle.getAdjecentTriangleCount();
			result = &triangle;

			if (minAdjecentTriangle == 0)
				break;
		}
	}
	return result;
}

std::vector<Vertex> Stripify::getBestTriangleStrip(AdjecentTriangle& startTriangle, const std::vector<Vertex>& vertexBuffer)
{
	std::vector<std::vector<AdjecentTriangle*>> triangleStrips;
	std::vector<std::vector<unsigned int>> triangleStripIndices;
	triangleStrips.reserve(6);

	// Build all possible triangle strips
	for (unsigned int vertexIdx = 0; vertexIdx < 3; vertexIdx++)
	{
		for (int clockWiseOrder = 0; clockWiseOrder < 2; clockWiseOrder++)
		{
			unsigned int firstVertexIdx = startTriangle.vertexIdx[vertexIdx];
			unsigned int secondVertexIdx = startTriangle.vertexIdx[(vertexIdx + 1 + clockWiseOrder) % 3];
			unsigned int thirdVertexIdx = startTriangle.vertexIdx[(vertexIdx + 2 - clockWiseOrder) % 3];
			std::vector<AdjecentTriangle*> triangles = { &startTriangle };
			std::vector<unsigned int> indices = { firstVertexIdx, secondVertexIdx, thirdVertexIdx };

			// Extend forwards
			extendTriangleStrip(startTriangle, secondVertexIdx, thirdVertexIdx, triangles, indices);

			std::reverse(triangles.begin(), triangles.end());
			std::reverse(indices.begin(), indices.end());
			secondVertexIdx = firstVertexIdx;
			thirdVertexIdx = secondVertexIdx;
			
			// Extend backwards
			extendTriangleStrip(startTriangle, secondVertexIdx, thirdVertexIdx, triangles, indices);
			triangleStrips.push_back(triangles);
			triangleStripIndices.push_back(indices);
		}
	}

	// Find the largest triangle strip for this starting triangle
	int largestStripIdx = 0;
	size_t maxStripLength = 0;
	for (int i = 0; i < 6; i++)
	{
		if (maxStripLength < triangleStrips[i].size())
		{
			maxStripLength = triangleStrips[i].size();
			largestStripIdx = i;
		}
	}

	// Set triangles as used
	for (auto& triangle : triangleStrips[largestStripIdx])
		triangle->isUsed = true;

	// Get strip vertices
	std::vector<Vertex> result;
	result.reserve(triangleStripIndices[largestStripIdx].size());
	for (auto& index : triangleStripIndices[largestStripIdx])
		result.push_back(vertexBuffer[index]);

	return result;
}

void Stripify::extendTriangleStrip(AdjecentTriangle& startTriangle, int firstVertex, int secondVertex, std::vector<AdjecentTriangle*>& o_triangles, std::vector<unsigned int>& o_indices)
{
	unsigned int previousVertex = firstVertex;
	unsigned int currentVertex = secondVertex;
	AdjecentTriangle* currentTriangle = &startTriangle;

	while (currentTriangle != nullptr)
	{
		AdjecentTriangle* nextTriangle = nullptr;

		// Find an adjecent triangle to connect to
		for (int adjecentTriangleIdx = 0; adjecentTriangleIdx < currentTriangle->getAdjecentTriangleCount(); adjecentTriangleIdx++)
		{
			AdjecentTriangle* adjecentTriangle = currentTriangle->getAdjecentTriangle(adjecentTriangleIdx);

			// Add this triangle if we can connect and it's not already part of the strip
			if (std::find(o_triangles.begin(), o_triangles.end(), adjecentTriangle) == o_triangles.end())
			{
				for (int i = 0; i < 3; i++)
				{
					if (previousVertex == adjecentTriangle->vertexIdx[i] && currentVertex == adjecentTriangle->vertexIdx[(i + 1) % 3] ||
						previousVertex == adjecentTriangle->vertexIdx[(i + 1) % 3] && currentVertex == adjecentTriangle->vertexIdx[i])
					{
						unsigned int newVertex = adjecentTriangle->vertexIdx[(i + 2) % 3];
						o_triangles.push_back(adjecentTriangle);
						o_indices.push_back(newVertex);

						// Set the new current vertices
						previousVertex = currentVertex;
						currentVertex = newVertex;
						nextTriangle = adjecentTriangle;
						break;
					}
				}
			}
		}

		// Move on to the next triangle
		currentTriangle = nextTriangle;
	}
}
