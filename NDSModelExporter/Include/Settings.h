#pragma once
#include <string>

struct Settings
{
	std::string inputFile;
	std::string outputFile;
	bool printDebugInfo = true;
	bool highPrecision = false;
	bool includeNormals = true;
};