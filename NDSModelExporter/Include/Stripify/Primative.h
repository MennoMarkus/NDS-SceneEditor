#pragma once
#include "Model.h"

enum class PrimativeType
{
	TRIANGLES = 0,
	TRIANGLE_STRIP = 2
};

struct Primative
{
	PrimativeType type;
	bool isClockWiseOrder;
	std::vector<Vertex> vertices;

	Primative(PrimativeType type) : type(type) {}
};