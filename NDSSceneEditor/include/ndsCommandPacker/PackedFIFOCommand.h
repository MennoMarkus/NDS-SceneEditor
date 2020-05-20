#pragma once
#include <vector>
#include "glm/glm.hpp"
#include "rendering/Mesh.h"

namespace nds_se
{
	enum class PolygonAttrFormat : uint32_t
	{
		POLY_FORMAT_LIGHT0 = (1 << 0),  // enable light number 0
		POLY_FORMAT_LIGHT1 = (1 << 1),  // enable light number 1
		POLY_FORMAT_LIGHT2 = (1 << 2),  // enable light number 2
		POLY_FORMAT_LIGHT3 = (1 << 3),  // enable light number 3
		POLY_MODULATION = (0 << 4),     // enable modulation shading mode; this is the default
		POLY_DECAL = (1 << 4),          // enable decal shading 
		POLY_TOON_HIGHLIGHT = (2 << 4), // enable toon/highlight shading mode
		POLY_SHADOW = (3 << 4),         // enable shadow shading 
		POLY_CULL_FRONT = (1 << 6),     // cull front polygons
		POLY_CULL_BACK = (2 << 6),      // cull rear polygons 
		POLY_CULL_NONE = (3 << 6),      // don't cull any polygons
		POLY_FOG = (1 << 15)            // enable/disable fog for this polygon
	};

	// NDS rendering FIFO commands are packed into sets of 4 commands followed by a variable amount of parameters for each command.
	struct PackedFIFOCommand
	{
		enum class FIFOCommand
		{
			NOP = 0x00,					// No operation (used for padding)
			COLOR = 0x20,				// Set vertex color
			NORMAL = 0x21,				// Set vertex normal
			TEXCOORD = 0x22,			// Set vertex texture coordinate
			VTX16 = 0x23,				// Set vertex position (16 bit component)
			VTX10 = 0x24,				// Set vertex position (10 bit component)
			POLYGON_ATTR = 0x29,		// Set polygon attributes
			BEGIN_VTXS = 0x40			// Begin new vertex list
		};
		FIFOCommand m_commands[4];
		std::vector<uint32_t> m_commandParameters[4];

		void setCommand(int commandId, FIFOCommand command, const std::vector<uint32_t> parameters);
		void setNOPCommand(int commandId);
		void setCOLORCommand(int commandId, glm::vec3 color);
		void setNORMALCommand(int commandId, glm::vec3 normal);
		void setVTX16Command(int commandId, glm::vec3 position);
		void setVTX10Command(int commandId, glm::vec3 position);
		void setPOLYGON_ATTRCommand(int commandId, PolygonAttrFormat format, unsigned int alpha, unsigned int polygonId);
		void setBEGIN_VTXSCommand(int commandId, MeshType type);
	};
}