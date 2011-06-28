/**
 *	DeltaASTDataTypes.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "DeltaASTDataTypes.h"

namespace delta {

const char * DeltaASTNodeTypeStr(int type)
{
	static const char *types[] =
	{
		NULL,	//DeltaASTNode SuperClass
		"StmtsASTNode",

		"ExpressionListASTNode",
		"ArgListASTNode",

		"UnaryKwdASTNode",
		"LeafKwdASTNode",

		"WhileASTNode",
		"ForASTNode",
		"ForeachASTNode",
		"IfASTNode",
		"ElseASTNode",
		"TryASTNode",
		"TrapASTNode",

		"FunctionASTNode",
		"FunctionNameASTNode",

		"StringifiedIdASTNode",

		"StdLibfuncASTNode",
		"LibfuncASTNode",
		"AttributeASTNode",
		"VariableASTNode",
		"ConstASTNode",
		"ArgASTNode",

		"NewAttributeASTNode",
		"TableElemASTNode",
		"TableElemsASTNode",
		"TableIndexListASTNode",
		"TableConstructASTNode",
		"TableConstKeyASTNode",

		"OtherStmtASTNode",

		NULL,	//Reserved for Cpp
		NULL,	//Reserved for Comments

		"TernaryOpASTNode",
		"PrefixOpASTNode",
		"SuffixOpASTNode",
		"BinaryOpASTNode",
		"UnaryOpASTNode",
		"CallASTNode",
		"CompoundASTNode",
		"UsingASTNode",
	};
	assert(type >= 0 && type < SIZEOF_ARRAY(types));
	return types[type];
}

//**********************************************************************

const char * ConstValueTypeStr(ConstValueType type)
{
	static const char *types[] =
	{
		"CONST_NUMBER",
		"CONST_STRING",
	};
	assert(type >= 0 && type < SIZEOF_ARRAY(types));
	return types[type];
}

//**********************************************************************

const char * TableConstKeyTypeStr(TableConstKeyType type)
{
	static const char *types[] =
	{
		"CONST_KEY_NUMBER",
		"CONST_KEY_STRING",
		"CONST_KEY_DOTID",
		"CONST_KEY_OPSTRING",
		"CONST_KEY_IDENT",
		"CONST_KEY_STRINGIFIED_ID",
	};
	assert(type >= 0 && type < SIZEOF_ARRAY(types));
	return types[type];
}

////////////////////////////////////////////////////////////////////////
// Range struct
//
void Range::Expand (const Range& r)
{
	if (this->Empty())
		*this = r;
	else if (!r.Empty()) {

		if (r.left < left)
			left = r.left;
		if (r.right > right)
			right = r.right;
	}
}

//**********************************************************************

void Range::ExpandLeft (const Range& r)
{
	if (this->Empty())
		*this = r;
	else if (!r.Empty()) {
		if (r.left < left)
			left = r.left;
	}
}

//**********************************************************************

void Range::ExpandRight (const Range& r)
{
	if (this->Empty())
		*this = r;
	else if (!r.Empty()) {
		if (r.right > right)
			right = r.right;
	}
}

//**********************************************************************

void Range::Update (uint pos, int offset)
{
	if (left >= pos) {
		left = this->applyOffset(left, pos, offset);
		right = this->applyOffset(right, pos, offset);
	}
	else if (right > pos)
		right = this->applyOffset(right, pos, offset);
}

//**********************************************************************

uint Range::applyOffset (uint value, uint pos, int offset)
{
	int newPos;
	return ((newPos = ((int) value) + offset) < (int) pos) ? pos : (uint) newPos;
}

////////////////////////////////////////////////////////////////////////

} // namespace delta
