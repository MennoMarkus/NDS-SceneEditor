#include "editor/MapEditor.h"
/*
#include <iostream>
#include <algorithm>
#include <iostream>
#include <fstream>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "imgui/imgui.h"
#include "imgui/ImGuizmo.h"

#include "services/ResourceManager.h"
#include "rendering/Window.h"
#include "services/Renderer.h"
#include "services/LightManager.h"
#include "services/DebugRenderer.h"
#include "rendering/Program.h"
#include "rendering/Mesh.h"
#include "components/CmpTransform.h"
#include "entities/EntLightPoint.h"
#include "utilities/util_string.h"

struct VertexCol {
	glm::vec3 pos;
	glm::vec3 col;
};
struct VertexInterpreted {
	glm::vec3 pos;
	glm::vec3 col;
	glm::vec3 norm;
	glm::vec2 uv;
};
struct Edge {
	glm::vec3 posBegin;
	glm::vec3 posEnd;
};
const float LIGHT_STRENGTH = 0.8;
const glm::vec3 LIGHT_AMBIENT = glm::vec3(0.2, 0.2, 0.2);
const float MERGEDIST = 0.01f;//Collapse distance
const float EXTRUDE = 100.f;

MapEditor::MapEditor(nds_se::ServiceLocator* serviceLocator) {
	//Get service
	ResourceManager* RM = nds_se::ServiceLocator::get().getService<ResourceManager>();
	Renderer* renderer = nds_se::ServiceLocator::get().getService<Renderer>();
	LightManager* LM = nds_se::ServiceLocator::get().getService<LightManager>();

	//Set up resources
	m_progLighting = new Program("resources/shaders/LightingShd.glsl.vs", "resources/shaders/LightingShd.glsl.fs");
	m_progLamps = new Program("resources/shaders/LampShd.glsl.vs", "resources/shaders/LampShd.glsl.fs");
	m_progShadTest = new Program("resources/shaders/ShadowTestShd.glsl.vs", "resources/shaders/ShadowTestShd.glsl.fs");
	RM->model_load({ ATTR_POS, ATTR_COL, ATTR_NORM, ATTR_UV }, m_modMap, true);
	RM->model_load({ ATTR_POS }, m_modLamp);
	m_transTest = new CmpTransform();
		//Generate the shadows for the map
	generate_shadows(glm::vec3(-1.f, -1.f, -1.f));

	//Set post processing shaders
	renderer->post_add("postTest", "resources/shaders/PostTestShd.glsl.fs");

	//Set renderer settings
	renderer->set_texture_name("texDiffuse");
	renderer->set_texture_name_post("texProcess");
	renderer->set_matrix_name("mvp");
	//renderer->uniform_buffer_add("lights", (3 + 1 + 3 + 1) * 4 * 20);

	//Add framebuffers
	renderer->framebuffer_add("pass_forward", glm::vec2(1.f,1.f), true, /*GL_NONE//GL_DEPTH24_STENCIL8);
}
MapEditor::~MapEditor(){}

void MapEditor::run() {
	return;
	//Set graphics settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
	//Get services
	ResourceManager* RM = nds_se::ServiceLocator::get().getService<ResourceManager>();
	Renderer* renderer = nds_se::ServiceLocator::get().getService<Renderer>();
	nds_se::Window* window = nullptr;// nds_se::ServiceLocator::get().getService<nds_se::Window>();
	LightManager* LM = nds_se::ServiceLocator::get().getService<LightManager>();
	DebugRenderer* debugRenderer = nds_se::ServiceLocator::get().getService<DebugRenderer>();

	//Clear framebuffers
	int width, height;
	glfwGetWindowSize(window->getGLFWWindow(), &width, &height);
	renderer->framebuffers_resize(width, height);
	//renderer->framebuffer_clear("", GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, glm::vec4(0.2f, 0.3f, 0.3f, 1.0f));
	renderer->framebuffers_clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, glm::vec4(0.2f, 0.3f, 0.3f, 1.0f));

	//Set forward frame buffer
	renderer->framebuffer_set("pass_forward");

	//Set up a projection
	float timeValue = glfwGetTime();
	if (glfwGetKey(window->getGLFWWindow(), GLFW_KEY_A)) {
		m_camRot -= timeValue-m_prevTime;
	}
	if (glfwGetKey(window->getGLFWWindow(), GLFW_KEY_D)) {
		m_camRot += timeValue-m_prevTime;
	}
	m_prevTime = timeValue;

	glm::mat4 view = glm::lookAt(glm::vec3(cos(m_camRot)*3.f, sin(m_camRot)*3.f, 2.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 1.f));
	glm::mat4 proj = glm::perspectiveFov(90.f, (float)width, (float)height, 0.1f, 1000.f);
	glm::mat4 mvp = proj * view;

	//Set lighting uniform
	std::vector<Light>* lightData = LM->get_lights();
	renderer->uniform_buffer_set("lights", (3 + 1 + 3 + 1) * 4 * 20, lightData->data());

	//Draw the model
	renderer->set_program(m_progLighting);
	renderer->set_texture_name("texDiffuse");
	renderer->set_view_projection(&mvp);
	renderer->model_draw(m_modMap, m_transTest);
	renderer->render();

	//Draw the lamps
	CmpTransform lampTransform;

	renderer->set_program(m_progLamps);
	renderer->set_texture_name("");
	renderer->set_view_projection(&mvp);
	for (int i = 0; i < LM->get_lights_count(); i++) {
		m_progLamps->set_uniform("col", (*LM->get_lights())[i].col);

		glm::mat4& lampMat = lampTransform.get_transform();
		lampMat = glm::mat4(1.f);
		lampMat = glm::translate(lampMat, (*LM->get_lights())[i].pos);
		lampMat = glm::scale(lampMat, glm::vec3(0.2f));

		renderer->model_render(m_modLamp, &lampTransform);
	}

	//Reset frame buffer
	renderer->framebuffer_set("");

	//Render post process test
	renderer->render_post("postTest", renderer->framebuffer_get_texture("pass_forward"));

	//Draw shadow mesh
	/*m_progShadTest->activate();
	m_progShadTest->set_uniform("mvp", mvp);
	m_shadowMesh->draw();//

	//Draw the debug lines
	debugRenderer->set_view_projection(&mvp);
	//debugRenderer->add_line(glm::vec3(0.f,0.f,0.f), glm::vec3(1.f,0.f,0.f), glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 0.f, 1.f));
	debugRenderer->render(true);


	//Set up the ImGui window for map editing
	//ImGui::ShowDemoWindow();
	bool open = true;
	ImGui::Begin("Editor", &open, ImGuiWindowFlags_None);
	//Direction lighting
	if (ImGui::CollapsingHeader("Sun direction")) {
		ImGui::DragFloat("Rotation Z-axis", &sunRotZ);
		ImGui::DragFloat("Rotation X-axis", &sunRotX);

		if (sunRotZ < 0) {
			sunRotZ += 360;
		}


	}
	//Light sources
	if (ImGui::CollapsingHeader("Light sources")) {
		//Button to add a new light source:
		if (ImGui::Button("Add light point")) {
			unsigned int LID;
			if (LM->light_add({glm::vec3(0.f,0.f,0.f), 8.f, glm::vec3(1.f,1.f,1.f)}, LID)) {
				EntLightPoint* lightPoint = new EntLightPoint();
				lightPoint->set_lightID(LID);
				lightPoint->set_colour(glm::vec3(1.f, 1.f, 1.f));
				lightPoint->set_range(8.f);
				m_lightPoints.push_back(lightPoint);
				m_gizmoTrans = static_cast<CmpTransform*>(m_lightPoints[m_lightPoints.size()-1]);
			}
		}
		//Button to deselect currently selected light source
		ImGui::SameLine();
		if (ImGui::Button("Deselect")) {
			m_gizmoTrans = nullptr;
		}
		ImGui::SameLine();
		if (ImGui::Button("Export")) {
			map_export_obj();
		}

		//Loop over every light entity
		for (int i = 0; i < m_lightPoints.size(); i++) {
			//Get pointer to the light entity
			EntLightPoint* lightPoint = m_lightPoints[i];

			//Create the light node
			std::string nodeName = "Light " + std::to_string(i);
			glm::vec3& col = lightPoint->get_colour();
			ImGui::PushStyleColor(ImGuiCol_Text, {col.r, col.g, col.b, 1.f});
			if (ImGui::TreeNode(nodeName.c_str())) {
				ImGui::PopStyleColor();

				//Add button to select light source for editing
				if (ImGui::Button("Select")) {
					m_gizmoTrans = static_cast<CmpTransform*>(lightPoint);
				}
				ImGui::SameLine();
				if (ImGui::Button("Delete")) {
					//Delete light point from light manager
					LM->light_delete(lightPoint->get_lightID());

					//Delete Gizmo when it's active on the current light entity
					if (m_gizmoTrans == static_cast<CmpTransform*>(lightPoint)) {
						m_gizmoTrans = nullptr;
					}

					//Delete light entity
					delete lightPoint;
					m_lightPoints.erase(m_lightPoints.begin() + i);

					ImGui::TreePop();

					i--;
					continue;
				}

				//Change the positional value
				glm::vec3 pos, rot, scale;
				ImGuizmo::DecomposeMatrixToComponents(&(lightPoint->get_transform())[0][0], &pos.x, &rot.x, &scale.x);
				ImGui::DragFloat3("Position", &pos.x, 0.1f);
				ImGuizmo::RecomposeMatrixFromComponents(&pos.x, &rot.x, &scale.x, &(lightPoint->get_transform())[0][0]);
				LM->light_update_pos(lightPoint->get_lightID(), pos);

				//Change the colour value
				ImGui::ColorEdit3("Color", &col.r, ImGuiColorEditFlags_Float);
				LM->light_update_col(lightPoint->get_lightID(), col);

				//Change the range value
				float& range = lightPoint->get_range();
				ImGui::DragFloat("Range", &range, 0.05f, 0.05f, 1000.f);
				LM->light_update_range(lightPoint->get_lightID(), range);
				
				ImGui::TreePop();
			}
			else {
				ImGui::PopStyleColor();
			}
		}
	}
	ImGui::End();

	//Set up ImgUizmo
	if (m_gizmoTrans) {
		ImGuizmo::Manipulate(&view[0][0], &proj[0][0], ImGuizmo::TRANSLATE, ImGuizmo::WORLD, &(m_gizmoTrans->get_transform())[0][0]);
		glm::vec3 pos, meh, beh;
		ImGuizmo::DecomposeMatrixToComponents(&(m_gizmoTrans->get_transform())[0][0], &pos.x, &meh.x, &beh.x);
		LM->light_update_pos(static_cast<EntLightPoint*>(m_gizmoTrans)->get_lightID(), pos);
	}
}
float lighting_compute(const glm::vec3& vPos, const glm::vec3& vNorm, const glm::vec3& lPos, float lRange) {
	float test = std::max(1.f,2.f);
	float prod = std::max(dot(vNorm, glm::normalize(lPos - vPos)), 0.f);
	float dist = std::max((lRange - glm::distance(vPos, lPos)) / lRange, 0.f);
	return dist * prod;
}
void MapEditor::map_export(){
	ResourceManager* RM = nds_se::ServiceLocator::get().getService<ResourceManager>();
	LightManager* LM = nds_se::ServiceLocator::get().getService<LightManager>();

	std::unordered_map<std::string, std::vector<float> >* rawData = RM->get_model(m_modMap)->get_data();
	std::string clipText = "";

	for (auto& it : (*rawData)){
		std::vector<VertexInterpreted>* interData = reinterpret_cast<std::vector<VertexInterpreted>*>(&it.second);
		for (int i = 0; i < interData->size(); i++) {
			glm::vec3& vertPos = (*interData)[i].pos;
			glm::vec3& vertNorm = (*interData)[i].norm;
			glm::vec3 lightCol = LIGHT_AMBIENT;

			for (int j = 0; j < m_lightPoints.size(); j++) {
				EntLightPoint* lightPoint = m_lightPoints[j];
				Light* lightData = LM->get_light(lightPoint->get_lightID());
				lightCol += lightData->col * lighting_compute(vertPos, vertNorm, lightData->pos, lightData->range) * LIGHT_STRENGTH;
			}

			clipText += "glColor3f(";
			clipText += std::to_string(lightCol.r);
			clipText += "f,";
			clipText += std::to_string(lightCol.g);
			clipText += "f,";
			clipText += std::to_string(lightCol.b);
			clipText += "f);\n";
			clipText += "glVertex3f(";
			clipText += std::to_string(vertPos.x);
			clipText += "f,";
			clipText += std::to_string(vertPos.y);
			clipText += "f,";
			clipText += std::to_string(vertPos.z);
			clipText += "f);\n";
		}
	}

	to_clipboard(clipText);
}

void MapEditor::map_export_obj() {
	//Create file
	std::ofstream modFile;
	modFile.open("exports/export.obj");
	
	ResourceManager* RM = nds_se::ServiceLocator::get().getService<ResourceManager>();
	LightManager* LM = nds_se::ServiceLocator::get().getService<LightManager>();

	std::unordered_map<std::string, std::vector<float> >* rawData = RM->get_model(m_modMap)->get_data();

	unsigned int indices = 0;
	for (auto& it : (*rawData)) {
		std::vector<VertexInterpreted>* interData = reinterpret_cast<std::vector<VertexInterpreted>*>(&it.second);

		for (int i = 0; i < interData->size(); i++) {
			const glm::vec3& vertPos = (*interData)[i].pos;
			const glm::vec3& vertNorm = (*interData)[i].norm;
			glm::vec3 lightCol = LIGHT_AMBIENT;

			for (int j = 0; j < m_lightPoints.size(); j++) {
				EntLightPoint* lightPoint = m_lightPoints[j];
				Light* lightData = LM->get_light(lightPoint->get_lightID());
				lightCol += lightData->col * lighting_compute(vertPos, vertNorm, lightData->pos, lightData->range) * LIGHT_STRENGTH;
			}

			//Write vertices
			modFile << "v " << vertPos.x << " " << vertPos.y << " " << vertPos.z << " " << lightCol.r << " " << lightCol.g << " " << lightCol.b << "\n";
			indices++;
		}
	}

	//Write indices
	for (unsigned int i = 1; i <= indices; i+= 3) {
		modFile << "f " << i << " " << i + 1 << " " << i + 2 << " " << "\n";
	}

	//Close file
	modFile.close();
}

void MapEditor::generate_shadows(const glm::vec3& shadowDir) {
	ResourceManager* RM = nds_se::ServiceLocator::get().getService<ResourceManager>();

	//List of edges that will be used to determine the silhouette
	std::vector<Edge> edges;

	//Access model data
	std::unordered_map<std::string, std::vector<float> >* rawData = RM->get_model(m_modMap)->get_data();

	//Determine silhouette edges of polygons
	unsigned int indices = 0;
	for (auto& i : (*rawData)) {
		std::vector<VertexInterpreted>* interData = reinterpret_cast<std::vector<VertexInterpreted>*>(&i.second);
		//Steps of 3 to loop over every polygon
		for (int j = 0; j < interData->size(); j+=3) {
			glm::vec3& vertPos0 = (*interData)[j].pos;
			glm::vec3& vertPos1 = (*interData)[j+1].pos;
			glm::vec3& vertPos2 = (*interData)[j+2].pos;

			glm::vec3 edge0 = vertPos1 - vertPos0;
			glm::vec3 edge1 = vertPos2 - vertPos0;

			glm::vec3 crossNorm = glm::cross(edge0,edge1);

			if (glm::dot(crossNorm,shadowDir) >= 0.f){
				edges.push_back({ vertPos0, vertPos1 });
				edges.push_back({ vertPos1, vertPos2 });
				edges.push_back({ vertPos2, vertPos0 });
			}
		}
	}

	//Discard overlapping edges
	for (unsigned int i = 0; i < edges.size(); i++) {
		for (unsigned int j = i+1; j < edges.size(); j++) {
			Edge& edge0 = edges[i];
			Edge& edge1 = edges[j];
			
			//Testing: Only inversed order edges overlap properly?!?!?
			if ((glm::distance(edge0.posBegin, edge1.posEnd) < MERGEDIST && glm::distance(edge0.posEnd, edge1.posBegin) < MERGEDIST)){
				edges.erase(edges.begin() + j);
				edges.erase(edges.begin() + i);

				i--;
				break;
			}
		}
	}

	//Discard aligned edges
	for (unsigned int i = 0; i < edges.size(); i++) {
		for (unsigned int j = i + 1; j < edges.size(); j++) {
			Edge& edge0 = edges[i];
			Edge& edge1 = edges[j];

			//Edges are connected with a single point
			if (glm::distance(edge0.posEnd, edge1.posBegin) < MERGEDIST || glm::distance(edge1.posEnd, edge0.posBegin) < MERGEDIST){
				glm::vec3 norm0 = glm::normalize(edge0.posEnd - edge0.posBegin);
				glm::vec3 norm1 = glm::normalize(edge1.posEnd - edge1.posBegin);

				//Check if the normals align (maybe we should use another constant than mergedist)
				if (glm::dot(norm0, norm1) > 1.f - MERGEDIST) {
					//Merge edges
					if (glm::distance(edge0.posEnd, edge1.posBegin) < MERGEDIST) {
						edges[i] = { edge0.posBegin, edge1.posEnd };
					}
					else {
						edges[i] = { edge1.posBegin, edge0.posEnd };
					}
					edges.erase(edges.begin() + j);

					i--;
					break;
				}
			}
		}
	}

	//Add edges to debug renderer
	DebugRenderer* debugRenderer = nds_se::ServiceLocator::get().getService<DebugRenderer>();
	for (unsigned int i = 0; i < edges.size(); i++) {
		Edge& edge0 = edges[i];

		debugRenderer->add_line(edge0.posBegin, glm::vec3(1.f,0.f,0.f), edge0.posEnd, glm::vec3(1.f,0.f,0.f));
		debugRenderer->add_line(edge0.posBegin, glm::vec3(1.f, 0.f, 0.f), edge0.posBegin + glm::vec3(0.1f, 0.f, 0.f), glm::vec3(1.f, 0.f, 0.f));
		debugRenderer->add_line(edge0.posBegin, glm::vec3(1.f, 0.f, 0.f), edge0.posBegin + glm::vec3(0.f, 0.1f, 0.f), glm::vec3(1.f, 0.f, 0.f));
		debugRenderer->add_line(edge0.posBegin, glm::vec3(1.f, 0.f, 0.f), edge0.posBegin + glm::vec3(0.f, 0.f, 0.1f), glm::vec3(1.f, 0.f, 0.f));
	}

	//Create shadowmesh
	glm::vec3 shadowDirNorm = normalize(shadowDir);
	for (unsigned int i = 0; i < edges.size(); i++) {
		Edge& edge0 = edges[i];

		m_shadowData.push_back(glm::vec3(edge0.posEnd));
		m_shadowData.push_back(glm::vec3(edge0.posEnd + shadowDirNorm * EXTRUDE));
		m_shadowData.push_back(glm::vec3(edge0.posBegin));
		m_shadowData.push_back(glm::vec3(edge0.posBegin + shadowDirNorm * EXTRUDE));
		m_shadowData.push_back(glm::vec3(edge0.posBegin));
		m_shadowData.push_back(glm::vec3(edge0.posEnd + shadowDirNorm * EXTRUDE));
	}
	std::vector<unsigned int> attributes;
	attributes.push_back(3);
	m_shadowMesh = new Mesh();
	m_shadowMesh->set(&m_shadowData, nullptr, &attributes, "Default");
}*/