#pragma once
#include "triangleStripper/connectivityGraph/TriangleConnectivityGraphTypes.h"

namespace nds_se
{
	class TriangleConnectivityGraph
	{
	protected:
		NodesVector	m_nodes;
		ArcsVector m_arcs;

	public:
		TriangleConnectivityGraph() = default;
		explicit TriangleConnectivityGraph(size_t nodesCount);

		TriangleConnectivityGraph(const TriangleConnectivityGraph&) = delete;
		TriangleConnectivityGraph& operator=(const TriangleConnectivityGraph&) = delete;

		void init(const std::vector<unsigned int>& indices);

		bool empty() const;
		size_t size() const;

		void swap(TriangleConnectivityGraph& other);
		friend void swap(TriangleConnectivityGraph& lhs, TriangleConnectivityGraph& rhs) { lhs.swap(rhs); }

		// Arc access member functions
		ArcsIterator insertArc(NodeID initial, NodeID terminal);
		ArcsIterator insertArc(NodesIterator initial, NodesIterator terminal);

		// Node access member functions
		Node& operator[](NodeID ID);
		const Node& operator[](NodeID ID) const;

		void unmarkNodes();

		NodesIterator begin();
		NodesIterator end();
		NodesConstIterator begin() const;
		NodesConstIterator end() const;
		NodesReverseIterator rbegin();
		NodesReverseIterator rend();
		NodesConstReverseIterator rbegin() const;
		NodesConstReverseIterator rend() const;

	private:
		static bool compareConnectivityEdge(const ConnectivityEdge& lhs, const ConnectivityEdge& rhs);

		void linkNeighbours(const EdgeMap& edgeMap, ConnectivityEdge edge);
	};
}