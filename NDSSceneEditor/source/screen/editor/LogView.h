#pragma once
#include "imgui/imgui.h"

namespace nds_se
{
	class LogView
	{
	private:
		ImGuiTextBuffer m_buffer;
		ImGuiTextFilter m_filter;
		ImVector<int> m_lineOffsets; 
		bool m_autoScroll = true;

	public:
		LogView();
		void render();

		void log(const std::string& message);
		void clear();
	};
}