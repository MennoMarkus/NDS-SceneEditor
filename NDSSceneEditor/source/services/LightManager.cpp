#include "services/LightManager.h"

LightManager::LightManager(unsigned int cap) {
	m_lightCap = cap;
	m_nextID = 0;
}

LightManager::~LightManager(){}

bool LightManager::light_add(Light light, unsigned int& returnID){
	//Light cap not reached, add light
	if (m_lightCount < m_lightCap) {
		m_lights[m_nextID] = light;
		returnID = m_nextID;
		
		m_lightCount++;
		m_nextID++;
		return true;
	}
	//Light cap reached, don't add
	return false;
}
bool LightManager::light_update_pos(unsigned int ID, glm::vec3 pos){
	if (m_lights.count(ID) != 0) {
		m_lights[ID].pos = pos;
		return true;
	}
	return false;
}
bool LightManager::light_update_range(unsigned int ID, float range){
	if (m_lights.count(ID) != 0) {
		m_lights[ID].range = range;
		return true;
	}
	return false;
}
bool LightManager::light_update_col(unsigned int ID, glm::vec3 col){
	if (m_lights.count(ID) != 0) {
		m_lights[ID].col = col;
		return true;
	}
	return false;
}
bool LightManager::light_update(unsigned int ID, Light light){
	if (m_lights.count(ID) != 0) {
		m_lights[ID] = light;
		return true;
	}
	return false;
}
bool LightManager::light_delete(unsigned int ID){
	if (m_lights.count(ID) != 0) {
		m_lights.erase(ID);
		m_lightCount--;
		return true;
	}
	return false;
}

Light* LightManager::get_light(unsigned int ID) {
	return &m_lights[ID];
}
std::vector<Light>* LightManager::get_lights() { 
	//IMPORTANT, THIS LIST IS ONLY RELEVANT AT THE MOMENT OF RETREIVING THE LIST
	m_lightVec.clear();

	unsigned int count = 0;
	for (auto const& i : m_lights)
	{
		m_lightVec.push_back(i.second);
		count++;
	}

	//Fill the rest of the data with dummy lights
	for (int i = 0; i < m_lightCap - count; i++) {
		m_lightVec.push_back({glm::vec3(0.f), 0.f, glm::vec3(0.f)});
	}

	return &m_lightVec;
}
unsigned int LightManager::get_lights_count() {
	return m_lightCount;
}