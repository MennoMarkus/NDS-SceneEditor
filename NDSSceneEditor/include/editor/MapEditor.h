#pragma once

#include <string>
#include <vector>

#include "glm/glm.hpp"

#include "services/ServiceLocator.h"

class Program;
class CmpTransform;
class Model;
class EntLightPoint;
class Mesh;

class MapEditor {
public:
	MapEditor(nds_se::ServiceLocator* serviceLocator);
	~MapEditor();
	
	void run();
private:
	void map_export();
	void map_export_obj();
	void generate_shadows(const glm::vec3& shadowDir);
	
	//Programs
	Program* m_progLighting;
	Program* m_progLamps;
	Program* m_progShadTest;

	//Editor variables
	float m_prevTime = 0.f;
	float m_camRot = 0.f;
	CmpTransform* m_gizmoTrans = nullptr;
	float sunRotZ = 0.f;
	float sunRotX = 0.f;
	std::vector<EntLightPoint*> m_lightPoints;

	//Utility models
	std::string m_modMap = "boxroom/Boxroom.obj";
	std::string m_modLamp = "unit_sphere.obj";

	//Testing variables
	CmpTransform* m_transTest;
	std::string m_modCol = "export.obj";
	Mesh* m_shadowMesh;
	std::vector<glm::vec3> m_shadowData;
};