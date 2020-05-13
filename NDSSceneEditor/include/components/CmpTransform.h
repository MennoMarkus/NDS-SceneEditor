#pragma once
#include "glm/glm.hpp"

namespace nds_se
{
	struct CmpTransform 
	{
		glm::mat4 matrix = glm::mat4(1.f);
	};
}