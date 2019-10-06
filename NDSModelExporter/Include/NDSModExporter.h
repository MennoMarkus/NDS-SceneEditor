#pragma once
#include <string>
#include "Stripify/Primative.h"

struct PackedFIFOCommand
{
	enum class FIFO_COMMAND
	{
		NOP = 0x00,
		COLOR = 0x20,
		NORMAL = 0x21,
		TEXCOORD = 0x22,
		VTX16 = 0x23,
		VTXS = 0x40
	};
	FIFO_COMMAND commands[4];
	std::vector<uint32_t> commandParameters[4];

	void setCommand(int commandId, FIFO_COMMAND command, const std::vector<uint32_t> parameters) { commands[commandId] = command; commandParameters[commandId] = parameters; }
	void setNOPCommand(int commandId);
	void setColorCommand(int commandId, float r, float g, float b);
	void setNormalCommand(int commandId, float x, float y, float z);
	void setVTX16Command(int commandId, float x, float y, float z);
	void setVTXSCommand(int commandId, PrimativeType type);
};

class NDSModExporter
{
public:
	static bool write(const std::string& filePath, const std::vector<Primative>& primatives);

private:
	static void getCommandsFromPrimative(const Primative& primative, std::vector<PackedFIFOCommand>& o_commandList, int& io_lastCommandIdx);
};