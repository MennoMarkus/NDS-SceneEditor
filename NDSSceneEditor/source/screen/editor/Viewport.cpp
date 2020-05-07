#include "screen/editor/Viewport.h"

#include "imgui/imgui.h"

namespace nds_se
{
	Viewport::Viewport()
	{
		// Enable docking
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	}

	void Viewport::render()
	{

		// Create a viewport window where other windows can dock into.
		// The viewport window bar functions as as toolbar.
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->GetWorkPos());
		ImGui::SetNextWindowSize(viewport->GetWorkSize());
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar |
									   ImGuiWindowFlags_NoDocking | // Toolbar cannot be docked
									   ImGuiWindowFlags_NoBackground | // Viewport should be transparent
									   ImGuiWindowFlags_NoTitleBar |
									   ImGuiWindowFlags_NoCollapse |
									   ImGuiWindowFlags_NoResize |
									   ImGuiWindowFlags_NoMove |
									   ImGuiWindowFlags_NoBringToFrontOnFocus |
									   ImGuiWindowFlags_NoNavFocus;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", nullptr, windowFlags);
		{
			ImGui::PopStyleVar(3);

			// Create dockspace
			ImGuiID dockspaceID = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

			// Create toolbar
			if (ImGui::BeginMenuBar())
			{
				// Create file menu
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("Test")) 
					{

					}
					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}
		}
		ImGui::End();
	}
}