#pragma once
#include <vector>

#include "triangleStripper/Triangle.h"

namespace nds_se
{
	class Node;
	class Arc;
	struct ConnectivityEdge;

	using NodeID = size_t;
	using NodesVector = std::vector<Node>;

	// Node iterator types
	using NodesIterator = NodesVector::iterator;
	using NodesConstIterator = NodesVector::const_iterator;
	using NodesReverseIterator = NodesVector::reverse_iterator;
	using NodesConstReverseIterator = NodesVector::const_reverse_iterator;

	using ArcsVector = std::vector<Arc>;

	// Arc iterator types
	using ArcsIterator = ArcsVector::iterator;
	using ArcsConstIterator = ArcsVector::const_iterator;

	using EdgeMap = std::vector<ConnectivityEdge>;


	class Arc
	{
		friend class TriangleConnectivityGraph;

	protected:
		NodesIterator m_terminal;

	public:
		NodesIterator getTerminal() const;

	protected:
		Arc(NodesIterator terminal);
	};

	class Node
	{
		friend class TriangleConnectivityGraph;
		friend class std::vector<Node>;

	protected:
		ArcsVector& m_arcs;
		size_t m_begin;
		size_t m_end;

		Triangle m_element;
		bool m_isMarked;

	public:
		void mark();
		void unmark();
		bool isMarked() const;

		bool empty() const;
		size_t size() const;

		ArcsIterator begin();
		ArcsIterator end();
		ArcsConstIterator begin() const;
		ArcsConstIterator end() const;

		Triangle& operator*();
		Triangle* operator->();
		const Triangle& operator*() const;
		const Triangle* operator->() const;

		Triangle& operator=(const Triangle& element);

	protected:
		Node(ArcsVector& arcs);
	};

	struct ConnectivityEdge : public TriangleEdge
	{
	private:
		size_t m_trianglePosition;

	public:
		size_t getTrianglePosition() const{ return m_trianglePosition; }

		ConnectivityEdge(unsigned int index1, unsigned int index2, size_t trianglePosition) :
			TriangleEdge(index1, index2), m_trianglePosition(trianglePosition)
		{}
	};
}