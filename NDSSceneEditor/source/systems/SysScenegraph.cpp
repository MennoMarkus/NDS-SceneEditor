#include "systems/SysScenegraph.h"

#include <string>

#include "imgui/imgui.h"
#include "imgui/imguizmo/ImGuizmo.h"

#include "components/CmpTransform.h"

namespace nds_se
{
	SysScenegraph::SysScenegraph() : SystemBase("SysScenegraph") {}

	void SysScenegraph::update() 
	{
		assert(getEntityManager() != nullptr && "System hasn't been registered with an entity manager.");

		// Start log window
		if (!ImGui::Begin("Scene graph"))
		{
			ImGui::End();
			return;
		}

		auto entityIt = getEntities().begin();
		while (entityIt != getEntities().end())
		{
			if (getEntityManager()->hasComponent<std::string>(*entityIt)) 
			{
				if (ImGui::CollapsingHeader(getEntityManager()->getComponent<std::string>(*entityIt).c_str()))
				{
					// Draw components
					// Transform component
					if (getEntityManager()->hasComponent<CmpTransform>(*entityIt)) 
					{
						if (ImGui::CollapsingHeader("Transform")) 
						{
							CmpTransform& comp = getEntityManager()->getComponent<CmpTransform>(*entityIt);
							glm::vec3 pos, rot, scale;

							ImGuizmo::DecomposeMatrixToComponents(&comp.matrix[0][0], &pos.x, &rot.x, &scale.x);
							ImGui::DragFloat3("Position", &pos.x, 0.1f);
							ImGui::DragFloat3("Rotation", &rot.x, 0.1f);
							ImGui::DragFloat3("Scale", &scale.x, 0.1f, 0.f);
							ImGuizmo::RecomposeMatrixFromComponents(&pos.x, &rot.x, &scale.x, &comp.matrix[0][0]);
						}
					}
					// Int component
					if (getEntityManager()->hasComponent<int>(*entityIt)) {
						ImGui::InputInt("input int", &getEntityManager()->getComponent<int>(*entityIt));
					}
					// Float component
					if (getEntityManager()->hasComponent<float>(*entityIt)) {
						ImGui::InputFloat("input float", &getEntityManager()->getComponent<float>(*entityIt));
					}
				}
			}
			entityIt++;
		}
		ImGui::End();
	}
}