#pragma once
/*
#include "Service.h"

#include <vector>
#include <map>

#include "glm/glm.hpp"

struct Light{
	Light() {
		pos = glm::vec3(0.f);
		range = 0.f;
		col = glm::vec3(0.f);
		
		padding = 0.f;
	}
	Light(glm::vec3 tPos, float tRange, glm::vec3 tCol) : pos(tPos), range(tRange), col(tCol), padding(0.f) {}

	glm::vec3 pos;
	float range;
	glm::vec3 col;

	float padding;
};

class LightManager : public Service{
public:
	LightManager(unsigned int cap);
	~LightManager();

	bool light_add(Light light, unsigned int& returnID);
	bool light_update_pos(unsigned int ID, glm::vec3 pos);
	bool light_update_range(unsigned int ID, float range);
	bool light_update_col(unsigned int ID, glm::vec3 col);
	bool light_update(unsigned int ID, Light light);
	bool light_delete(unsigned int ID);

	Light* get_light(unsigned int ID);
	std::vector<Light>* get_lights();
	unsigned int get_lights_count();
private:
	unsigned int m_lightCap;
	unsigned int m_lightCount;
	unsigned int m_nextID;
	std::map<unsigned int, Light> m_lights;
	std::vector<Light> m_lightVec;
};*/