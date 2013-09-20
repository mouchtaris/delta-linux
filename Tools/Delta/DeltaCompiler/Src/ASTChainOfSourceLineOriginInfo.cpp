// ASTChainOfSourceLineOriginInfoNode.cpp
// Custom AST node functionality.
// ScriptFighter Project.
// Y. Lilis, September 2013.
// Separated from AST::Node.
//

#include "ASTChainOfSourceLineOriginInfo.h"
#include "ASTNode.h"

/////////////////////////////////////////////////////////

void AST::AddSourceLineOrigin (AST::Node* node, const SourceLineOriginInfo& info, bool front) {
	DASSERT(!info.symbolicURI.empty() && info.line);
	struct ChainOfSourceLineOriginInfoTraits {
		static void* Copier(void* val) {
			ChainOfSourceLineOriginInfo* info = (ChainOfSourceLineOriginInfo*) val;
			return DNEWCLASS(ChainOfSourceLineOriginInfo, (DPTR(info)->begin(), DPTR(info)->end()));
		}
		static void Cleaner(void* val) {
			ChainOfSourceLineOriginInfo* info = (ChainOfSourceLineOriginInfo*) val;
			DPTR(info);
			DDELETE(info);
		}
	};
	if (TreeAttribute* attr = DPTR(node)->GetAttribute(DELTA_AST_CHAIN_SOURCE_LINE_ORIGIN)) {
		ChainOfSourceLineOriginInfo* l = (ChainOfSourceLineOriginInfo*) DPTR(attr)->GetPointer();
		if (front)
			DPTR(l)->push_front(info);
		else
			DPTR(l)->push_back(info);
	}
	else {
		attr = DNEWCLASS(
			TreeAttribute,
			(DNEWCLASS(ChainOfSourceLineOriginInfo, (1, info)), ChainOfSourceLineOriginInfoTraits::Copier, ChainOfSourceLineOriginInfoTraits::Cleaner)
		);
		DPTR(node)->SetAttribute(DELTA_AST_CHAIN_SOURCE_LINE_ORIGIN, attr);
	}
}

/////////////////////////////////////////////////////////

void AST::AddChainOfSourceLineOrigin (AST::Node* node, const ChainOfSourceLineOriginInfo& info, bool front) {
	for (ChainOfSourceLineOriginInfo::const_iterator i = info.begin(); i != info.end(); ++i)
		AddSourceLineOrigin(node, *i, front);
}

/////////////////////////////////////////////////////////

const AST::ChainOfSourceLineOriginInfo* AST::GetChainOfSourceLineOrigin (const Node* node) {
	const TreeAttribute* attr = DPTR(node)->GetAttribute(DELTA_AST_CHAIN_SOURCE_LINE_ORIGIN);
	return (const ChainOfSourceLineOriginInfo*)(attr ? attr->GetPointer() : 0);
}

/////////////////////////////////////////////////////////
