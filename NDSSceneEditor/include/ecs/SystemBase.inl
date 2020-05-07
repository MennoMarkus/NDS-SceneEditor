#pragma once
#include "SystemBase.h"
#include "ecs/EntityManager.h"

namespace nds_se
{
	template<class ...Ts>
	inline SystemBase<Ts...>::SystemBase(const std::string& name)
	{
		m_name = name;
	}

	template<class ...Ts>
	inline SystemBase<Ts...>::SystemBase(const std::string& name, UpdateFunctionPtr updateFunction) :
		m_updateFunction(updateFunction)
	{
		m_name = name;
	}

	template<class ...Ts>
	inline void SystemBase<Ts...>::update()
	{
		assert(m_entityManager != nullptr && "System hasn't been registered with an entity manager.");

		// Update each entity in this system.
		// Update can handle entities within this system being deleted while running, 
		// but can't guarantee that entities in inserted in this system while running will or will not be updated this frame.
		std::set<Entity>::iterator entityIt = m_entities.begin();
		std::set<Entity>::iterator tempIt; // Prevent iterator invalidation on entity removal.
		while (entityIt != m_entities.end())
		{
			tempIt = entityIt;
			++tempIt;
			foreach(*entityIt, m_entityManager->getComponent<Ts>(*entityIt)...);
			entityIt = tempIt;
		}
	}

	template<class ...Ts>
	inline void SystemBase<Ts...>::foreach(Entity entity, Ts&... ts)
	{
		assert(m_updateFunction != nullptr && "System doesn't have an update function or lambda specified.");
		m_updateFunction(entity, ts...);
	}

	template<class ...Ts>
	inline EntityArchetype SystemBase<Ts...>::getArchetype() const
	{
		return m_archetype;
	}

	template<class ...Ts>
	inline EntityManager* SystemBase<Ts...>::getEntityManager() const
	{
		return m_entityManager;
	}

	template<class ...Ts>
	inline const std::string& SystemBase<Ts...>::getName() const
	{
		return m_name;
	}

	template<class ...Ts>
	inline void SystemBase<Ts...>::registerComponents()
	{
		assert(m_entityManager != nullptr && "System hasn't been registered with an entity manager.");

		// Ensures each component is registered in the entity manager.
		(m_entityManager->registerComponent<Ts>(), ...);

		// Create a list containing each template arguments component type.
		auto componentTypes = std::initializer_list<ComponentType>{ m_entityManager->getComponentType<Ts>()... };

		// Construct the archetype by orr-ing together each component type.
		for (auto& componentType : componentTypes)
		{
			m_archetype.set(componentType, true);
		}
	}
}