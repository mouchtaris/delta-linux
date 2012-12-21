/**
 *	DeltaAST.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef DELTA_DELTAAST_H
#define DELTA_DELTAAST_H

#include "Common.h"
#include "DeltaASTVisitor.h"

#include <list>
#include <string.h>

#include <boost/static_assert.hpp>
#include <boost/lexical_cast.hpp>

class DeltaTable;

namespace delta {

////////////////////////////////////////////////////////////////////////

class DeltaASTNode;
typedef std::list<DeltaASTNode*> DeltaASTNodeList;

////////////////////////////////////////////////////////////////////////

class DeltaASTNode {
public:
	CLASS_TYPE_ID(0);

	typedef Range Range;

	DeltaASTNode (void) : m_parent(0) {}
	DeltaASTNode (const Range& range) : m_range(range), m_parent(0) {}
	DeltaASTNode (const DeltaASTNode& other) : m_range(other.m_range), m_parent(0) {}
	virtual ~DeltaASTNode (void) {}

	const Range&	GetRange (void) const { return m_range; }
	Range&			GetRange (void) { return m_range; }

	DeltaASTNode*	GetParent (void) const { return m_parent; }

	virtual DeltaTable* ToTable (void) const;
	virtual const std::string GetValueStr(void) const = 0;

	//******************************************************************
	// RemoveChild, and ReplaceChild also delete the node.
	// Return true if the operation was successful.
	//
	virtual DeltaASTNode* Clone (void) const = 0;

	virtual bool GetChildren (DeltaASTNodeList& result) const = 0;
	virtual bool RemoveChild (DeltaASTNode* node) = 0;
	virtual bool ReplaceChild (DeltaASTNode* node, DeltaASTNode* replacement) = 0;

	//******************************************************************
	// Visitor support
	//
	virtual bool Accept (DeltaASTVisitor* visitor) = 0;
	virtual bool AcceptUp (DeltaASTVisitor* visitor) = 0;
	virtual bool AcceptBack (DeltaASTVisitor* visitor, DeltaASTNode* child) = 0;
	virtual void AcceptNode (DeltaASTNodeVisitor* visitor) = 0;

	bool AcceptBack (DeltaASTVisitor* visitor)
		{ return this->Accept(visitor) && this->AcceptBackParent(visitor, this); }

// protected: ??? WTF ???
	void SetParent (DeltaASTNode* parent) { m_parent = parent; }

	bool AcceptUpParent (DeltaASTVisitor* visitor) {
		if (m_parent)
			return m_parent->AcceptUp(visitor);
		return false;
	}
	bool AcceptBackParent (DeltaASTVisitor* visitor, DeltaASTNode* node) {
		if (m_parent)
			return m_parent->AcceptBack(visitor, node);
		return false;
	}

private:
	Range			m_range;
	DeltaASTNode*	m_parent;
};

////////////////////////////////////////////////////////////////////////
// Utility classes
//
template<class TValue>
class ValueHolder {
public:
	typedef TValue			DataType;
	typedef PARAM(TValue)	ParamDataType;

	void			SetValue (PARAM(TValue) value) { m_value = value; }
	PARAM(TValue)	GetValue (void) const { return m_value; }

protected:
	const std::string GetValueStr (void) const
		{ return boost::lexical_cast<std::string>(m_value); }

private:
	TValue m_value;
};

//**********************************************************************

template<>
class ValueHolder<ConstValueType> {
public:
	typedef ConstValueType	DataType;
	typedef ConstValueType	ParamDataType;

	void			SetValue (ConstValueType value) { m_value = value; }
	ConstValueType	GetValue (void) const { return m_value; }

protected:
	const std::string GetValueStr (void) const { return ConstValueTypeStr(m_value); }

private:
	ConstValueType m_value;
};

//**********************************************************************

template<>
class ValueHolder<TableConstKeyType> {
public:
	typedef TableConstKeyType	DataType;
	typedef TableConstKeyType	ParamDataType;

	void				SetValue (TableConstKeyType value) { m_value = value; }
	TableConstKeyType	GetValue (void) const { return m_value; }

protected:
	const std::string GetValueStr (void) const { return TableConstKeyTypeStr(m_value); }

private:
	TableConstKeyType m_value;
};

//**********************************************************************

template<>
class ValueHolder<void> {
protected:
	const std::string GetValueStr (void) const { return ""; }
};

////////////////////////////////////////////////////////////////////////

template<class TASTNode>
struct NodeConverter : public std::unary_function<TASTNode*, DeltaASTNode*> {
	DeltaASTNode* operator() (TASTNode* node) const { return node; }
};

//**********************************************************************

template<class TASTNode>
struct NodeCloner : public std::unary_function<const TASTNode*, TASTNode*> {
	TASTNode* operator() (const TASTNode* node) const
		{ return static_cast<TASTNode*>(node->Clone()); }
};

////////////////////////////////////////////////////////////////////////

template<unsigned _TypeId, class TASTNode, class TValue>
class ContainerASTNode : public DeltaASTNode, public ValueHolder<TValue> {
public:
	CLASS_TYPE_ID(_TypeId);

	typedef TASTNode				ValueType;

	typedef std::list<TASTNode*>			List;
	typedef typename List::iterator			Iterator;
	typedef typename List::reverse_iterator	RIterator;

	////////////////////////////////////////////////////////////////////

	ContainerASTNode (void) {}
	ContainerASTNode (const Range& range) : DeltaASTNode(range) {}
	ContainerASTNode (const ContainerASTNode& other) :
		DeltaASTNode(other), ValueHolder<TValue>(other), m_children(other.m_children.size())
		{
			std::transform(
				other.m_children.begin(), other.m_children.end(),
				m_children.begin(), NodeCloner<TASTNode>()
			);
			std::for_each(
				m_children.begin(), m_children.end(),
				std::bind2nd(std::mem_fun(&DeltaASTNode::SetParent), this)
			);
		}
	~ContainerASTNode (void) { this->Clear(); }

	//******************************************************************
	// Unary specific API
	//
	void AppendChild (TASTNode* child) {				// Takes ownership
		if (child) {
			m_children.push_back(child);
			child->SetParent(this);
		}
	}
	void InsertChild (Iterator iter, TASTNode* child) {
		if (child) {
			m_children.insert(iter, child);
			child->SetParent(this);
		}
	}
	void RemoveChild (Iterator iter) {
		if (iter != m_children.end()) {
			delete *iter;
			m_children.erase(iter);
		}
	}
	void Splice (Iterator iter, ContainerASTNode* src) {
		std::for_each(src->Begin(), src->End(),
			std::bind2nd(std::mem_fun(&DeltaASTNode::SetParent), this));
		m_children.splice(iter, src->m_children);
	}

	Iterator EraseChild (Iterator iter) {
		assert(iter != this->End());
		delete *iter;
		return m_children.erase(iter);
	}

	bool	Empty (void) const { return m_children.empty(); }
	size_t	Size (void) const { return m_children.size(); }

	void Clear (void) {
		std::for_each(m_children.begin(), m_children.end(), util::deleter<TASTNode>());
		m_children.clear();
	}

	const List&	GetChildren (void) const { return m_children; }

	TASTNode* Front (void) { return m_children.front(); }
	TASTNode* Back (void) { return m_children.back(); }

	Iterator Begin (void) { return m_children.begin(); }
	Iterator End (void) { return m_children.end(); }

	//******************************************************************
	// Common virtual API
	//
	virtual const std::string GetValueStr(void) const { return ValueHolder<TValue>::GetValueStr(); }

	virtual DeltaASTNode* Clone (void) const { return new ContainerASTNode(*this); }

	virtual bool GetChildren (DeltaASTNodeList& result) const
	{
		std::transform(m_children.begin(), m_children.end(),
			std::back_inserter(result), NodeConverter<TASTNode>());
		return !m_children.empty();
	}

	virtual bool RemoveChild (DeltaASTNode* node)
	{
		Iterator iter = std::find(m_children.begin(), m_children.end(), node);
		if (iter != m_children.end()) {
			delete *iter;
			m_children.erase(iter);
			return true;
		}
		return false;
	}

	virtual bool ReplaceChild (DeltaASTNode* node, DeltaASTNode* replacement)
	{
		if (!TASTNode::Type || TASTNode::Type == replacement->GetType()) {
			Iterator iter = std::find(m_children.begin(), m_children.end(), node);
			if (iter != m_children.end()) {
				delete *iter;
				if (replacement) {
					replacement->SetParent(this);
					m_children.insert(iter, static_cast<TASTNode*>(replacement));
				}
				m_children.erase(iter);
				return true;
			}
		}
		return false;
	}

	//******************************************************************
	// Visitor support
	//
	virtual bool Accept (DeltaASTVisitor* visitor) { // Cannot modify children inside visitor
		if (visitor->VisitEnter(this)) {
			Iterator i = m_children.begin(), end = m_children.end();
			for (; i != end; ++i)
				if (!(*i)->Accept(visitor))
					return false;
			return visitor->VisitLeave(this);
		}
		return false;
	}
	virtual bool AcceptUp (DeltaASTVisitor* visitor) {
		if (visitor->VisitEnter(this))
			this->AcceptUpParent(visitor);
		return visitor->VisitLeave(this);
	}
	virtual bool AcceptBack (DeltaASTVisitor* visitor, DeltaASTNode* node) {
		assert(node->GetParent() == this);
		if (visitor->VisitEnter(this)) {
			RIterator i = std::find(m_children.rbegin(), m_children.rend(), node);
			assert(i != m_children.rend());
			while (++i != m_children.rend()) {
				if (!(*i)->Accept(visitor))
					return false;
			}
			if (this->AcceptBackParent(visitor, this))
				return visitor->VisitLeave(this);
		}
		return false;
	}
	virtual void AcceptNode (DeltaASTNodeVisitor* visitor) { visitor->Visit(this); }

private:
	List m_children;
};

////////////////////////////////////////////////////////////////////////

template<unsigned _TypeId, unsigned _N, class TASTNode, class TValue>
class NaryASTNode : public DeltaASTNode, public ValueHolder<TValue> {
public:
	CLASS_TYPE_ID(_TypeId);

	////////////////////////////////////////////////////////////////////

	template<unsigned N> struct acceptor {
		static bool apply (TASTNode* nodes[], DeltaASTVisitor* visitor) {
			return acceptor<N-1>::apply(nodes, visitor) &&
				(!nodes[N] || nodes[N]->Accept(visitor));
		}
	};
	template<> struct acceptor<0> {
		static bool apply (TASTNode* nodes[], DeltaASTVisitor* visitor)
			{ return !nodes[0] || nodes[0]->Accept(visitor); }
	};

	////////////////////////////////////////////////////////////////////

	template<unsigned N> struct cloner {
		static void apply (TASTNode* nodes[], const TASTNode* const others[]) {
			cloner<N-1>::apply(nodes, others);
			nodes[N] = others[N] ? others[N]->Clone() : 0;
		}
	};
	template<> struct cloner<0> {
		static void apply (TASTNode* nodes[], const TASTNode* const others[])
			{ nodes[0] = others[0] ? others[0]->Clone() : 0; }
	};

	////////////////////////////////////////////////////////////////////

	template<unsigned N> struct parentsetter {
		static void apply (TASTNode* nodes[], DeltaASTNode* parent) {
			parentsetter<N-1>::apply(nodes, parent);
			if (nodes[N])
				nodes[N]->SetParent(parent);
		}
	};
	template<> struct parentsetter<0> {
		static void apply (TASTNode* nodes[], DeltaASTNode* parent)
			{ if (nodes[0]) nodes[0]->SetParent(parent); }
	};

	////////////////////////////////////////////////////////////////////

	template<unsigned N> struct appender {
		static bool apply (TASTNode* const nodes[], DeltaASTNodeList& result) {
			bool retVal = appender<N-1>::apply(nodes, result);
			if (!nodes[N])
				return retVal;
			result.push_back(nodes[N]);
			return true;
		}
	};
	template<> struct appender<0> {
		static bool apply (TASTNode* const nodes[], DeltaASTNodeList& result) {
			if (!nodes[0])
				return false;
			result.push_back(nodes[0]);
			return true;
		}
	};

	////////////////////////////////////////////////////////////////////

	typedef TASTNode ValueType;

	NaryASTNode (void) { this->zero(); }
	NaryASTNode (const Range& range) : DeltaASTNode(range) { this->zero(); }
	NaryASTNode (const NaryASTNode& other) :
		DeltaASTNode(other), ValueHolder<TValue>(other)
	{
		cloner<_N-1>::apply(m_children, other.m_children);
		parentsetter<_N-1>::apply(m_children, this);
	}
	~NaryASTNode (void) { this->Clear(); }

	//******************************************************************
	// Unary specific API
	//
	template<unsigned _index>
	void SetChild (TASTNode* child = 0) {		// Takes ownership
		BOOST_STATIC_ASSERT(_index < _N);
		delete m_children[_index];
		if (m_children[_index] = child)
			child->SetParent(this);
	}
	template<unsigned _index>
	TASTNode* GetChild (void) const {
		BOOST_STATIC_ASSERT(_index < _N);
		return m_children[_index];
	}
	void Clear (void) {
		for (size_t i = 0; i < _N; ++i) {
			delete m_children[i];
			m_children[i] = 0;
		}
	}

	//******************************************************************
	// Common virtual API
	//
	virtual const std::string GetValueStr(void) const { return ValueHolder<TValue>::GetValueStr(); }

	virtual DeltaASTNode* Clone (void) const { return new NaryASTNode(*this); }

	virtual bool GetChildren (DeltaASTNodeList& result) const
		{ return appender<_N-1>::apply(m_children, result); }

	virtual bool RemoveChild (DeltaASTNode* node)
	{
		for (size_t i = 0; i < _N; ++i) {
			if (m_children[i] == node) {
				delete m_children[i];
				m_children[i] = 0;
				return true;
			}
		}
		return false;
	}

	virtual bool ReplaceChild (DeltaASTNode* node, DeltaASTNode* replacement)
	{
		if (!TASTNode::Type || TASTNode::Type == replacement->GetType())
			for (size_t i = 0; i < _N; ++i)
				if (m_children[i] == node) {
					delete m_children[i];
					if (replacement)
						replacement->SetParent(this);
					m_children[i] = static_cast<TASTNode*>(replacement);
					return true;
				}
		return false;
	}


	//******************************************************************
	// Visitor support
	//
	virtual bool Accept (DeltaASTVisitor* visitor) {
		return visitor->VisitEnter(this) && acceptor<_N-1>::apply(m_children, visitor) &&
			visitor->VisitLeave(this);
	}
	virtual bool AcceptUp (DeltaASTVisitor* visitor) {
		if (visitor->VisitEnter(this))
			this->AcceptUpParent(visitor);
		return visitor->VisitLeave(this);
	}
	virtual bool AcceptBack (DeltaASTVisitor* visitor, DeltaASTNode* node) {
		assert(node->GetParent() == this);
		if (visitor->VisitEnter(this)) {
			size_t index =0;
			for (; index < _N; ++index) {
				if (m_children[index] == node)
					break;
			}
			while (index--) {
				if (m_children[index] && !m_children[index]->Accept(visitor))
					return false;
			}
			if (this->AcceptBackParent(visitor, this))
				return visitor->VisitLeave(this);
		}
		return false;
	}
	virtual void AcceptNode (DeltaASTNodeVisitor* visitor) { visitor->Visit(this); }

private:
	void zero (void) { memset(m_children, 0, sizeof(TASTNode*) * _N); }

	TASTNode* m_children[_N];
};

////////////////////////////////////////////////////////////////////////

template<unsigned _TypeId, class TASTNodeLeft, class TASTNodeRight, class TValue>
class BinaryASTNode : public DeltaASTNode, public ValueHolder<TValue> {
public:
	CLASS_TYPE_ID(_TypeId);

	typedef TASTNodeLeft	LeftValueType;
	typedef TASTNodeRight	RightValueType;

	BinaryASTNode (void) : m_leftChild(0), m_rightChild(0) {}
	BinaryASTNode (const Range& range, TASTNodeLeft* left = 0, TASTNodeRight* right = 0) :
		DeltaASTNode(range), m_leftChild(0), m_rightChild(0) {
			this->SetLeftChild(left);
			this->SetRightChild(right);
		}
	BinaryASTNode (const BinaryASTNode& other) :
		DeltaASTNode(other), ValueHolder<TValue>(other), m_leftChild(0), m_rightChild(0) {
			if (other.m_leftChild)
				SetLeftChild(static_cast<TASTNodeLeft*>(other.m_leftChild->Clone()));
			if (other.m_rightChild)
				SetRightChild(static_cast<TASTNodeLeft*>(other.m_rightChild->Clone()));
		}
	~BinaryASTNode (void) {
		delete m_leftChild;
		delete m_rightChild;
	}

	//******************************************************************
	// Unary specific API
	//
	void SetLeftChild (TASTNodeLeft* left = 0) {	// Takes ownership
		delete m_leftChild;
		if (m_leftChild = left)
			left->SetParent(this);
	}
	void SetRightChild (TASTNodeRight* right = 0) {	// Takes ownership
		delete m_rightChild;
		if (m_rightChild = right)
			right->SetParent(this);
	}
	TASTNodeLeft*	GetLeftChild (void) const { return m_leftChild; }
	TASTNodeRight*	GetRightChild (void) const { return m_rightChild; }

	//******************************************************************
	// Common virtual API
	//
	virtual const std::string GetValueStr(void) const { return ValueHolder<TValue>::GetValueStr(); }

	virtual DeltaASTNode* Clone (void) const { return new BinaryASTNode(*this); }

	virtual bool GetChildren (DeltaASTNodeList& result) const
	{
		bool retVal = false;
		if (m_leftChild) {
			result.push_back(m_leftChild);
			retVal = true;
		}
		if (m_rightChild) {
			result.push_back(m_rightChild);
			retVal = true;
		}
		return retVal;
	}

	virtual bool RemoveChild (DeltaASTNode* node)
	{
		if (m_leftChild == node) {
			this->SetLeftChild(0);
			return true;
		}
		else if (m_rightChild == node) {
			this->SetRightChild(0);
			return true;
		}
		return false;
	}

	virtual bool ReplaceChild (DeltaASTNode* node, DeltaASTNode* replacement)
	{
		if (m_leftChild == node) {
			this->SetLeftChild(replacement);
			return true;
		}
		else if (m_rightChild == node) {
			this->SetRightChild(replacement);
			return true;
		}
		return false;
	}

	//******************************************************************
	// Visitor support
	//
	virtual bool Accept (DeltaASTVisitor* visitor) {
		return visitor->VisitEnter(this) && (!m_leftChild || m_leftChild->Accept(visitor)) &&
			(!m_rightChild || m_rightChild->Accept(visitor)) && visitor->VisitLeave(this);
	}
	virtual bool AcceptUp (DeltaASTVisitor* visitor) {
		if (visitor->VisitEnter(this))
			this->AcceptUpParent(visitor);
		return visitor->VisitLeave(this);
	}
	virtual bool AcceptBack (DeltaASTVisitor* visitor, DeltaASTNode* node) {
		assert(node->GetParent() == this);
		if (visitor->VisitEnter(this)) {
			if (m_rightChild == node) {
				if (m_leftChild && !m_leftChild->Accept(visitor))
					return false;
			}
			if (this->AcceptBackParent(visitor, this))
				return visitor->VisitLeave(this);
		}
		return false;
	}
	virtual void AcceptNode (DeltaASTNodeVisitor* visitor) { visitor->Visit(this); }

private:
	TASTNodeLeft*	m_leftChild;
	TASTNodeRight*	m_rightChild;
};

////////////////////////////////////////////////////////////////////////

template<unsigned _TypeId, class TASTNode, class TValue>
class UnaryASTNode : public DeltaASTNode, public ValueHolder<TValue> {
public:
	CLASS_TYPE_ID(_TypeId);

	typedef TASTNode ValueType;

	UnaryASTNode (void) : m_child(0) {}
	UnaryASTNode (const Range& range, TASTNode* child = 0) :
		DeltaASTNode(range), m_child(0) { this->SetChild(child); }
	UnaryASTNode (const UnaryASTNode& other) : DeltaASTNode(other), ValueHolder<TValue>(other), m_child(0)
		{ if (other.m_child) SetChild(static_cast<TASTNode*>(other.m_child->Clone())); }
	~UnaryASTNode (void) { delete m_child; }

	//******************************************************************
	// Unary specific API
	//
	void SetChild (TASTNode* child = 0) {	// Takes ownership
		delete m_child;
		if (m_child = child)
			child->SetParent(this);
	}
	TASTNode* GetChild (void) const { return m_child; }

	//******************************************************************
	// Common virtual API
	//
	virtual const std::string GetValueStr(void) const { return ValueHolder<TValue>::GetValueStr(); }

	virtual DeltaASTNode* Clone (void) const { return new UnaryASTNode(*this); }

	virtual bool GetChildren (DeltaASTNodeList& result) const
	{
		if (!m_child)
			return false;
		result.push_back(m_child);
		return true;
	}

	virtual bool RemoveChild (DeltaASTNode* node)
	{
		if (m_child == node) {
			this->SetChild(0);
			return true;
		}
		return false;
	}

	virtual bool ReplaceChild (DeltaASTNode* node, DeltaASTNode* replacement)
	{
		if (m_child == node && (!TASTNode::Type || TASTNode::Type == replacement->GetType())) {
			this->SetChild(static_cast<TASTNode*>(replacement));
			return true;
		}
		return false;
	}

	//******************************************************************
	// Visitor support
	//
	virtual bool Accept (DeltaASTVisitor* visitor) {
		return visitor->VisitEnter(this) && (!m_child || m_child->Accept(visitor)) &&
			visitor->VisitLeave(this);
	}
	virtual bool AcceptUp (DeltaASTVisitor* visitor) {
		if (visitor->VisitEnter(this))
			this->AcceptUpParent(visitor);
		return visitor->VisitLeave(this);
	}
	virtual bool AcceptBack (DeltaASTVisitor* visitor, DeltaASTNode* node) {
		assert(node->GetParent() == this && m_child == node);
		if (visitor->VisitEnter(this))
			return this->AcceptBackParent(visitor, this) && visitor->VisitLeave(this);
		return false;
	}
	virtual void AcceptNode (DeltaASTNodeVisitor* visitor) { visitor->Visit(this); }

private:
	TASTNode* m_child;
};

////////////////////////////////////////////////////////////////////////

template<unsigned _TypeId, class TValue>
class LeafASTNode : public DeltaASTNode, public ValueHolder<TValue> {
public:
	CLASS_TYPE_ID(_TypeId);

	LeafASTNode (void) {}
	LeafASTNode (const Range& range) : DeltaASTNode(range) {}
	LeafASTNode (const LeafASTNode& other) : DeltaASTNode(other), ValueHolder<TValue>(other) {}
	~LeafASTNode (void) {}

	//******************************************************************
	// Common virtual API
	//
	virtual const std::string GetValueStr(void) const { return ValueHolder<TValue>::GetValueStr(); }

	virtual DeltaASTNode* Clone (void) const { return new LeafASTNode(*this); }

	virtual bool GetChildren (DeltaASTNodeList& result) const { return false; }
	virtual bool RemoveChild (DeltaASTNode* node) { return false; }
	virtual bool ReplaceChild (DeltaASTNode* node, DeltaASTNode* replacement) { return false; }

	//******************************************************************
	// Visitor support
	//
	virtual bool Accept (DeltaASTVisitor* visitor) { return visitor->Visit(this); }
	virtual bool AcceptUp (DeltaASTVisitor* visitor) {
		if (visitor->Visit(this))
			return this->AcceptUpParent(visitor);
		return false;
	}
	virtual bool AcceptBack (DeltaASTVisitor* visitor, DeltaASTNode* node) {
		assert(node == this || node == 0);
		if (visitor->Visit(this))
			return this->AcceptBackParent(visitor, this);
		return false;
	}
	virtual void AcceptNode (DeltaASTNodeVisitor* visitor) { visitor->Visit(this); }
};

////////////////////////////////////////////////////////////////////////

} // namespace delta

#endif // DELTA_DELTAAST_H
