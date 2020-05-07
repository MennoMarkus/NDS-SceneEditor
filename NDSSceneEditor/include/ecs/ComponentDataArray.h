#pragma once
#include <cassert>
#include <vector>
#include <algorithm>
#include "Entity.h"

namespace nds_se
{
	// Needed as a base class pointer type as we can't have differently templated classes in the same container.
	class IComponentDataArray
	{
	public:
		virtual void erase(Entity entity) = 0;
	};
	//

	template<typename T>
	class ComponentDataArray : public IComponentDataArray
	{
	private:
		// We'll never have more than MAX_ENTITY_ID indices as every entity can only have 1 component.
		// Therefor we can reserve MAX_ENTITY_ID + 1 as an invalid index.
		const size_t INVALID_INDEX = MAX_ENTITY_ID + 1; 

		size_t m_size = 0;
		// Components are stored in a vector instead of array to minimize memory waste. This results in potentially slower insertion.
		std::vector<T> m_components; 

		// Map entities to component indicies and back.
		std::vector<Entity> m_indexToEntity;
		std::vector<size_t> m_entityToIndex;

	public:
		ComponentDataArray(size_t size);

		T& get(Entity entity);
		size_t size() const;

		void insert(Entity entity, T componentData);
		void insert(Entity entity);
		void erase(Entity entity) override;
		void reserve(size_t count);
	};

	/*
	 * Implementation details
	 */

	template<typename T>
	inline ComponentDataArray<T>::ComponentDataArray(size_t size)
	{
		m_components.reserve(std::min(size, (size_t)MAX_ENTITY_ID));
		m_indexToEntity.reserve(std::min(size, (size_t)MAX_ENTITY_ID));
		m_entityToIndex.reserve(std::min(size, (size_t)MAX_ENTITY_ID));

		// Set all entries to invalid.
		std::fill(std::begin(m_indexToEntity), std::end(m_indexToEntity), INVALID_ENTITY);
		std::fill(std::begin(m_entityToIndex), std::end(m_entityToIndex), INVALID_INDEX);
	}

	template<typename T>
	inline T& ComponentDataArray<T>::get(Entity entity)
	{
		assert(m_entityToIndex[entity] != INVALID_INDEX && "Component data array does not contain entity.");
		return m_components[m_entityToIndex[entity]];
	}

	template<typename T>
	inline size_t ComponentDataArray<T>::size() const
	{
		return (size_t)m_size;
	}

	template<typename T>
	inline void ComponentDataArray<T>::insert(Entity entity, T componentData)
	{
		// Alloacte new memory if needed
		if (m_entityToIndex.size() <= entity) 
			m_entityToIndex.resize(entity + 1, (Entity)INVALID_INDEX);

		if (m_indexToEntity.size() <= m_size)
		{
			m_indexToEntity.resize(m_size + 1, INVALID_ENTITY);
			m_components.resize(m_size + 1);
		}

		assert(m_entityToIndex[entity] == INVALID_INDEX && "Component data array already contains entity.");

		// Insert component
		m_entityToIndex[entity] = m_size;
		m_indexToEntity[m_size] = entity;
		m_components[m_size] = componentData;
		m_size++;
	}

	template<typename T>
	inline void ComponentDataArray<T>::insert(Entity entity)
	{
		// Alloacte new memory if needed
		if (m_entityToIndex.size() <= entity)
			m_entityToIndex.resize(entity + 1, INVALID_INDEX);

		if (m_indexToEntity.size() <= m_size)
		{
			m_indexToEntity.resize(m_size + 1, INVALID_ENTITY);
			m_components.resize(m_size + 1);
		}

		assert(m_entityToIndex[entity] == INVALID_INDEX && "Component data array already contains entity.");

		// Insert component
		m_entityToIndex[entity] = m_size;
		m_indexToEntity[m_size] = entity;
		m_size++;
	}

	template<typename T>
	inline void ComponentDataArray<T>::erase(Entity entity)
	{
		assert(m_entityToIndex[entity] != INVALID_INDEX && "Component data array does not contain entity.");

		// Swap the component data of the removed entity and the last component data element to keep a packed array.
		size_t removedEntityIndex = m_entityToIndex[entity];
		m_components[removedEntityIndex] = m_components[m_size - 1];

		// Update the last component data elements entity to point to it's new index
		Entity entityOfLastIndex = m_indexToEntity[m_size - 1];
		m_entityToIndex[entityOfLastIndex] = removedEntityIndex;
		m_indexToEntity[removedEntityIndex] = entityOfLastIndex;

		// Reset removed entity entries
		m_entityToIndex[entity] = (Entity)INVALID_INDEX;
		m_indexToEntity[m_size - 1] = INVALID_ENTITY;

		m_size--;
	}

	template<typename T>
	inline void ComponentDataArray<T>::reserve(size_t count)
	{
		// Allocate the minimum amount of memory to store "count" entities.
		// m_entityToIndex might require more memory at time of insertion.
		if (m_entityToIndex.size() <= count)
			m_entityToIndex.resize(count + 1, INVALID_INDEX);

		if (m_indexToEntity.size() <= count)
		{
			m_indexToEntity.resize(count + 1, INVALID_ENTITY);
			m_components.resize(count + 1);
		}
	}
}