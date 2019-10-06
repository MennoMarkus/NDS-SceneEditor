#pragma once
#include "Stripify/AdjecentTriangle.h"
#include "Stripify/Primative.h"

struct Vertex;

class Stripify
{
public:
	static std::vector<Primative> Strip(const std::vector<Vertex>& vertices);
	static std::vector<Primative> Strip(const std::vector<unsigned int>& indexBuffer, const std::vector<Vertex>& vertexBuffer);

private:
	// Create a new index and vertex buffer from a list of vertices.
	static void createIndexBuffer(const std::vector<Vertex>& vertices, std::vector<unsigned int>& o_indexBuffer, std::vector<Vertex>& o_vertexBuffer);

	// Creates triangles and maps their connectivity to easily find adjecent triangles.
	static std::vector<AdjecentTriangle> createConnectivityGraph(const std::vector<unsigned int>& indexBuffer);
	static bool areTrianglesAdjecent(const AdjecentTriangle& triangle1, const AdjecentTriangle& triangle2);

	// Get the best triangle to be used as startign triangle for a new triangle strip
	static AdjecentTriangle* getBestStripTriangle(std::vector<AdjecentTriangle>& triangles);

	// Create the best triangle strip for a starting triangle
	static std::vector<Vertex> getBestTriangleStrip(AdjecentTriangle& startTriangle, const std::vector<Vertex>& vertexBuffer);
	// Builds a triangle strip from the start triangle, in one direction. The start triangle is not added to the output.
	static void extendTriangleStrip(AdjecentTriangle& startTriangle, int firstVertex, int secondVertex, std::vector<AdjecentTriangle*>& o_triangles, std::vector<unsigned int>& o_indices);
};