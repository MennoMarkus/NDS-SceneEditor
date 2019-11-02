#include "IO/NDSModExporter.h"
#include <fstream>
#include <iostream>
#include <cassert>

void PackedFIFOCommand::setNOPCommand(int commandId)
{
	setCommand(commandId, FIFO_COMMAND::NOP, {});
}

void PackedFIFOCommand::setColorCommand(int commandId, float r, float g, float b)
{
	uint32_t packedColor = (((short)(r * 255) >> 3) | (((short)(g * 255) >> 3) << 5) | (((short)(b * 255) >> 3) << 10));
	setCommand(commandId, FIFO_COMMAND::COLOR, { packedColor });
}

void PackedFIFOCommand::setNormalCommand(int commandId, float x, float y, float z)
{
	short normalX = ((x > .998) ? 0x1FF : ((short)((x) * (1 << 9))));
	short normalY = ((y > .998) ? 0x1FF : ((short)((y) * (1 << 9))));
	short normalZ = ((z > .998) ? 0x1FF : ((short)((z) * (1 << 9))));
	uint32_t packedNormal = (uint32_t)(((normalX) & 0x3FF) | (((normalY) & 0x3FF) << 10) | ((normalZ) << 20));
	setCommand(commandId, FIFO_COMMAND::NORMAL, { packedNormal });
}

void PackedFIFOCommand::setVTX16Command(int commandId, float x, float y, float z)
{
	uint32_t vertexXY = (((short)((y) * (1 << 12)) & 0xFFFF) << 16) | ((short)((x) * (1 << 12)) & 0xFFFF);
	uint32_t vertexZW = ((short)((z) * (1 << 12)) & 0xFFFF);
	setCommand(commandId, FIFO_COMMAND::VTX16, { vertexXY, vertexZW });
}

void PackedFIFOCommand::setVTX10Command(int commandId, float x, float y, float z)
{
	uint32_t vertexXYZ = (((short)(z * (1 << 6)) & 0x3FF) << 20) | (((short)(y * (1 << 6)) & 0x3FF) << 10) | ((short)(x * (1 << 6)) & 0x3FF);
	setCommand(commandId, FIFO_COMMAND::VTX10, { vertexXYZ });
}

void PackedFIFOCommand::setVTXSCommand(int commandId, PrimativeType type)
{
	setCommand(commandId, FIFO_COMMAND::VTXS, { (uint32_t)type });
}

bool NDSModExporter::write(const Settings& settings, const std::vector<Primative>& primatives)
{
	std::ofstream file(settings.outputFile, std::ios::binary);
	assert(file);

	std::vector<PackedFIFOCommand> commandList;
	int lastCommandIdx = 0;

	// Pack all commands into one list
	for (auto& primative : primatives)
		getCommandsFromPrimative(settings, primative, commandList, lastCommandIdx);

	// Pad the command list to fill up the last command
	if (commandList.size() > 0 && lastCommandIdx != 3)
	{
		for (int i = lastCommandIdx + 1; i < 4; i++)
			commandList.back().setNOPCommand(i);
	}

	// Write data
	std::vector<uint32_t> buffer;
	for (auto& packedCommand : commandList)
	{
		uint32_t commands = ((uint32_t)packedCommand.commands[3] << 24) | ((uint32_t)packedCommand.commands[2] << 16) |
			((uint32_t)packedCommand.commands[1] << 8) | ((uint32_t)packedCommand.commands[0]);
		buffer.push_back(commands);
		buffer.insert(buffer.end(), packedCommand.commandParameters[0].begin(), packedCommand.commandParameters[0].end());
		buffer.insert(buffer.end(), packedCommand.commandParameters[1].begin(), packedCommand.commandParameters[1].end());
		buffer.insert(buffer.end(), packedCommand.commandParameters[2].begin(), packedCommand.commandParameters[2].end());
		buffer.insert(buffer.end(), packedCommand.commandParameters[3].begin(), packedCommand.commandParameters[3].end());
	}
	buffer.insert(buffer.begin(), buffer.size());
	file.write((char*)& buffer[0], buffer.size() * sizeof(uint32_t));

	file.close();

	if (settings.printDebugInfo)
		std::cout << "Command Count: " << commandList.size() << " Total Size: " << (buffer.size() * sizeof(uint32_t)) / 1000.f << "kb" << std::endl;
	return true;
}

