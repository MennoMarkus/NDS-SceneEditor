#pragma once
#include <set>
#include <functional>
#include "Entity.h"
#include "services/ServiceLocator.h"

namespace nds_se
{
	class EntityManager;

	// Needed as a base class pointer type as we can't have differently templated classes in the same container.
	class ISystemBase
	{
		friend class EntityManager;
		template <class... Ts> friend class SystemBase;

	private:
		std::string m_name;
		EntityArchetype m_archetype = 0; // The archetype this system matches against.
		EntityManager* m_entityManager = nullptr; // Cached pointer to the entity manager.

		// All entities this system runs on. Entities are sorted for better cache access when accsssing multiple componets at once.
		std::set<Entity> m_entities;

	public:
		ISystemBase() = default;
		virtual void update() = 0;

	protected:
		inline const std::string& getName() const { return m_name; }
		inline EntityArchetype getEntityArcheType() const { return m_archetype; }
		inline EntityManager* getEntityManager() { return m_entityManager; }
		inline const std::set<Entity>& getEntities() const { return m_entities; }

	private:
		virtual void registerComponents() = 0;
	};
	//

	template <class... Ts>
	class SystemBase : public ISystemBase
	{
	private:
		// Function pointer to allow for lambda expressions.
		typedef std::function<void(Entity, Ts&...)> UpdateFunctionPtr;
		UpdateFunctionPtr m_updateFunction = nullptr;

	public:
		SystemBase(const std::string& name);
		SystemBase(const std::string& name, UpdateFunctionPtr updateFunction);

		// Override if you want to manually get the components for each entity in this system.
		virtual void update() override;
		// Override if you want to run an operation on each entity that matches this system.
		virtual void foreach(Entity entity, Ts&... ts);

		EntityManager* getEntityManager() const;
		const std::string& getName() const;
		EntityArchetype getArchetype() const;

	private:
		void registerComponents() override;
	};
}

#include "SystemBase.inl"