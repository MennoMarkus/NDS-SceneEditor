#include "components/CmpTransform.h"

CmpTransform::CmpTransform() {
	m_transform = glm::mat4(1.f);
}
CmpTransform::~CmpTransform(){}

glm::mat4& CmpTransform::get_transform() {
	return m_transform;
}