void NDSModExporter::getCommandsFromPrimative(const Settings& settings, const Primative& primative, std::vector<PackedFIFOCommand>& o_commandList, int& io_lastCommandIdx)
{
	PackedFIFOCommand currentPackedCommand = PackedFIFOCommand();
	int currentCommandId = 0;

	// Add to the last packed command if there is still space
	if (o_commandList.size() > 0 && io_lastCommandIdx != 3)
	{
		currentPackedCommand = o_commandList.back();
		currentCommandId = io_lastCommandIdx + 1;
	}

	// Start primate
	currentPackedCommand.setVTXSCommand(currentCommandId, primative.type);
	// Create new command pack if the current is filled
	currentCommandId++;
	if (currentCommandId >= 4)
	{
		o_commandList.push_back(currentPackedCommand);
		currentCommandId = 0;
		currentPackedCommand = PackedFIFOCommand();
	}

	const Vertex* previousVertex = nullptr;
	for (auto& currentVertex : primative.vertices)
	{
		bool materialChanged = false;

		// Set color command
		if (previousVertex == nullptr || previousVertex->diffuseColor[0] != currentVertex.diffuseColor[0] || previousVertex->diffuseColor[1] != currentVertex.diffuseColor[1] || previousVertex->diffuseColor[2] != currentVertex.diffuseColor[2])
		{
			materialChanged = true;
			currentPackedCommand.setColorCommand(currentCommandId, currentVertex.diffuseColor[0], currentVertex.diffuseColor[1], currentVertex.diffuseColor[2]);
			// Create new command pack if the current is filled
			currentCommandId++;
			if (currentCommandId >= 4)
			{
				o_commandList.push_back(currentPackedCommand);
				currentCommandId = 0;
				currentPackedCommand = PackedFIFOCommand();
			}
		}
		// Set normal command
		if (settings.includeNormals)
		{
			if (previousVertex == nullptr || materialChanged ||
				previousVertex->normal[0] != currentVertex.normal[0] || previousVertex->normal[1] != currentVertex.normal[1] || previousVertex->normal[2] != currentVertex.normal[2])
			{
				currentPackedCommand.setNormalCommand(currentCommandId, currentVertex.normal[0], currentVertex.normal[1], currentVertex.normal[2]);
				// Create new command pack if the current is filled
				currentCommandId++;
				if (currentCommandId >= 4)
				{
					o_commandList.push_back(currentPackedCommand);
					currentCommandId = 0;
					currentPackedCommand = PackedFIFOCommand();
				}
			}
		}

		// Set vertex command
		if (settings.highPrecision)
			currentPackedCommand.setVTX16Command(currentCommandId, currentVertex.position[0], currentVertex.position[1], currentVertex.position[2]);
		else
			currentPackedCommand.setVTX10Command(currentCommandId, currentVertex.position[0], currentVertex.position[1], currentVertex.position[2]);

		// Create new command pack if the current is filled
		currentCommandId++;
		if (currentCommandId >= 4)
		{
			o_commandList.push_back(currentPackedCommand);
			currentCommandId = 0;
			currentPackedCommand = PackedFIFOCommand();
		}

		// Set previous to compare to next
		previousVertex = &currentVertex;
	}

	// Add last command pack
	if (currentCommandId != 0)
		o_commandList.push_back(currentPackedCommand);
	io_lastCommandIdx = currentCommandId == 0 ? 3 : currentCommandId - 1;
}
