#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include <typeinfo>

#include "services/Service.h"
#include "services/Logger.h"

#include "services/resourceAllocator/ResourceID.h"

namespace nds_se
{
	template<typename RESOURCE>
	class ResourceAllocator : public Service
	{
	private:
		ResourceID<RESOURCE> m_currentID;
		std::unordered_map<ResourceID<RESOURCE>, std::unique_ptr<RESOURCE>> m_resources;

	public:
		ResourceAllocator<RESOURCE>() = default;
		ResourceAllocator<RESOURCE>& operator=(const ResourceAllocator<RESOURCE>&) = delete;
		ResourceAllocator<RESOURCE>(const ResourceAllocator<RESOURCE>&) = delete;

		ResourceID<RESOURCE> addResource(std::unique_ptr<RESOURCE> resourcePtr);

		template<typename ... Args>
		ResourceID<RESOURCE> loadResource(Args&& ... args);

		template<typename ... Args>
		RESOURCE& getOrLoadResource(ResourceID<RESOURCE>* o_ID, Args&& ... args);

		RESOURCE& getResource(ResourceID<RESOURCE> ID);

		void unloadResource(ResourceID<RESOURCE> ID);

		const std::unordered_map<ResourceID<RESOURCE>, std::unique_ptr<RESOURCE>>& getResources() const;
	};

	/*
	 * Implementation details
	 */

	template<typename RESOURCE>
	inline ResourceID<RESOURCE> ResourceAllocator<RESOURCE>::addResource(std::unique_ptr<RESOURCE> resourcePtr)
	{
		// Check if the resource hasn't been loaded yet.
		for (auto& resource : m_resources)
		{
			if (resource.second->equalsResource(*resourcePtr))
			{
				LOG(LOG_ERROR, "ResourceAllocator<" << typeid(RESOURCE).name() << "> tried to add a resource that has already been loaded.");
				return resource.first;
			}
		}

		// Add new resource
		m_currentID.m_ID++;
		auto it = m_resources.emplace(m_currentID, std::move(resourcePtr));

		return m_currentID;
	}

	template<typename RESOURCE>
	template<typename ...Args>
	inline ResourceID<RESOURCE> ResourceAllocator<RESOURCE>::loadResource(Args && ...args)
	{
		std::unique_ptr<RESOURCE> resourcePtr(new RESOURCE(std::forward<Args>(args)...));

		// Check if the resource hasn't been loaded yet.
		for (auto& resource : m_resources)
		{
			if (resource.second->equalsResource(*resourcePtr))
			{
				LOG(LOG_ERROR, "ResourceAllocator<" << typeid(RESOURCE).name() << "> tried to load a resource that has already been loaded.");
				return resource.first;
			}
		}

		// Load resource
		resourcePtr->load();

		// Add new resource
		m_currentID.m_ID++;
		auto it = m_resources.emplace(m_currentID, std::move(resourcePtr));
		
		return m_currentID;
	}

	template<typename RESOURCE>
	template<typename ...Args>
	inline RESOURCE& ResourceAllocator<RESOURCE>::getOrLoadResource(ResourceID<RESOURCE>* o_ID, Args&& ...args)
	{
		std::unique_ptr<RESOURCE> resourcePtr(new RESOURCE(std::forward<Args>(args)...));

		// Try to find the resource to return
		for (auto& resource : m_resources)
		{
			if (resource.second->equalsResource(*resourcePtr))
			{
				if (o_ID != nullptr)
					*o_ID = resource.first;
				return *resource.second;
			}
		}

		// Load resource
		resourcePtr->load();

		// Add new resource
		m_currentID.m_ID++;
		auto it = m_resources.emplace(m_currentID, std::move(resourcePtr));

		if (o_ID != nullptr)
			*o_ID = m_currentID;

		return *it.first->second;
	}

	template<typename RESOURCE>
	inline RESOURCE& ResourceAllocator<RESOURCE>::getResource(ResourceID<RESOURCE> ID)
	{
		return *m_resources.at(ID);
	}

	template<typename RESOURCE>
	inline void ResourceAllocator<RESOURCE>::unloadResource(ResourceID<RESOURCE> ID)
	{
		// Remove the resource with a matching ID, else do nothing
		auto it = m_resources.find(ID);
		if (it != m_resources.end())
			m_resources.erase(it);
	}

	template<typename RESOURCE>
	inline const std::unordered_map<ResourceID<RESOURCE>, std::unique_ptr<RESOURCE>>& ResourceAllocator<RESOURCE>::getResources() const
	{
		return m_resources;
	}
}