#include <iostream>
#include <algorithm>
#include "Settings.h"
#include "IO/ObjLoader.h"
#include "Stripify/Stripify.h"
#include "IO/NDSModExporter.h"

Settings settings;

static void showUsageMessage()
{
	std::cout << 
		"-----Nintendo DS Model exporter-----\n" <<
		"A tool developed to convert obj models to binary Nintendo DS commands.\n"
		"Github: https://github.com/MennoMarkus/NDS-Model-Exporter \n" <<
		"Usage:\n" <<
		"\t-h,--help\t\t\tShow this help message.\n" <<
		"\t-o,--output OUTPUT_FILE\t\tSpecify the ouptut file path.\n" <<
		"\t-v,--verbose \t\t\tPrint debug information.\n" <<
		"\t--debugColorPrimatives\t\tAssign a debug color to each primative. This overrides the model color data.\n" <<
		"\t--highPrecision\t\t\tUse the higher 12 bit fractional precision for vertices instead of 6 bit, at the cost of a larger output file.\n" <<
		"\t--excludeNormals\t\tExclude normals from the output file.\n" <<
		"\t--mergeModels\t\t\tMerge all models into one output file.\n" <<
		std::endl;
}

// Process the command line arguments input. Returns whether the input was valid.
static bool getInput(int argc, char* argv[])
{
	if (argc < 2)
	{
		showUsageMessage();
		return false;
	}
	else
	{
		for (int i = 0; i < argc; i++)
		{
			std::string arg = argv[i];
			if (arg == "-h" || arg == "--help")
			{
				showUsageMessage();
				return false;
			}
			else if (arg == "-o" || arg == "--output")
			{
				if (i + 1 < argc)
					settings.outputFile = argv[++i];
				else
				{
					std::cerr << "-o,--output requires one argument.\n";
					return false;
				}
			}
			else if (arg == "-v" || arg == "--verbose")
			{
				settings.printDebugInfo = true;
			}
			else if (arg == "--debugColorPrimatives")
			{
				settings.debugColorPrimatives = true;
			}
			else if (arg == "--highPrecision")
			{
				settings.highPrecision = true;
			}
			else if (arg == "--excludeNormals")
			{
				settings.includeNormals = false;
			}
			else if (arg == "--mergeModels")
			{
				settings.mergeModels = true;
			}
			else
			{
				settings.inputFile = arg;
			}
		}
	}

	return true;
}

int main(int argc, char* argv[])
{

	// Process input 
	bool succes = getInput(argc, argv);
	if (!succes || settings.inputFile.empty())
	{
		settings.inputFile = "B:/CppProjects/NDSModelExporter/Cube.obj";
		// TODO debug return 1;
	}

	// Get ouptut file if none is specified
	if (settings.outputFile.empty())
	{
		size_t lastindex = settings.inputFile.find_last_of(".");
		settings.outputFile = settings.inputFile.substr(0, lastindex) + ".ndsmod";
	}


	   	 
	// Load model
	std::vector<Model> models;
	if (!ObjLoader::load(settings, models))
		exit(1);

	if (settings.mergeModels)
	{
		// Merge models into one
		Model mergedModel;
		for (auto& model : models)
		{
			mergedModel.vertexBuffer.reserve(mergedModel.vertexBuffer.size() + model.vertexBuffer.size());
			mergedModel.vertexBuffer.insert(mergedModel.vertexBuffer.end(), model.vertexBuffer.begin(), model.vertexBuffer.end());

			mergedModel.minBound[0] = std::min(mergedModel.minBound[0], model.minBound[0]);
			mergedModel.minBound[1] = std::min(mergedModel.minBound[1], model.minBound[1]);
			mergedModel.minBound[2] = std::min(mergedModel.minBound[2], model.minBound[2]);

			mergedModel.maxBound[0] = std::max(mergedModel.maxBound[0], model.maxBound[0]);
			mergedModel.maxBound[1] = std::max(mergedModel.maxBound[1], model.maxBound[1]);
			mergedModel.maxBound[2] = std::max(mergedModel.maxBound[2], model.maxBound[2]);
		}

		models.clear();
		models.push_back(mergedModel);
	}

	for (auto& model : models)
	{
		if (settings.printDebugInfo)
			std::cout << "\n-----Stripfication:-----\n";

		// Create triangle strips
		std::vector<Primative> primatives = Stripify::Strip(model.vertexBuffer);

		// Print debug info
		if (settings.printDebugInfo)
		{
			int totalVertexCount = 0;
			std::cout << model.vertexBuffer.size() / 3 << " initial triangles contaning " << model.vertexBuffer.size() << " vertices.\n";
			std::cout << "Converted into " << primatives.size() << " primatives:\n";
			for (auto& primative : primatives)
			{
				std::cout << "\tType: \t" << (primative.type == PrimativeType::TRIANGLES ? "Triangles" : "Strip    ");
				std::cout << " \tTriangle Count: " << (primative.type == PrimativeType::TRIANGLES ? primative.vertices.size() / 3 : (primative.vertices.size() - 3) + 1);
				std::cout << " \tVertex Count: " << primative.vertices.size() << "\n";
				totalVertexCount += primative.vertices.size();
			}
			std::cout << "New total vertices: " << totalVertexCount << std::endl;
		}

		// Convert primatives into a packed command list
		if (settings.printDebugInfo)
			std::cout << "\n-----Command packer:-----\n";
		std::vector<PackedFIFOCommand> commandList = NDSModExporter::getCommandList(settings, primatives);

		// Output command list to file
		Settings modelSettings = settings;
		if (models.size() > 1)
		{
			size_t lastindex = modelSettings.outputFile.find_last_of(".");
			modelSettings.outputFile.insert(lastindex, "_" + model.name);
		}
		NDSModExporter::write(modelSettings, commandList);
	}

	return 0;
}