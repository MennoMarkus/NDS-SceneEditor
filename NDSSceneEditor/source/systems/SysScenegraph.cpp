#include "systems/SysScenegraph.h"

#include <string>

#include "imgui/imgui.h"
#include "imgui/imguizmo/ImGuizmo.h"

#include "components/CmpTransform.h"

namespace nds_se
{
	SysScenegraph::SysScenegraph() : SystemBase("SysScenegraph") {}

	void SysScenegraph::update() {
		assert(m_entityManager != nullptr && "System hasn't been registered with an entity manager.");

		// Start log window
		if (!ImGui::Begin("ImguiTest"))
		{
			ImGui::End();
			return;
		}

		auto entityIt = m_entities.begin();
		while (entityIt != m_entities.end())
		{
			if (m_entityManager->hasComponent<std::string>(*entityIt)) {
				if (ImGui::CollapsingHeader(m_entityManager->getComponent<std::string>(*entityIt).c_str()))
				{
					// Draw components
						// Transform component
					if (m_entityManager->hasComponent<CmpTransform>(*entityIt)) {
						if (ImGui::CollapsingHeader("Transform")) {
							CmpTransform& comp = m_entityManager->getComponent<CmpTransform>(*entityIt);
							glm::vec3 pos, rot, scale;

							ImGuizmo::DecomposeMatrixToComponents(&comp.matrix[0][0], &pos.x, &rot.x, &scale.x);
							ImGui::DragFloat3("Position", &pos.x, 0.1f);
							ImGui::DragFloat3("Rotation", &rot.x, 0.1f);
							ImGui::DragFloat3("Scale", &scale.x, 0.1f, 0.f);
							ImGuizmo::RecomposeMatrixFromComponents(&pos.x, &rot.x, &scale.x, &comp.matrix[0][0]);
						}
					}
						// Int component
					if (m_entityManager->hasComponent<int>(*entityIt)) {
						ImGui::InputInt("input int", &m_entityManager->getComponent<int>(*entityIt));
					}
						// Float component
					if (m_entityManager->hasComponent<float>(*entityIt)) {
						ImGui::InputFloat("input float", &m_entityManager->getComponent<float>(*entityIt));
					}
				}
			}
			entityIt++;
		}
		ImGui::End();
	}
}