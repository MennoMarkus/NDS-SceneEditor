#pragma once

namespace nds_se
{
	struct Triangle
	{
		unsigned int m_index1 = 0;
		unsigned int m_index2 = 0;
		unsigned int m_index3 = 0;

		unsigned int m_stripID = 0;

		Triangle() = default;
		Triangle(unsigned int index1, unsigned int index2, unsigned int index3) :
			m_index1(index1), m_index2(index2), m_index3(index3)
		{}
	};

	struct TriangleEdge
	{
		unsigned int m_index1;
		unsigned int m_index2;

		bool operator==(const TriangleEdge& rhs) const { return m_index1 == rhs.m_index1 && m_index2 == rhs.m_index2; }

		TriangleEdge(unsigned int index1, unsigned int index2) :
			m_index1(index1), m_index2(index2)
		{}
	};

	enum TriangleStripOrder
	{
		ABC,
		BCA,
		CAB
	};

	struct TriangleStrip
	{
		size_t m_size = 0;
		unsigned int m_startTriangleID = 0;
		TriangleStripOrder m_order = ABC;

		TriangleStrip() = default;
		TriangleStrip(unsigned int startTriangleID, TriangleStripOrder order, size_t size) :
			m_startTriangleID(startTriangleID), m_order(order), m_size(size)
		{}
	};
}