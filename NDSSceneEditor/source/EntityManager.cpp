#include "ecs/EntityManager.h"

#include <cassert>

namespace nds_se
{
	EntityManager::EntityManager()
	{
		std::fill(std::begin(m_entityArchetypes), std::end(m_entityArchetypes), 0);
		std::fill(std::begin(m_avialableEntityIDs), std::end(m_avialableEntityIDs), true);
		m_avialableEntityIDs[0] = false;
	}

	void EntityManager::update()
	{
		for (auto& system : m_systems)
			system->update();
	}

	Entity EntityManager::createEntity()
	{
		m_activeEntityCount++;
		Entity entity = getFirstAvialableEntityID();

		// Set the new entity ID to unavialable.
		m_avialableEntityIDs[entity] = false;

		// Add entity for systems that run on all entities
		for (auto& system : m_systems)
		{
			EntityArchetype systemArcheType = system->m_archetype;

			// Systems only run on entities with a matching archetype.
			if (systemArcheType == 0)
				system->m_entities.insert(entity);
		}

		return entity;
	}

	void EntityManager::destroyEntity(Entity entity)
	{
		assert(m_avialableEntityIDs[entity] == false && "Entity is not avialable.");

		// Remove components
		for (auto& componentDataArray : m_componentArrays)
		{
			ComponentType componentType = m_componentTypes[componentDataArray.first];
			EntityArchetype archetype = getArchetype(entity);

			// Check if this entity has this component
			if (archetype.test(componentType) != false)
				componentDataArray.second->erase(entity);
		}

		// Remove entity for systems
		for (auto& system : m_systems)
		{
			system->m_entities.erase(entity);
		}

		// Reset archetype to no components
		m_entityArchetypes[entity] = 0;

		// Set the entity ID to avialable.
		m_avialableEntityIDs[entity] = true;

		m_activeEntityCount--;
	}

	EntityArchetype EntityManager::getArchetype(Entity entity) const
	{
		assert(m_avialableEntityIDs[entity] == false && "Entity is not avialable.");
		return m_entityArchetypes[entity];
	}

	void EntityManager::setArchetype(Entity entity, EntityArchetype archetype)
	{
		assert(m_avialableEntityIDs[entity] == false && "Entity is not avialable.");
		m_entityArchetypes[entity] = archetype;

		// Update the systems to add or remove this system
		for (auto& system : m_systems)
		{
			EntityArchetype systemArcheType = system->m_archetype;

			// Systems only run on entities with a matching archetype.
			if ((systemArcheType & archetype) == systemArcheType || systemArcheType == 0)
				system->m_entities.insert(entity);
			else
				system->m_entities.erase(entity);
		}
	}

	Entity EntityManager::getFirstAvialableEntityID() const
	{
		assert(m_activeEntityCount <= MAX_ENTITY_ID && "Out of entity space.");

		// Find the index avialable in the m_avialableEntityIDs
		Entity index = 0;
		while (!m_avialableEntityIDs[index++]);
		index -= 1;

		return index;
	}
}