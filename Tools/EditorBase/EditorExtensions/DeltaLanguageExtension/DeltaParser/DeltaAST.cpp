/**
 *	DeltaAST.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "DeltaAST.h"
#include "DeltaByteCodeTypes.h"
#include "DeltaTable.h"
#include "DeltaObject.h"

#include <boost/foreach.hpp>

namespace delta {

////////////////////////////////////////////////////////////////////////
// DeltaASTNode class
//

DeltaTable* DeltaASTNode::ToTable(void) const {
	DeltaTable* table = DeltaObject::NativeCodeHelpers::NewObject();
	table->Set("type", DeltaASTNodeTypeStr(GetType()));
	
	const std::string value = GetValueStr();
	if (!value.empty())
		table->Set("value", value);

	DeltaTable* range = DeltaObject::NewObject();
	table->Set("range", range);
	range->Set("left", (DeltaNumberValueType) m_range.left);
	range->Set("right", (DeltaNumberValueType) m_range.right);

	DeltaASTNodeList childrenList;
	if (GetChildren(childrenList)) {
		DeltaTable* children = DeltaObject::NewObject();
		table->Set("children", children);
		int i = 0;
		BOOST_FOREACH(DeltaASTNode *node, childrenList) {
			DeltaTable* t = node->ToTable();
			children->Set((DeltaNumberValueType) i++, t);
			DPTR(t)->DecRefCounter((DeltaValue *) 0);
		}
		children->Set("size", (DeltaNumberValueType) childrenList.size());
	}
	return table;
}

////////////////////////////////////////////////////////////////////////

} // namespace delta
