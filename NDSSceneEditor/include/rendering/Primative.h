#pragma once
#include <vector>

namespace nds_se
{
	enum PrimativeType
	{
		TRIANGLES = 0x0004, // OpenGL value
		TRIANGLE_STRIP = 0x0005
	};

	struct Primative
	{
		PrimativeType m_type;
		std::vector<unsigned int> m_indices;
	};
}