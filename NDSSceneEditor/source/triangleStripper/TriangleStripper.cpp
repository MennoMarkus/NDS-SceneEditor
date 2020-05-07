#include "triangleStripper/TriangleStripper.h"

#include <cassert>
#include <algorithm>

namespace nds_se 
{
	TriangleStripper::TriangleStripper(const std::vector<unsigned int>& indices) :
		m_connectivityGraph(indices.size() / 3),
		m_currentStripID(0),
		m_firstRun(true)
	{
		m_connectivityGraph.init(indices);
	}

	void TriangleStripper::strip(std::vector<Primative>& o_primatives)
	{
		// Reset all data in case strip is called twice.
		if (!m_firstRun) 
		{
			// Reset all strip ids
			m_currentStripID = 0;
			for (auto it = m_connectivityGraph.begin(); it != m_connectivityGraph.end(); ++it)
			{
				(*it)->m_stripID = 0;
			}

			m_connectivityGraph.unmarkNodes();
			m_triangleHeap.clear();
			m_candidateTriangleIDs.clear();
		}
		m_firstRun = false;

		// Run stripify process.
		initTriangleHeap();
		addTriangleStripPrimatives();
		addTrianglesPrimative();
	
		// Return the resulting vector by swapping references.
		m_primatives.swap(o_primatives);
	}

	void TriangleStripper::initTriangleHeap()
	{
		// Add all triangles into the heap, sorted by connection count. Less connections is higher priority.
		m_triangleHeap.reserve(m_connectivityGraph.size());
		for (unsigned int i = 0; i < m_connectivityGraph.size(); i++)
		{
			m_triangleHeap.push({ i, (unsigned int)m_connectivityGraph[i].size() });
		}

		// Remove triangles without connections.
		while (m_triangleHeap.size() > 0 && m_triangleHeap.top().m_connectionCount == 0)
		{
			m_triangleHeap.pop();
		}
	}

	void TriangleStripper::addTriangleStripPrimatives()
	{
		while (m_triangleHeap.size() > 0)
		{

			// Add the best triangle as a starting candidate for the strip.
			unsigned int candidate = m_triangleHeap.top().m_triangleID;
			m_candidateTriangleIDs.push_back(candidate);

			while (m_candidateTriangleIDs.size() > 0)
			{
				TriangleStrip bestTriangleStrip = getBestTriangleStrip();

				if (bestTriangleStrip.m_size >= MIN_STRIP_LENGTH)
					buildStrip(bestTriangleStrip);
			}

			// Remove all triangles that are no longer possible candidates.
			m_triangleHeap.eraseTriangle(candidate);

			while (m_triangleHeap.size() > 0 && m_triangleHeap.top().m_connectionCount == 0)
			{
				m_triangleHeap.pop();
			}
		}
	}

	void TriangleStripper::addTrianglesPrimative()
	{
		// Add all the triangles that are not part of any strip into a single triangles primative
		Primative trianglesPrimative;
		trianglesPrimative.m_type = TRIANGLES;

		m_primatives.push_back(trianglesPrimative);
		std::vector<unsigned int>& indices = m_primatives.back().m_indices;

		for (size_t i = 0; i < m_connectivityGraph.size(); i++)
		{
			if (!m_connectivityGraph[i].isMarked())
			{
				indices.push_back(m_connectivityGraph[i]->m_index1);
				indices.push_back(m_connectivityGraph[i]->m_index2);
				indices.push_back(m_connectivityGraph[i]->m_index3);
			}
		}

		// Don't add the triangles primative if there are no left over triangles.
		if (indices.size() == 0)
		{
			m_primatives.pop_back();
		}
	}

	TriangleStrip TriangleStripper::getBestTriangleStrip()
	{
		TriangleStrip bestStrip;

		while (m_candidateTriangleIDs.size() > 0) 
		{
			// Get a candidate triangle.
			unsigned int candidate = m_candidateTriangleIDs.back();
			m_candidateTriangleIDs.pop_back();

			// Skip useless candidates.
			if ((m_connectivityGraph[candidate].isMarked()) || 
				(m_triangleHeap.findTriangle(candidate)->m_connectionCount == 0))
				continue;		

			// Forward extend candidate trianlge on all 3 edges.
			for (int i = 0; i < 3; i++) 
			{

				TriangleStrip strip = forwardExtendTriangleStrip(candidate, (TriangleStripOrder)i);

				if (strip.m_size >= MIN_STRIP_LENGTH && strip.m_size > bestStrip.m_size)
					bestStrip = strip;
			}

			// Backward extend candidate in all 6 possible directions.
			for (int i = 0; i < 3; i++) 
			{
				TriangleStrip strip = backwardExtendTriangleStrip(candidate, (TriangleStripOrder)i, false);

				if (strip.m_size >= MIN_STRIP_LENGTH && strip.m_size > bestStrip.m_size)
					bestStrip = strip;
			}

			for (int i = 0; i < 3; i++)
			{
				TriangleStrip strip = backwardExtendTriangleStrip(candidate, (TriangleStripOrder)i, true);

				if (strip.m_size >= MIN_STRIP_LENGTH && strip.m_size > bestStrip.m_size)
					bestStrip = strip;
			}
		}

		return bestStrip;
	}

