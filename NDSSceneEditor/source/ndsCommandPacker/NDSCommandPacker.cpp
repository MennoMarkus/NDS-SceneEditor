#include "ndsCommandPacker/NDSCommandPacker.h"

#include "glm/gtc/random.hpp"

namespace nds_se
{
	void NDSCommandPacker::pack(const std::vector<Primative>& primatives, const std::vector<Vertex>& vertices, std::vector<PackedFIFOCommand>& o_packedCommands)
	{
		int lastCommandIdx = 0;

		// Pack all commands into one list
		for (auto& primative : primatives)
			getCommandsFromPrimative(primative, vertices, o_packedCommands, lastCommandIdx);

		// Pad the command list to fill up the last command
		if (o_packedCommands.size() > 0 && lastCommandIdx != 3)
		{
			for (int i = lastCommandIdx + 1; i < 4; i++)
				o_packedCommands.back().setNOPCommand(i);
		}
	}

	void NDSCommandPacker::getCommandsFromPrimative(const Primative& primative, const std::vector<Vertex>& vertices, std::vector<PackedFIFOCommand>& o_packedCommands, int& io_lastCommandIdx)
	{
		PackedFIFOCommand currentPackedCommand = PackedFIFOCommand();
		int currentCommandID = 0;

		// Generate random color
		glm::vec3 debugColor = glm::linearRand(glm::vec3(0.0f), glm::vec3(1.0));

		// Add to the last packed command if there is still space
		if (o_packedCommands.size() > 0 && io_lastCommandIdx != 3)
		{
			currentPackedCommand = o_packedCommands.back();
			currentCommandID = io_lastCommandIdx + 1;
		}

		// Set primative polygon settings
		currentPackedCommand.setPOLYGON_ATTRCommand(currentCommandID, PolygonAttrFormat::POLY_CULL_BACK, 31, 0);
		moveToNextCommand(currentCommandID, currentPackedCommand, o_packedCommands);

		// Start primate
		currentPackedCommand.setBEGIN_VTXSCommand(currentCommandID, primative.m_type);
		moveToNextCommand(currentCommandID, currentPackedCommand, o_packedCommands);

		const Vertex* previousVertex = nullptr;
		for (int i = 0; i < primative.m_indices.size(); i++)
		{
			const Vertex& currentVertex = vertices[primative.m_indices[i]];
			bool materialChanged = false;

			/*
			 * Order of these commands is important for performance.
			 */

			// Set color command
			if (previousVertex == nullptr || previousVertex->color != currentVertex.color)
			{
				materialChanged = true;
				if (m_useDebugColor)
					currentPackedCommand.setCOLORCommand(currentCommandID, debugColor);
				else
					currentPackedCommand.setCOLORCommand(currentCommandID, (glm::vec3)currentVertex.color);

				moveToNextCommand(currentCommandID, currentPackedCommand, o_packedCommands);
			}

			// Set normal command
			if (m_useNormals)
			{
				if (previousVertex == nullptr || materialChanged || previousVertex->normal != currentVertex.normal)
				{
					currentPackedCommand.setNORMALCommand(currentCommandID, currentVertex.normal);
					moveToNextCommand(currentCommandID, currentPackedCommand, o_packedCommands);
				}
			}

			// Set vertex command
			if (m_useLowVertexPrecision)
				currentPackedCommand.setVTX10Command(currentCommandID, currentVertex.position);
			else
				currentPackedCommand.setVTX16Command(currentCommandID, currentVertex.position);

			moveToNextCommand(currentCommandID, currentPackedCommand, o_packedCommands);

			// Set previous to compare to next
			previousVertex = &currentVertex;
		}

		// Add last command pack
		if (currentCommandID != 0)
			o_packedCommands.push_back(currentPackedCommand);
		io_lastCommandIdx = currentCommandID == 0 ? 3 : currentCommandID - 1;
	}

	void NDSCommandPacker::moveToNextCommand(int& o_currentCommandID, PackedFIFOCommand& o_currentPackedCommand, std::vector<PackedFIFOCommand>& o_packedCommands)
	{
		o_currentCommandID++;

		// Create new command pack if the current is filled
		if (o_currentCommandID >= 4)
		{
			o_packedCommands.push_back(o_currentPackedCommand);
			o_currentCommandID = 0;
			o_currentPackedCommand = PackedFIFOCommand();
		}
	}
}