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

		// Add entity for systems that run on all entities.
		for (auto& system : m_systems)
		{
			EntityArchetype systemArcheType = system->m_archetype;

			// Systems only run on entities with a matching archetype.
			if (systemArcheType == 0)
				system->m_entities.insert(entity);
		}

		// Add entity to queries that match.
		for (auto& query : m_querries)
		{
			if (query.first == 0)
				query.second.insert(entity);
		}

		return entity;
	}

	void EntityManager::destroyEntity(Entity entity)
	{
		assert(m_avialableEntityIDs[entity] == false && "Entity is not avialable.");

		// Remove components.
		for (auto& componentDataArray : m_componentArrays)
		{
			ComponentType componentType = m_componentTypes[componentDataArray.first];
			EntityArchetype archetype = getArchetype(entity);

			// Check if this entity has this component.
			if (archetype.test(componentType) != false)
				componentDataArray.second->erase(entity);
		}

		// Remove entity form systems.
		for (auto& system : m_systems)
		{
			system->m_entities.erase(entity);
		}

		// Remove entity from queries.
		for (auto& query : m_querries)
		{
			query.second.erase(entity);
		}

		// Reset archetype to no components.
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

	const std::set<Entity>& EntityManager::getEntityQuery(EntityArchetype archetype)
	{
		auto it = m_querries.find(archetype);

		// Create a new querry.
		if (it == m_querries.end())
		{
			// Get all entities that match this querry.
			for (Entity entityID = 1; entityID < m_avialableEntityIDs.size(); entityID++)
			{
				// Skip entity IDs that are not in use.
				if (m_avialableEntityIDs[entityID] == true)
					continue;

				EntityArchetype entityArcheType = getArchetype(entityID);
				if ((archetype & entityArcheType) == archetype || archetype == 0)
					m_querries[archetype].insert(entityID);
			}

			return m_querries[archetype];
		}
		// return an excisting query
		else
		{
			return it->second;
		}
	}

	void EntityManager::setArchetype(Entity entity, EntityArchetype archetype)
	{
		assert(m_avialableEntityIDs[entity] == false && "Entity is not avialable.");
		m_entityArchetypes[entity] = archetype;

		// Update the systems to add or remove this entity.
		for (auto& system : m_systems)
		{
			EntityArchetype systemArcheType = system->m_archetype;

			// Systems only run on entities with a matching archetype.
			if ((systemArcheType & archetype) == systemArcheType || systemArcheType == 0)
				system->m_entities.insert(entity);
			else
				system->m_entities.erase(entity);
		}

		// Update the queries to add or remove this entity.
		for (auto& query : m_querries)
		{
			if ((query.first & archetype) == query.first || query.first == 0)
				query.second.insert(entity);
			else
				query.second.erase(entity);
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