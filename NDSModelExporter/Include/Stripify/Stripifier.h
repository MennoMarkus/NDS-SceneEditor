#pragma once
#include "Stripify/AdjecentTriangle.h"

struct Vertex;

class Stripify
{
public:
	static bool Strip(const std::vector<Vertex>& vertices);

private:
	static void createAdjecentTriangles(const std::vector<Vertex>& vertices, std::vector<AdjecentTriangle>& o_adjecentTriangles);
	static bool hasSharedEdge(const AdjecentTriangle& tri1, const AdjecentTriangle& tri2, int& o_tri1EdgeIdx, int& o_tri2EdgeIdx);

	static AdjecentTriangle* getBestStripStartingTriangle(std::vector<AdjecentTriangle>& triangles);
	static std::vector<AdjecentTriangle*> getTriangleStrip(const AdjecentTriangle& startingTriangle);
	static void extendTrinagleStrip(const AdjecentTriangle& startingTriangle, int startingEdge, std::vector<AdjecentTriangle*>& o_triangleStrip);
};