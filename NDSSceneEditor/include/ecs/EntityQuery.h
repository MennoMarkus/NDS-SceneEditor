#pragma once
#include <set>
#include "ecs/Entity.h"

namespace nds_se
{
	struct EntityQuery
	{
		// The archetype this query matches against.
		EntityArchetype m_archetype = 0;

		// All queried entities. Entities are sorted for better cache access when accsssing multiple components at once.
		std::set<Entity> m_entities;
	};
}