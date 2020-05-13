#pragma once
#include "rendering/Primative.h"
#include "rendering/Vertex.h"
#include "ndsCommandPacker/PackedFIFOCommand.h"

namespace nds_se
{
	class NDSCommandPacker
	{
	private:
		bool m_useDebugColor = false;
		bool m_useNormals = false;
		bool m_useLowVertexPrecision = false;

	public:
		NDSCommandPacker() = default;

		void pack(const std::vector<Primative>& primatives, const std::vector<Vertex>& vertices, std::vector<PackedFIFOCommand>& o_packedCommands);

	private:
		void getCommandsFromPrimative(const Primative& primative, const std::vector<Vertex>& vertices, std::vector<PackedFIFOCommand>& o_packedCommands, int& io_lastCommandIdx);
		void moveToNextCommand(int& o_currentCommandID, PackedFIFOCommand& o_currentPackedCommand, std::vector<PackedFIFOCommand>& o_packedCommands);
	};
}