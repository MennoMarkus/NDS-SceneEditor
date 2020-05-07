#pragma once

#include "glm/glm.hpp"

class CmpLightPoint {
public:
	glm::vec3& get_colour();
	float& get_range();
	unsigned int get_lightID();

	void set_colour(glm::vec3 col);
	void set_range(float range);
	void set_lightID(unsigned int LID);
private:
	glm::vec3 m_col;
	float m_range;
	unsigned int m_LID;
};