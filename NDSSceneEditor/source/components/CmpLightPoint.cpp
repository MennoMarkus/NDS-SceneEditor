#include "components/CmpLightPoint.h"

glm::vec3& CmpLightPoint::get_colour() { return m_col; }
unsigned int CmpLightPoint::get_lightID() { return m_LID; }
float& CmpLightPoint::get_range() { return m_range; }

void CmpLightPoint::set_colour(glm::vec3 col) { m_col = col; }
void CmpLightPoint::set_range(float range) { m_range = range; }
void CmpLightPoint::set_lightID(unsigned int LID) { m_LID = LID; }