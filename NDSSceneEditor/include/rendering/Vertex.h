#pragma once
#include "glm/glm.hpp"

namespace nds_se
{
	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 textureCoordinate;
		glm::vec4 color;
	};
}