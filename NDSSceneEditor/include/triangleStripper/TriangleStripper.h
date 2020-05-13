#pragma once
#include <vector>

#include "triangleStripper/Triangle.h"
#include "triangleStripper/TriangleHeap.h"
#include "triangleStripper/connectivityGraph/TriangleConnectivityGraph.h"
#include "rendering/Primative.h"

namespace nds_se
{
	class TriangleStripper
	{
	private:
		const unsigned int MIN_STRIP_LENGTH = 2;

		bool m_firstRun;

		size_t m_currentStripID;
		TriangleHeap m_triangleHeap;
		TriangleConnectivityGraph m_connectivityGraph;
		std::vector<Primative> m_primatives;
		std::vector<unsigned int> m_candidateTriangleIDs;

	public:
		TriangleStripper(const std::vector<unsigned int>& indices);
		void strip(std::vector<Primative>& o_primatives);

	private:
		void initTriangleHeap();
		void addTriangleStripPrimatives();
		void addTrianglesPrimative();

		TriangleStrip getBestTriangleStrip();

		TriangleStrip forwardExtendTriangleStrip(unsigned int start, TriangleStripOrder order);
		ArcsConstIterator forwardLinkToNeighbour(NodesConstIterator node, bool isClockWise, TriangleStripOrder& o_order, bool isBuildStrip);
		TriangleStrip backwardExtendTriangleStrip(unsigned int start, TriangleStripOrder order, bool isClockWise);
		ArcsConstIterator backwardLinkToNeighbour(NodesConstIterator node, bool isClockWise, TriangleStripOrder& o_order);

		void buildStrip(TriangleStrip strip);

		TriangleEdge getTriangleStartEdge(const Triangle& triangle, TriangleStripOrder order);
		TriangleEdge getTriangleEndEdge(const Triangle& triangle, TriangleStripOrder order);
		void addTriangleStripIndex(unsigned int index, bool isBuildStrip);
		void addTriangleStripTriangle(const Triangle& triangle, TriangleStripOrder order, bool isBuildStrip);
		void markTriangle(unsigned int triangleID);
	};
}
