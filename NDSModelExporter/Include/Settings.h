#pragma once
#include <string>

struct Settings
{
	std::string inputFile;
	std::string outputFile;
	bool printDebugInfo = true;
	bool debugColorPrimatives = false;
	bool highPrecision = false;
	bool includeNormals = true;
	bool mergeModels = false;
};