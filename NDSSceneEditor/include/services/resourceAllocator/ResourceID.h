#pragma once

/*
ResourceID acts as a wrapper around unsigned int.
The purpose of ResourceID is for explicit declaration, private protection and default initialization.
*/

namespace nds_se
{
	template<typename T>
	class ResourceID
	{
		template<typename T> friend class ResourceAllocator;

	public:
		static const ResourceID INVALID;

	private:
		unsigned int m_ID = INVALID.m_ID;

	public:
		ResourceID<T>() = default;
		ResourceID<T>(unsigned int ID) : m_ID(ID) {};

		unsigned int get() const { return m_ID; }

		operator unsigned int() { return m_ID; }
		bool operator==(const ResourceID<T>& other) const { return m_ID == other.m_ID; }
		bool operator!=(const ResourceID<T>& other) const { return !(*this == other); }
	};

	template<typename T>
	const ResourceID<T> ResourceID<T>::INVALID = ResourceID<T>{ (unsigned int)-1 };
}

namespace std
{
	template<typename T>
	struct hash<nds_se::ResourceID<T>>
	{
		std::size_t operator()(const nds_se::ResourceID<T>& r) const
		{
			using std::hash;
			return hash<unsigned int>()(r.get());
		}
	};
}