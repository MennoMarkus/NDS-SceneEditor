#pragma once

#include "glm/glm.hpp"

class CmpTransform {
public:
	CmpTransform();
	~CmpTransform();

	glm::mat4& get_transform();
private:
	glm::mat4 m_transform;
};