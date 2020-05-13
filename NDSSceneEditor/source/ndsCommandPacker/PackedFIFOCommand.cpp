#include "ndsCommandPacker/PackedFIFOCommand.h"

namespace nds_se
{
	void PackedFIFOCommand::setCommand(int commandId, FIFOCommand command, const std::vector<uint32_t> parameters)
	{
		m_commands[commandId] = command; m_commandParameters[commandId] = parameters;
	}

	void PackedFIFOCommand::setNOPCommand(int commandId)
	{
		setCommand(commandId, FIFOCommand::NOP, {});
	}

	void PackedFIFOCommand::setCOLORCommand(int commandId, glm::vec3 color)
	{
		uint32_t packedColor = (((uint16_t)(color.r * 255) >> 3) | 
							   (((uint16_t)(color.g * 255) >> 3) << 5) | 
							   (((uint16_t)(color.b * 255) >> 3) << 10));
		setCommand(commandId, FIFOCommand::COLOR, { packedColor });
	}

	void PackedFIFOCommand::setNORMALCommand(int commandId, glm::vec3 normal)
	{
		int16_t normalX = ((normal.x > .998) ? 0x1FF : ((int16_t)((normal.x) * (1 << 9))));
		int16_t normalY = ((normal.y > .998) ? 0x1FF : ((int16_t)((normal.y) * (1 << 9))));
		int16_t normalZ = ((normal.z > .998) ? 0x1FF : ((int16_t)((normal.z) * (1 << 9))));

		uint32_t packedNormal = (uint32_t)(((normalX) & 0x3FF) | 
										  (((normalY) & 0x3FF) << 10) | 
										   ((normalZ) << 20));

		setCommand(commandId, FIFOCommand::NORMAL, { packedNormal });
	}

	void PackedFIFOCommand::setVTX16Command(int commandId, glm::vec3 position)
	{
		uint32_t vertexXY = (((int16_t)((position.y) * (1 << 12)) & 0xFFFF) << 16) |
							 ((int16_t)((position.x) * (1 << 12)) & 0xFFFF);

		uint32_t vertexZW =  ((int16_t)((position.z) * (1 << 12)) & 0xFFFF);

		setCommand(commandId, FIFOCommand::VTX16, { vertexXY, vertexZW });
	}

	void PackedFIFOCommand::setVTX10Command(int commandId, glm::vec3 position)
	{
		uint32_t vertexXYZ = (((int16_t)(position.z * (1 << 6)) & 0x3FF) << 20) | 
							 (((int16_t)(position.y * (1 << 6)) & 0x3FF) << 10) | 
							  ((int16_t)(position.x * (1 << 6)) & 0x3FF);

		setCommand(commandId, FIFOCommand::VTX10, { vertexXYZ });
	}

	void PackedFIFOCommand::setPOLYGON_ATTRCommand(int commandId, PolygonAttrFormat format, unsigned int alpha, unsigned int polygonId)
	{
		setCommand(commandId, FIFOCommand::POLYGON_ATTR, { ((unsigned int)format | (alpha << 16) | (polygonId << 24)) });
	}

	void PackedFIFOCommand::setBEGIN_VTXSCommand(int commandId, PrimativeType type)
	{
		uint32_t typeID = 0;
		switch (type)
		{
			case nds_se::TRIANGLES:			typeID = 0; break;
			case nds_se::TRIANGLE_STRIP:	typeID = 2; break;
		}

		setCommand(commandId, FIFOCommand::BEGIN_VTXS, { typeID });
	}
}