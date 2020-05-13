#pragma once

#include "services/resourceAllocator/ResourceAllocator.h"
#include "rendering/Model.h"

namespace nds_se
{
	struct CmpModel
	{
		ResourceID<Model> model;
	};
}