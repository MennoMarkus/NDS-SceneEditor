#pragma once
#include <vector>
#include <algorithm>

namespace nds_se
{
	/* 
	The triangle heap keeps track of the highest priority triangle(s) to pick as triangle strip.
	Triangles with less connections get a higher priority.
	*/

	struct TriangleHeapElement
	{
		unsigned int m_triangleID;
		unsigned int m_connectionCount;
	};

	class TriangleHeap
	{
	private:
		std::vector<TriangleHeapElement> m_elements;

	public:
		TriangleHeap() = default;

		inline void clear();
		inline void reserve(size_t count);
		inline size_t size() const;

		inline TriangleHeapElement top() const;
		inline void push(TriangleHeapElement element);
		inline void pop();

		inline TriangleHeapElement* findTriangle(unsigned int triangleID);
		inline void eraseTriangle(unsigned int triangleID);
		inline bool hasTriangle(unsigned int triangleID);

	private:
		static bool compareElements(const TriangleHeapElement& lhs, const TriangleHeapElement& rhs)
		{
			return lhs.m_connectionCount > rhs.m_connectionCount;
		}
	};

	/*
	 * Implementation details
	 */

	inline void TriangleHeap::clear()
	{
		m_elements.clear();
	}

	inline void TriangleHeap::reserve(size_t count)
	{
		m_elements.reserve(count);
	}

	inline size_t TriangleHeap::size() const
	{
		return m_elements.size();
	}

	inline TriangleHeapElement TriangleHeap::top() const
	{
		return m_elements.front();
	}

	inline void TriangleHeap::push(TriangleHeapElement element)
	{
		m_elements.push_back(element);
		std::push_heap(m_elements.begin(), m_elements.end(), compareElements);
	}

	inline void TriangleHeap::pop()
	{
		std::pop_heap(m_elements.begin(), m_elements.end(), compareElements);
		m_elements.pop_back();
	}

	inline TriangleHeapElement* TriangleHeap::findTriangle(unsigned int triangleID)
	{
		// Find the first element with triangleID.
		auto it = std::find_if(m_elements.begin(), m_elements.end(), [triangleID](const TriangleHeapElement& e)
		{
			return e.m_triangleID == triangleID;
		});

		if (it != m_elements.end())
			return &(*it);

		return nullptr;
	}

	inline void TriangleHeap::eraseTriangle(unsigned int triangleID)
	{
		// Find the first element with triangleID.
		auto it = std::find_if(m_elements.begin(), m_elements.end(), [triangleID](const TriangleHeapElement& e)
		{
			return e.m_triangleID == triangleID;
		});

		// Erase and maintain the heap after removal.
		if (it != m_elements.end())
		{
			m_elements.erase(it);
			std::make_heap(m_elements.begin(), m_elements.end(), compareElements);
		}
	}

	inline bool TriangleHeap::hasTriangle(unsigned int triangleID)
	{
		return findTriangle(triangleID) != nullptr;
	}
}