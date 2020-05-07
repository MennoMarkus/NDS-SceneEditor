#pragma once
#include <cstdint>
#include <limits>
#include <bitset>

namespace nds_se
{
	// ID handle for entities
	using Entity = uint16_t;
	const Entity MAX_ENTITY_ID = 2048;
	const Entity INVALID_ENTITY = 0; // First entity is reserved for debug purposses. Entity 0 is chosen so that INVALID_ENTITY == false.

	// ID handle for components
	using ComponentType = uint8_t;
	const ComponentType MAX_COMPONENT_ID = 64;

	// A bit field indicating which ComponentTypes an entity has
	using EntityArchetype = std::bitset<MAX_COMPONENT_ID>;
}