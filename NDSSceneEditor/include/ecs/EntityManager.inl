#pragma once
#include "EntityManager.h"
#include "ecs/SystemBase.h"

namespace nds_se
{
	template<typename T>
	inline void EntityManager::registerComponent(size_t reserveSize)
	{
		// Don't register this component again if it's already registered
		if (m_componentTypes.find(typeid(T).hash_code()) != m_componentTypes.end())
			return;

		m_componentTypes.insert(std::make_pair(typeid(T).hash_code(), m_currentComponentTypeID));
		m_componentArrays.insert(std::make_pair(typeid(T).hash_code(), std::make_unique<ComponentDataArray<T>>(reserveSize)));

		m_currentComponentTypeID++;
	}

	template<typename T, typename ...Args>
	inline void EntityManager::registerSystem(Args&& ...args)
	{
		std::unique_ptr<ISystemBase> system(new T(std::forward<Args>(args)...));

		// Don't register this system again if it's already registered
		for (auto& registeredSystem : m_systems)
		{
			assert(registeredSystem->m_name != system->m_name && "System has already been registered.");
		}

		system->m_entityManager = this;
		system->registerComponents();
		m_systems.emplace_back(std::move(system));
	}

	inline void EntityManager::registerSystem(std::unique_ptr<ISystemBase> system)
	{
		// Don't register this system again if it's already registered
		for (auto& registeredSystem : m_systems)
		{
			assert(registeredSystem->m_name != system->m_name && "System has already been registered.");
		}

		system->m_entityManager = this;
		system->registerComponents();
		m_systems.emplace_back(std::move(system));
	}

	template<typename T>
	inline void EntityManager::reserveComponentCount(size_t reserveSize)
	{
		getComponentDataArray<T>().reserve(reserveSize);
	}

	template<typename T>
	inline ComponentType EntityManager::getComponentType() const
	{
		assert(m_componentTypes.find(typeid(T).hash_code()) != m_componentTypes.end() && "Component type does not excist.");
		return m_componentTypes.at(typeid(T).hash_code());
	}

	template<typename T>
	inline T& EntityManager::getComponent(Entity entity)
	{
		return getComponentDataArray<T>().get(entity);
	}

	template<typename T>
	inline bool EntityManager::hasComponent(Entity entity) const
	{
		EntityArchetype archetype = getArchetype(entity);
		return archetype.test(getComponentType<T>());
	}

	template<typename T>
	inline void EntityManager::setComponent(Entity entity, T componentData)
	{
		// Set component data
		getComponentDataArray<T>().insert(entity, componentData);

		// Update the archetype bit field
		EntityArchetype archetype = getArchetype(entity);
		archetype.set(getComponentType<T>(), true);
		setArchetype(entity, archetype);
	}

	template<typename T>
	inline void EntityManager::addComponent(Entity entity)
	{
		// Add component data
		getComponentDataArray<T>().insert(entity);

		// Update the archetype bit field
		EntityArchetype archetype = getArchetype(entity);
		archetype.set(getComponentType<T>(), true);
		setArchetype(entity, archetype);
	}

	template<typename T>
	inline void EntityManager::removeComponent(Entity entity)
	{
		// Remove component data
		getComponentDataArray<T>().erase(entity);

		// Update the archetype bit field
		EntityArchetype archetype = getArchetype(entity);
		archetype.set(getComponentType<T>(), false);
		setArchetype(entity, archetype);
	}

	template<typename T>
	inline ComponentDataArray<T>& EntityManager::getComponentDataArray()
	{
		assert(m_componentTypes.find(typeid(T).hash_code()) != m_componentTypes.end() && "Component type does not excist.");
		return *static_cast<ComponentDataArray<T>*>(m_componentArrays[typeid(T).hash_code()].get());
	}
}