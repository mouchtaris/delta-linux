/**
 *	xml.cpp
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
#include "xml.h"

namespace xml
{
	//-------------------------------------------------------//
	//---- class Node ---------------------------------------//

	Node::Node(void)
		: node(0)
	{
	}

	//-----------------------------------------------------------------------

	Node::Node(wxXmlNode* node)
		: node(node)
	{
	}

	//-----------------------------------------------------------------------

	Node::Node(const Node& src)
		: node(src.node)
	{
	}

	//-----------------------------------------------------------------------

	Node::~Node(void)
	{
	}

	//-----------------------------------------------------------------------

	void Node::Create(const String& name, const Node& parent)
	{
		Destroy();
		node = new wxXmlNode(parent.node, wxXML_ELEMENT_NODE, name);
	}

	//-----------------------------------------------------------------------

	void Node::Destroy(void)
	{
		delete node;
	}

	//-----------------------------------------------------------------------

	char* Node::ToString(void)
	{
		return 0;
	}

	//-----------------------------------------------------------------------

	const String Node::GetContent(void) const
	{
		return node->GetNodeContent();
	}

	//-----------------------------------------------------------------------

	const String Node::GetProperty(const String& key) const
	{
		static String null_string;
		return node->GetPropVal(key, null_string);
	}

	//-----------------------------------------------------------------------

	void Node::SetContent(const String& content)
	{
		for (wxXmlNode* child = node->GetChildren(); child != 0; child = child->GetNext())
			if (child->GetType() == wxXML_TEXT_NODE) {
				child->SetContent(content);
				return;
			}

		node->AddChild(new wxXmlNode(node, wxXML_TEXT_NODE, _T("text_node"), content));
	}

	//-----------------------------------------------------------------------

	void Node::SetProperty(const String& key, const String& value)
	{
		if (node->HasProp(key))
			node->DeleteProperty(key);
		node->AddProperty(key, value);
	}

	//-----------------------------------------------------------------------

	size_t Node::GetChildSize(const String& name) const
	{
		int count=0;

		if (name.empty())
		{
			for (wxXmlNode* child = node->GetChildren(); child != 0; child = child->GetNext())
				if (child->GetType() == wxXML_ELEMENT_NODE)
					++count;
		}
		else
		{
			for (wxXmlNode* child = node->GetChildren(); child != 0; child = child->GetNext())
				if (child->GetType() == wxXML_ELEMENT_NODE && child->GetName() == name)
					++count;
		}

		return count;
	}

	//-----------------------------------------------------------------------

	void Node::InsertChild(const Node& child)
	{
		node->AddChild(child.node);
	}

	//-----------------------------------------------------------------------

	Node Node::GetChild(const String& name, int index) const
	{
		int count=0;

		if (name.empty())
		{
			for (wxXmlNode* child = node->GetChildren(); child != 0; child = child->GetNext())
				if (child->GetType() == wxXML_ELEMENT_NODE	&&
					index == count++
				)
					return Node(child);
		}
		else
		{
			for (wxXmlNode* child = node->GetChildren(); child != 0; child = child->GetNext())
				if (child->GetType() == wxXML_ELEMENT_NODE	&&
					child->GetName() == name				&&
					index == count++
				)
					return Node(child);
		}

		return Node();
	}

	//-----------------------------------------------------------------------

	Node::Iterator Node::ChildBegin(const String& name) const
	{
		return Node::Iterator(name, node->GetChildren());
	}

	//-----------------------------------------------------------------------

	Node::Iterator Node::ChildEnd(const String& name) const
	{
		return Node::Iterator(name, Node());
	}

	//-----------------------------------------------------------------------


	//-------------------------------------------------------//
	//---- struct Node::Iterator ----------------------------//

	Node::Iterator::Iterator(const String& name, const Node& context)
		: name(name), context(context)
	{
		if (!name.empty() && !context.isNull() && name != context.Name())
			++(*this);
	}

	//-----------------------------------------------------------------------

	Node::Iterator& Node::Iterator::operator ++(void)
	{
		if (name.empty())
			do {
				context.node = context.node->GetNext();
			} while (context.node && context.node->GetType() != wxXML_ELEMENT_NODE);
		else
			do {
				context.node = context.node->GetNext();
			} while (context.node	&&
					(context.node->GetType() != wxXML_ELEMENT_NODE	||
					 context.node->GetName() != name));

		return (*this);
	}

	//-----------------------------------------------------------------------

	Node::Iterator Node::Iterator::operator ++(int)
	{
		Node::Iterator tmp(name, context);

		if (name.empty())
			do {
				context.node = context.node->GetNext();
			} while (context.node && context.node->GetType() != wxXML_ELEMENT_NODE);
		else
			do {
				context.node = context.node->GetNext();
			} while (context.node &&
					(context.node->GetType() != wxXML_ELEMENT_NODE	||
					 context.node->GetName() != name));

		return tmp;
	}

	//-----------------------------------------------------------------------
}
