#pragma once

#include <unordered_map>
#include <typeinfo>

#include "Service.h"

namespace nds_se
{
	class ServiceLocator
	{
	private:
		static ServiceLocator m_instance;
		std::unordered_map<size_t, Service* > m_services;

	public:
		static ServiceLocator& get() { return m_instance; }

		template<typename T>
		static T* get() { return ServiceLocator::get().getService<T>(); }

		template< class a_service >
		a_service* getService() { return static_cast<a_service*>(m_services[typeid(a_service).hash_code()]); };

		template< class a_service >
		void provideService(Service* service_instance) { m_services[typeid(a_service).hash_code()] = (a_service*)service_instance; };
	};
}