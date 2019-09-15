#pragma once
#include <string>
#include "Model.h"

class ObjLoader
{
public:
	static bool load(const std::string& file, std::vector<Model>& o_models);
};