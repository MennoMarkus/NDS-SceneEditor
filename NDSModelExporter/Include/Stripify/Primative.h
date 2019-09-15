#pragma once
#include "Model.h"

enum class PrimativeType
{
	TRIANGLES,
	TRIANGLE_STRIP
};

struct Primative
{
	PrimativeType type;
	std::vector<Vertex> vertices;

	Primative(PrimativeType type) : type(type) {}
};