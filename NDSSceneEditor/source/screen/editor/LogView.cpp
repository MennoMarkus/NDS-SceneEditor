#include "screen/editor/LogView.h"

namespace nds_se
{
	LogView::LogView()
	{
		clear();
	}

	void LogView::render()
	{
		// Start log window
		if (!ImGui::Begin("Log"))
		{
			ImGui::End();
			return;
		}
		{
			// Main window topbar
			ImGui::Checkbox("Auto scroll", &m_autoScroll);

			ImGui::SameLine();
			bool shouldJumpToBottom = ImGui::Button("Jump To Bottom");

			ImGui::SameLine();
			bool shouldClear = ImGui::Button("Clear");

			ImGui::SameLine();
			bool shouldCopy = ImGui::Button("Copy");

			ImGui::SameLine();
			m_filter.Draw("Filter", -100.0f);

			ImGui::Separator();
			ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

			if (shouldClear)
				clear();

			if (shouldCopy)
				ImGui::LogToClipboard();

			// Main window console
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
			const char* bufferStart = m_buffer.begin();
			const char* bufferEnd = m_buffer.end();

			// Print console text with filter
			if (m_filter.IsActive())
			{
				for (int lineNumber = 0; lineNumber < m_lineOffsets.Size; lineNumber++)
				{
					// Get line start and end
					const char* lineStart = bufferStart + m_lineOffsets[lineNumber];
					const char* lineEnd;
					if (lineNumber + 1 < m_lineOffsets.Size)
						lineEnd = (bufferStart + m_lineOffsets[lineNumber + 1] - 1);
					else
						lineEnd = bufferEnd;

					// Check against filter 
					if (m_filter.PassFilter(lineStart, lineEnd))
						ImGui::TextUnformatted(lineStart, lineEnd);
				}
			}
			// Print only items within the console viewport
			else
			{
				ImGuiListClipper clipper;
				clipper.Begin(m_lineOffsets.Size);
				while (clipper.Step())
				{
					for (int lineNumber = clipper.DisplayStart; lineNumber < clipper.DisplayEnd; lineNumber++)
					{
						// Get line start and end
						const char* lineStart = bufferStart + m_lineOffsets[lineNumber];
						const char* lineEnd;
						if (lineNumber + 1 < m_lineOffsets.Size)
							lineEnd = (bufferStart + m_lineOffsets[lineNumber + 1] - 1);
						else
							lineEnd = bufferEnd;

						// Print line to the console
						ImGui::TextUnformatted(lineStart, lineEnd);
					}
				}
				clipper.End();
			}
			ImGui::PopStyleVar();

			if ((m_autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) || shouldJumpToBottom)
				ImGui::SetScrollHereY(1.0f);

			ImGui::EndChild();
		}
		ImGui::End();
	}

	void LogView::log(const std::string& message)
	{
		// Add new message to the buffer
		int oldSize = m_buffer.size();

		m_buffer.append(message.c_str());

		for (int newSize = m_buffer.size(); oldSize < newSize; oldSize++)
		{
			if (m_buffer[oldSize] == '\n')
				m_lineOffsets.push_back(oldSize + 1);
		}
	}

	void LogView::clear()
	{
		m_buffer.clear();
		m_lineOffsets.clear();
		m_lineOffsets.push_back(0);
	}
}