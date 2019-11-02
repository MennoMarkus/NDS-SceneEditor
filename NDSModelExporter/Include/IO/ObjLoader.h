#pragma once
#include <string>
#include "Settings.h"
#include "Model.h"

class ObjLoader
{
public:
	static bool load(const Settings& settings, std::vector<Model>& o_models);
};