	TriangleStrip TriangleStripper::forwardExtendTriangleStrip(unsigned int start, TriangleStripOrder order)
	{
		TriangleStripOrder startOrder = order;

		// Begin a new triangle strip.
		m_currentStripID++;
		m_connectivityGraph[start]->m_stripID = m_currentStripID;
		addTriangleStripTriangle(*m_connectivityGraph[start], order, false);

		size_t size = 1;
		bool clockWise = false;

		// Extend strip while there are triangles that can connect.
		for (auto node = m_connectivityGraph.begin() + start; node != m_connectivityGraph.end(); ++size)
		{
			auto link = forwardLinkToNeighbour(node, clockWise, order, false);

			// Stop if we can't extend further.
			if (link == node->end())
			{
				node = m_connectivityGraph.end();
				--size;
			}
			else
			{
				// Get the next node to link to.
				node = link->getTerminal();
				(*node)->m_stripID = m_currentStripID;
				clockWise = !clockWise;
			}
		}

		return TriangleStrip(start, startOrder, size);
	}

	ArcsConstIterator TriangleStripper::forwardLinkToNeighbour(NodesConstIterator node, bool isClockWise, TriangleStripOrder& o_order, bool isBuildStrip)
	{
		TriangleEdge edge = getTriangleEndEdge(**node, o_order);

		for (auto link = node->begin(); link != node->end(); ++link)
		{
			Triangle& nextTriangle = **link->getTerminal();

			// Check if nextTriangle has already been used.
			if (isBuildStrip || nextTriangle.m_stripID != m_currentStripID)
			{
				if (!link->getTerminal()->isMarked())
				{
					// Does the current candidate triangle match the required position for the strip?
					if (edge.m_index1 == nextTriangle.m_index2 &&
						edge.m_index2 == nextTriangle.m_index1)
					{
						o_order = isClockWise ? ABC : BCA;
						addTriangleStripIndex(nextTriangle.m_index3, isBuildStrip);
						return link;
					}
					else if (edge.m_index1 == nextTriangle.m_index3 &&
							 edge.m_index2 == nextTriangle.m_index2)
					{
						o_order = isClockWise ? BCA : CAB;
						addTriangleStripIndex(nextTriangle.m_index1, isBuildStrip);
						return link;
					}
					else if (edge.m_index1 == nextTriangle.m_index1 &&
							 edge.m_index2 == nextTriangle.m_index3)
					{
						o_order = isClockWise ? CAB : ABC;
						addTriangleStripIndex(nextTriangle.m_index2, isBuildStrip);
						return link;
					}
				}
			}
		}

		return node->end();
	}

	TriangleStrip TriangleStripper::backwardExtendTriangleStrip(unsigned int start, TriangleStripOrder order, bool isClockWise)
	{
		// Begin a new triangle strip.
		m_currentStripID++;
		m_connectivityGraph[start]->m_stripID = m_currentStripID;

		size_t size = 1;

		// Extend strip while there are triangles that can connect.
		NodesIterator node;
		for (node = m_connectivityGraph.begin() + start; node != m_connectivityGraph.end(); ++size)
		{
			auto link = backwardLinkToNeighbour(node, isClockWise, order);

			// Stop if we can't extend further.
			if (link == node->end())
			{
				break;
			}
			else
			{
				// Get the next node to link to.
				node = link->getTerminal();
				(*node)->m_stripID = m_currentStripID;
				isClockWise = !isClockWise;
			}
		}

		// If the first triangle is clockwise, return an empty strip.
		if (isClockWise)
			return TriangleStrip();

		return TriangleStrip(node - m_connectivityGraph.begin(), order, size);
	}

	ArcsConstIterator TriangleStripper::backwardLinkToNeighbour(NodesConstIterator node, bool isClockWise, TriangleStripOrder& o_order)
	{
		const TriangleEdge edge = getTriangleStartEdge(**node, o_order);

		for (auto link = node->begin(); link != node->end(); ++link)
		{
			const Triangle& nextTriangle = **link->getTerminal();

			// Check if nextTriangle has already been used.
			if (nextTriangle.m_stripID != m_currentStripID && !link->getTerminal()->isMarked())
			{
				// Does the current candidate triangle match the required position for the strip?
				if (edge.m_index1 == nextTriangle.m_index2 && 
					edge.m_index2 == nextTriangle.m_index1)
				{
					o_order = isClockWise ? CAB : BCA;
					return link;
				}
				else if (edge.m_index1 == nextTriangle.m_index3 && 
						 edge.m_index2 == nextTriangle.m_index2)
				{
					o_order = isClockWise ? ABC : CAB;
					return link;
				}
				else if (edge.m_index1 == nextTriangle.m_index1 && 
						 edge.m_index2 == nextTriangle.m_index3)
				{
					o_order = isClockWise ? BCA : ABC;
					return link;
				}
			}
		}

		return node->end();
	}

