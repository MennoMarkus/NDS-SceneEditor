#include "triangleStripper/connectivityGraph/TriangleConnectivityGraphTypes.h"

namespace nds_se
{
	NodesIterator Arc::getTerminal() const
	{
		return m_terminal;
	}

	Arc::Arc(NodesIterator terminal) :
		m_terminal(terminal)
	{}

	Node::Node(ArcsVector& arcs) :
		m_arcs(arcs),
		m_begin(std::numeric_limits<size_t>::max()),
		m_end(std::numeric_limits<size_t>::max()),
		m_isMarked(false)
	{}

	void Node::mark()
	{
		m_isMarked = true;
	}

	void Node::unmark()
	{
		m_isMarked = false;
	}

	bool Node::isMarked() const
	{
		return m_isMarked;
	}

	bool Node::empty() const
	{
		return m_begin == m_end;
	}

	size_t Node::size() const
	{
		return m_end - m_begin;
	}

	ArcsIterator Node::begin()
	{
		return m_arcs.begin() + m_begin;
	}

	ArcsIterator Node::end()
	{
		return m_arcs.begin() + m_end;
	}

	ArcsConstIterator Node::begin() const
	{
		return m_arcs.begin() + m_begin;
	}

	ArcsConstIterator Node::end() const
	{
		return m_arcs.begin() + m_end;
	}

	Triangle& Node::operator*()
	{
		return m_element;
	}

	Triangle* Node::operator->()
	{
		return &m_element;
	}

	const Triangle& Node::operator*() const
	{
		return m_element;
	}

	const Triangle* Node::operator->() const
	{
		return &m_element;
	}

	Triangle& Node::operator=(const Triangle& element)
	{
		return m_element = element;
	}
}