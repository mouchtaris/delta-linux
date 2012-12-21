// TreeToStringVisitor.cpp
// A simple visitor to make a string out of a tree
// mainly for testing purposes.
// ScriptFighter Project.
// A. Savidis, September 2009.
//

#include "TreeToStringVisitor.h"
#include "ustrings.h"

/////////////////////////////////////////////////////////

void AttributeStringifier::SetHandler (const std::string& nodeTag, Handler f, void* c)
	{ handlers[nodeTag].set(f, c); }

const std::string AttributeStringifier::operator()(
		const std::string&		nodeTag, 
		const std::string&		attr, 
		const TreeAttribute*	data
	) const {

	Handlers::const_iterator i = handlers.find(nodeTag);

	std::string result;
	if (i != handlers.end())
		i->second(attr, data, &result);
	else
	if (defaultHandler.f())
		defaultHandler(attr, data, &result);
	else
		result = data->toString();
	return result;
}

/////////////////////////////////////////////////////////

TreeToStringVisitor::TreeToStringVisitor (const AttributeStringifier& stringifier) : 
	tabstops(0),
	attrStringifier(stringifier) 
	{ SetDefaultHandler(&ToStringHandler, this); }

TreeToStringVisitor::~TreeToStringVisitor(){}

/////////////////////////////////////////////////////////

void TreeToStringVisitor::ToStringHandler (TreeNode* node, const std::string& id, bool entering, void* closure) {

	TreeToStringVisitor* self = (TreeToStringVisitor*) closure;
	DASSERT(self);

	bool isLeaf = !DPTR(node)->GetTotalAttributes() && ! DPTR(node)->GetTotalChildren();

	if (entering) {

		self->result.append(
			uconstructstr(
				"%s%s%s", 
				utabstops(self->tabstops++).c_str(), 
				id.empty() ? "" : (id + ":").c_str(),
				node->GetTag().c_str()
			)
		);

		if (!isLeaf) {
			self->result.append(" {\n");
			typedef TreeNode::Attrs Attrs;
			const Attrs& attrs = node->GetAttributes();
		
			for (Attrs::const_iterator i = attrs.begin(); i != attrs.end(); ++i) {
				std::string str = self->GetAttributeStringifier()(node->GetTag(), i->first, i->second);
				if (!str.empty())
					self->result.append(
						uconstructstr(
							"%s%s:%s\n", 
							utabstops(self->tabstops).c_str(), 
							i->first.c_str(),
							str.c_str()
						)
					);
			}
		}
	}
	else
	if (isLeaf)
		self->result.append("\n");
	else
		self->result.append(
			uconstructstr(
				"%s}\n", 
				utabstops(--self->tabstops).c_str()
			)
		);
}

/////////////////////////////////////////////////////////

const std::string TreeToStringVisitor::operator()(TreeNode* node, const std::string& id) {
	result.clear();
	tabstops = 0;
	node->AcceptPreOrder(this, id);
	return result;
}

/////////////////////////////////////////////////////////
