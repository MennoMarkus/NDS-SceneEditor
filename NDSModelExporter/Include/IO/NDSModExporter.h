#pragma once
#include <string>
#include "Settings.h"
#include "Stripify/Primative.h"

enum class POLYGON_ATTR_FORMAT : unsigned
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

struct PackedFIFOCommand
{
	enum class FIFO_COMMAND
	{
		NOP = 0x00,
		COLOR = 0x20,
		NORMAL = 0x21,
		TEXCOORD = 0x22,
		VTX16 = 0x23,
		VTX10 = 0x24,
		POLYGON_ATTR = 0x29,
		VTXS = 0x40
	};
	FIFO_COMMAND commands[4];
	std::vector<uint32_t> commandParameters[4];

	void setCommand(int commandId, FIFO_COMMAND command, const std::vector<uint32_t> parameters) { commands[commandId] = command; commandParameters[commandId] = parameters; }
	void setNOPCommand(int commandId);
	void setColorCommand(int commandId, float r, float g, float b);
	void setNormalCommand(int commandId, float x, float y, float z);
	void setVTX16Command(int commandId, float x, float y, float z);
	void setVTX10Command(int commandId, float x, float y, float z);
	void setPOLYGON_ATTRCommand(int commandId, POLYGON_ATTR_FORMAT format, unsigned int alpha, unsigned int polygonId);
	void setVTXSCommand(int commandId, PrimativeType type);
};

class NDSModExporter
{
public:
	// Write a NDS graphics command list to file.
	static bool write(const Settings& settings, const std::vector<PackedFIFOCommand>& commandList);
	// Get the NDS graphics command list from the list of primatives. Command lists should not be merged or edited.
	static std::vector<PackedFIFOCommand> getCommandList(const Settings& settings, const std::vector<Primative>& primatives);

private:
	static void getCommandsFromPrimative(const Settings& settings, const Primative& primative, std::vector<PackedFIFOCommand>& o_commandList, int& io_lastCommandIdx);
};