#pragma once
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

		// Can append to an excisting o_packedCommands list.
		void pack(MeshType type, const Indices& indices, const Vertices& vertices, std::vector<PackedFIFOCommand>& o_packedCommands);

	private:
		void addCommands(MeshType type, const Indices& indices, const Vertices& vertices, std::vector<PackedFIFOCommand>& o_packedCommands, int& io_lastCommandIdx);
		void moveToNextCommand(int& o_currentCommandID, PackedFIFOCommand& o_currentPackedCommand, std::vector<PackedFIFOCommand>& o_packedCommands);
	};
}