	void TriangleStripper::buildStrip(TriangleStrip strip)
	{
		unsigned int start = strip.m_startTriangleID;
		bool isClockWise = false;
		TriangleStripOrder order = strip.m_order;

		// Create a new triangle strip.
		m_primatives.push_back(Primative());
		m_primatives.back().m_type = TRIANGLE_STRIP;
		addTriangleStripTriangle(*m_connectivityGraph[start], order, true);
		markTriangle(start);

		// Extend strip while there are triangles that can connect.
		auto node = m_connectivityGraph.begin() + start;
		for (int i = 1; i < strip.m_size; i++)
		{
			auto link = forwardLinkToNeighbour(node, isClockWise, order, true);
			assert(link != node->end());

			// Get the next triangle.
			node = link->getTerminal();
			markTriangle(node - m_connectivityGraph.begin());
			isClockWise = !isClockWise;
		}
	}

	TriangleEdge TriangleStripper::getTriangleStartEdge(const Triangle& triangle, const TriangleStripOrder order)
	{
			 if (order == ABC) return TriangleEdge(triangle.m_index1, triangle.m_index2);
		else if (order == BCA) return TriangleEdge(triangle.m_index2, triangle.m_index3);
		else if (order == CAB) return TriangleEdge(triangle.m_index3, triangle.m_index1);
		else
		{
			assert(false && "Invalid order.");
			return TriangleEdge(0, 0);
		}
	}

	TriangleEdge TriangleStripper::getTriangleEndEdge(const Triangle& triangle, const TriangleStripOrder order)
	{
			 if (order == ABC) return TriangleEdge(triangle.m_index2, triangle.m_index3);
		else if (order == BCA) return TriangleEdge(triangle.m_index3, triangle.m_index1);
		else if (order == CAB) return TriangleEdge(triangle.m_index1, triangle.m_index2);
		else
		{
			assert(false && "Invalid order.");
			return TriangleEdge(0, 0);
		}
	}

	void TriangleStripper::addTriangleStripIndex(unsigned int index, bool isBuildStrip)
	{
		if (isBuildStrip)
		{
			m_primatives.back().m_indices.push_back(index);
		}
	}

	void TriangleStripper::addTriangleStripTriangle(const Triangle& triangle, TriangleStripOrder order, bool isBuildStrip)
	{
		if (order == ABC)
		{
			addTriangleStripIndex(triangle.m_index1, isBuildStrip);
			addTriangleStripIndex(triangle.m_index2, isBuildStrip);
			addTriangleStripIndex(triangle.m_index3, isBuildStrip);
		}
		else if (order == BCA)
		{
			addTriangleStripIndex(triangle.m_index2, isBuildStrip);
			addTriangleStripIndex(triangle.m_index3, isBuildStrip);
			addTriangleStripIndex(triangle.m_index1, isBuildStrip);
		}
		else if (order == CAB)
		{
			addTriangleStripIndex(triangle.m_index3, isBuildStrip);
			addTriangleStripIndex(triangle.m_index1, isBuildStrip);
			addTriangleStripIndex(triangle.m_index2, isBuildStrip);
		}
	}

	void TriangleStripper::markTriangle(unsigned int triangleID)
	{
		// Mark the triangle node.
		m_connectivityGraph[triangleID].mark();

		// Remove triangle from the heap.
		m_triangleHeap.eraseTriangle(triangleID);

		// Adjust the connection count of neighbouring triangles.
		for (auto link = m_connectivityGraph[triangleID].begin(); link != m_connectivityGraph[triangleID].end(); ++link)
		{
			unsigned int neighbourTriangleID = link->getTerminal() - m_connectivityGraph.begin();

			if (!m_connectivityGraph[neighbourTriangleID].isMarked() && m_triangleHeap.hasTriangle(neighbourTriangleID))
			{
				unsigned int connectionCount = m_triangleHeap.findTriangle(neighbourTriangleID)->m_connectionCount;
				connectionCount -= 1;

				// Update heap element.
				m_triangleHeap.eraseTriangle(neighbourTriangleID);
				m_triangleHeap.push({ neighbourTriangleID, connectionCount });
			}
		}
	}
} 
