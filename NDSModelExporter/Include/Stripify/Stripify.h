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
	static void createIndexBuffer(const std::vector<Vertex>& vertices, std::vector<unsigned int>& o_indexBuffer, std::vector<Vertex>& o_vertexBuffer);

	static void createAdjecentTriangles(const std::vector<unsigned int>& indices, const std::vector<Vertex>& vertices, std::vector<AdjecentTriangle>& o_adjecentTriangles);
	static bool hasSharedEdge(const AdjecentTriangle& tri1, const AdjecentTriangle& tri2, int& o_tri1EdgeIdx, int& o_tri2EdgeIdx);

	static AdjecentTriangle* getBestStripStartingTriangle(std::vector<AdjecentTriangle>& triangles);
	static std::vector<AdjecentTriangle*> getTriangleStrip(AdjecentTriangle& startingTriangle);
	static void extendTrinagleStrip(const AdjecentTriangle& startingTriangle, int startingEdge, std::vector<AdjecentTriangle*>& o_triangleStrip);
};