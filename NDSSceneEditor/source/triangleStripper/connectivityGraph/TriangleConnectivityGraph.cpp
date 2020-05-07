#include "triangleStripper/connectivityGraph/TriangleConnectivityGraph.h"

#include <assert.h>
#include <algorithm>

namespace nds_se
{
	TriangleConnectivityGraph::TriangleConnectivityGraph(size_t nodesCount) :
		m_nodes(nodesCount, Node(m_arcs))
	{
		m_arcs.reserve(nodesCount * 2);
	}

	void TriangleConnectivityGraph::init(const std::vector<unsigned int>& indices)
	{
		assert((indices.size() / 3) == size());

		// Fill the triangle data.
		for (NodeID i = 0; i < size(); i++)
		{
			(*this)[i] = Triangle(indices[i * 3 + 0], indices[i * 3 + 1], indices[i * 3 + 2]);
		}

		// Build the edge lookup table.
		EdgeMap edgeMap;
		edgeMap.reserve(size() * 3);

		for (NodeID i = 0; i < size(); i++)
		{
			const Triangle& triangle = *(*this)[i];

			edgeMap.push_back(ConnectivityEdge(triangle.m_index1, triangle.m_index2, i));
			edgeMap.push_back(ConnectivityEdge(triangle.m_index2, triangle.m_index3, i));
			edgeMap.push_back(ConnectivityEdge(triangle.m_index3, triangle.m_index1, i));
		}

		std::sort(edgeMap.begin(), edgeMap.end(), compareConnectivityEdge);

		// Link neighbour triangles together using the lookup table.
		for (NodeID i = 0; i < size(); i++)
		{
			const Triangle& triangle = *(*this)[i];

			linkNeighbours(edgeMap, ConnectivityEdge(triangle.m_index2, triangle.m_index1, i));
			linkNeighbours(edgeMap, ConnectivityEdge(triangle.m_index3, triangle.m_index2, i));
			linkNeighbours(edgeMap, ConnectivityEdge(triangle.m_index1, triangle.m_index3, i));
		}
	}

	bool TriangleConnectivityGraph::empty() const
	{
		return m_nodes.empty();
	}

	size_t TriangleConnectivityGraph::size() const
	{
		return m_nodes.size();
	}

	void TriangleConnectivityGraph::swap(TriangleConnectivityGraph& other)
	{
		std::swap(m_nodes, other.m_nodes);
		std::swap(m_arcs, other.m_arcs);
	}

	ArcsIterator TriangleConnectivityGraph::insertArc(NodeID initial, NodeID terminal)
	{
		assert(initial < size());
		assert(terminal < size());

		return insertArc(m_nodes.begin() + initial, m_nodes.begin() + terminal);
	}

	ArcsIterator TriangleConnectivityGraph::insertArc(NodesIterator initial, NodesIterator terminal)
	{
		assert((initial >= begin()) && (initial < end()));
		assert((terminal >= begin()) && (terminal < end()));

		Node& node = *initial;

		if (node.empty())
		{
			node.m_begin = m_arcs.size();
			node.m_end = m_arcs.size() + 1;
		}
		else
		{
			assert(node.m_end == m_arcs.size());
			++(node.m_end);
		}

		m_arcs.push_back(Arc(terminal));
		auto it = m_arcs.end();

		return (--it);
	}

	Node& TriangleConnectivityGraph::operator[](NodeID ID)
	{
		assert(ID < size());
		return m_nodes[ID];
	}

	const Node& TriangleConnectivityGraph::operator[](NodeID ID) const
	{
		assert(ID < size());
		return m_nodes[ID];
	}

	void TriangleConnectivityGraph::unmarkNodes()
	{
		for (auto& node : *this)
		{
			node.unmark();
		}
	}

	NodesIterator TriangleConnectivityGraph::begin()
	{
		return m_nodes.begin();
	}

	NodesIterator TriangleConnectivityGraph::end()
	{
		return m_nodes.end();
	}

	NodesConstIterator TriangleConnectivityGraph::begin() const
	{
		return m_nodes.begin();
	}

	NodesConstIterator TriangleConnectivityGraph::end() const
	{
		return m_nodes.end();
	}

	NodesReverseIterator TriangleConnectivityGraph::rbegin()
	{
		return m_nodes.rbegin();
	}

	NodesReverseIterator TriangleConnectivityGraph::rend()
	{
		return m_nodes.rend();
	}

	NodesConstReverseIterator TriangleConnectivityGraph::rbegin() const
	{
		return m_nodes.rbegin();
	}

	NodesConstReverseIterator TriangleConnectivityGraph::rend() const
	{
		return m_nodes.rend();
	}

	bool TriangleConnectivityGraph::compareConnectivityEdge(const ConnectivityEdge& lhs, const ConnectivityEdge& rhs)
	{
		return (lhs.m_index1 < rhs.m_index1) || 
			   ((lhs.m_index1 == rhs.m_index1) && (lhs.m_index2 < rhs.m_index2));
	}

	void TriangleConnectivityGraph::linkNeighbours(const EdgeMap& edgeMap, ConnectivityEdge edge)
	{
		// Find the first edge equal to edge.
		auto it = std::lower_bound(edgeMap.begin(), edgeMap.end(), edge, compareConnectivityEdge);

		// See if there are any other edges that are equal.
		while (it != edgeMap.end() && edge == (*it))
		{
			insertArc(edge.getTrianglePosition(), it->getTrianglePosition());
			++it;
		}
	}
}