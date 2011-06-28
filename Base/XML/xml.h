/**
 *	xml.h
 *
 *	-- XML parser / saver --
 *
 *	Shell component is the basic skeleton of the IDE
 *	where the other visual components are attached
 *	in order to build the whole environment.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	March 2007
 */
#ifndef BASE_XML_H_
#define BASE_XML_H_

#include "Common.h"
#include "LibraryAPIDecls.h"

#include <wx/xml/xml.h>

#include <iterator>

namespace xml
{
	typedef wxXmlDocument Document;

	//------------------------------
	//-- class Node

	class _BASE_API Node
	{
	public:
		///--- type definitions
		struct Iterator;
		friend struct Node::Iterator;

		typedef Iterator iterator;
		typedef wxXmlNode* native_type;

		///--- constructors / destructor
		Node(void);
		Node(wxXmlNode* node);
		Node(const Node& src);
		~Node(void);

		///--- public API
		void Create(const String& name, const Node& parent=0);
		void Destroy(void);
		char* ToString(void);

		const String Name(void) const	{ return node->GetName(); }
		bool isNull(void) const			{ return node == 0; }

		const String GetContent(void) const;
		const String GetProperty(const String& key) const;

		void SetContent(const String& content);
		void SetProperty(const String& key, const String& value);

		size_t GetChildSize(const String& name=String()) const;
		void InsertChild(const Node& child);
		Node GetChild(const String& name, int index=0) const;

		iterator ChildBegin(const String& name=String()) const;
		iterator ChildEnd(const String& name=String()) const;

		native_type NativeType(void)	{ return node; }

		///--- overloaded operators
		const String operator [](const String& key) const	{ return GetProperty(key); }

		bool operator ==(const Node& _right) const	{ return (this->node == _right.node); }
		bool operator !=(const Node& _right) const	{ return (this->node != _right.node); }

	private:
		///--- private members
		native_type node;
	};

	//--------------------------------------------------------------


	//------------------------------
	//-- struct Node::Iterator

#pragma warning (push)
#pragma warning (disable:4275)

	struct _BASE_API Node::Iterator :
		public std::iterator<std::forward_iterator_tag, Node>
	{
		Iterator(const String& name, const Node& context);

		reference operator *(void)				{ return context; }
		const Node& operator *(void) const		{ return context; }

		Node* operator ->(void)					{ return &context; }
		const Node* operator ->(void) const		{ return &context; }

		Iterator& operator ++(void);	///< pre-increment
		Iterator  operator ++(int);		///< post-increment

		bool operator ==(const Iterator& _right)	{ return (this->context == _right.context); }
		bool operator !=(const Iterator& _right)	{ return (this->context != _right.context); }

	private:
		String name;
		value_type context;
	};

#pragma warning (pop)

	//--------------------------------------------------------------
}

#endif	//BASE_XML_H_
