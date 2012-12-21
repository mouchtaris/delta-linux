/**
 *	DeltaASTSymbolFinder.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "DeltaASTSymbolFinder.h"

#include "EditorMain/EditorWindow.h"
#include "DeltaParser/DeltaAST.h"

namespace delta {

////////////////////////////////////////////////////////////////////////
// DeltaASTSymbolFinder class
//
DeltaASTSymbolFinder::DeltaASTSymbolFinder (
		editor::EditorWindow*	window,
		const String&			hint,
		long					flags
	) : m_window(window), m_flags(flags), m_hint(hint)
{
}

//**********************************************************************

DeltaASTSymbolFinder::~DeltaASTSymbolFinder (void)
{
}

//**********************************************************************

const String DeltaASTSymbolFinder::GetCompletionList (void) const
{
	String completionList;
	StringSet::const_iterator iter = m_completionSet.begin();
	for (; iter != m_completionSet.end(); ++iter) {
		if (iter != m_completionSet.begin())
			completionList += + _T('\a');
		completionList += iter->symbol + _T('?') + iter->type;
	}
	return completionList;
}

//**********************************************************************

const String DeltaASTSymbolFinder::GetDeclarationList (void) const
{
	String declarationList;
	StringList::const_iterator iter = m_declarationList.begin();
	for (; iter != m_declarationList.end(); ++iter) {
		if (iter != m_declarationList.begin())
			declarationList += + _T('\a');
		declarationList += iter->symbol + _T('?') + iter->type;
	}
	return declarationList;
}

//**********************************************************************

#define IMPLEMENT_EMPTY_VISIT_ENTER(type)	bool DeltaASTSymbolFinder::VisitEnter (type* node) { return true; }
#define IMPLEMENT_EMPTY_VISIT_LEAVE(type)	bool DeltaASTSymbolFinder::VisitLeave (type* node) { return true; }
#define IMPLEMENT_EMPTY_VISIT_ENTER_AND_LEAVE(type)	\
	IMPLEMENT_EMPTY_VISIT_ENTER(type)				\
	IMPLEMENT_EMPTY_VISIT_LEAVE(type)

#define IMPLEMENT_EMPTY_VISIT(type)			bool DeltaASTSymbolFinder::Visit (type* node) { return true; }

//**********************************************************************

IMPLEMENT_EMPTY_VISIT_ENTER_AND_LEAVE(StmtsASTNode)
IMPLEMENT_EMPTY_VISIT_ENTER_AND_LEAVE(ExpressionListASTNode)
IMPLEMENT_EMPTY_VISIT_ENTER_AND_LEAVE(ArgListASTNode)
IMPLEMENT_EMPTY_VISIT_ENTER_AND_LEAVE(UnaryKwdASTNode)

IMPLEMENT_EMPTY_VISIT(LeafKwdASTNode)

IMPLEMENT_EMPTY_VISIT_ENTER_AND_LEAVE(WhileASTNode)
IMPLEMENT_EMPTY_VISIT_ENTER_AND_LEAVE(ForASTNode)
IMPLEMENT_EMPTY_VISIT_ENTER_AND_LEAVE(ForeachASTNode)
IMPLEMENT_EMPTY_VISIT_ENTER_AND_LEAVE(IfASTNode)
IMPLEMENT_EMPTY_VISIT_ENTER_AND_LEAVE(ElseASTNode)
IMPLEMENT_EMPTY_VISIT_ENTER_AND_LEAVE(CompoundASTNode)
IMPLEMENT_EMPTY_VISIT_ENTER_AND_LEAVE(TryASTNode)
IMPLEMENT_EMPTY_VISIT_ENTER_AND_LEAVE(TrapASTNode)
IMPLEMENT_EMPTY_VISIT_ENTER_AND_LEAVE(FunctionASTNode)

//**********************************************************************


bool DeltaASTSymbolFinder::Visit (FunctionNameASTNode* node)
{
	if (m_flags & SYMBFL_FUNC) {
		this->AppendSymbol(this->getText(node), _T('0'));
		this->AppendSymbolDeclaration(this->getText(node), node->GetRange(), _T('0'));
	}
	return true;
}

//**********************************************************************

IMPLEMENT_EMPTY_VISIT_ENTER_AND_LEAVE(TernaryOpASTNode)
IMPLEMENT_EMPTY_VISIT_ENTER_AND_LEAVE(PrefixOpASTNode)
IMPLEMENT_EMPTY_VISIT_ENTER_AND_LEAVE(SuffixOpASTNode)
IMPLEMENT_EMPTY_VISIT_ENTER_AND_LEAVE(BinaryOpASTNode)
IMPLEMENT_EMPTY_VISIT_ENTER_AND_LEAVE(UnaryOpASTNode)
IMPLEMENT_EMPTY_VISIT_ENTER_AND_LEAVE(CallASTNode)

//**********************************************************************

IMPLEMENT_EMPTY_VISIT(StringifiedIdASTNode)

bool DeltaASTSymbolFinder::Visit (StdLibfuncASTNode* node)
{
	if (m_flags & SYMBFL_FUNC)
		this->AppendSymbol(this->getText(node), _T('0'));
	return true;
}

bool DeltaASTSymbolFinder::Visit (LibfuncASTNode* node)
{
	if (m_flags & SYMBFL_FUNC)
		this->AppendSymbol(this->getText(node), _T('0'));
	return true;
}

bool DeltaASTSymbolFinder::Visit (AttributeASTNode* node)
{
	if (m_flags & SYMBFL_ATTR) {
		this->AppendSymbol(this->getText(node), _T('4'));
		//TODO not always a declaration...
		this->AppendSymbolDeclaration(this->getText(node), node->GetRange(), _T('4'));
	}
	return true;
}

bool DeltaASTSymbolFinder::Visit (VariableASTNode* node)
{
	if (m_flags & SYMBFL_VAR) {
		this->AppendSymbol(this->getText(node), _T('3'));
		//TODO not always a declaration...
		this->AppendSymbolDeclaration(this->getText(node), node->GetRange(), _T('3'));
	}
	return true;
}

IMPLEMENT_EMPTY_VISIT(ConstASTNode)

bool DeltaASTSymbolFinder::Visit (ArgASTNode* node)
{
	if (m_flags & SYMBFL_VAR) {
		this->AppendSymbol(this->getText(node), _T('1'));
		this->AppendSymbolDeclaration(this->getText(node), node->GetRange(), _T('1'));
	}
	return true;
}

//**********************************************************************

IMPLEMENT_EMPTY_VISIT_ENTER_AND_LEAVE(NewAttributeASTNode)
IMPLEMENT_EMPTY_VISIT_ENTER_AND_LEAVE(TableElemASTNode)
IMPLEMENT_EMPTY_VISIT_ENTER_AND_LEAVE(TableElemsASTNode)

//**********************************************************************

bool DeltaASTSymbolFinder::VisitEnter (TableIndexListASTNode* node)
{
	if (m_flags & SYMBFL_KEY) {
		String text = (m_flags & SYMBFL_ID) ? this->getIdKey(node) :
			this->getStringKey(node);
		if (!text.empty()) {
			this->AppendSymbol(text, _T('2'));
			this->AppendSymbolDeclaration(text, node->GetRange(), _T('2'));
		}
	}
	return true;
}

IMPLEMENT_EMPTY_VISIT_LEAVE(TableIndexListASTNode)

//**********************************************************************

IMPLEMENT_EMPTY_VISIT_ENTER_AND_LEAVE(TableConstructASTNode)
IMPLEMENT_EMPTY_VISIT_ENTER_AND_LEAVE(UsingASTNode)
IMPLEMENT_EMPTY_VISIT_ENTER_AND_LEAVE(QuotedElementsASTNode)

//**********************************************************************

bool DeltaASTSymbolFinder::Visit (TableConstKeyASTNode* node)
{
	if (m_flags & SYMBFL_KEY) {
		String text = (m_flags & SYMBFL_ID) ? this->getIdKey(node) :
			this->getStringKey(node);
		if (!text.empty())
			this->AppendSymbol(text, _T('2'));
	}
	return true;
}

//**********************************************************************

IMPLEMENT_EMPTY_VISIT_ENTER_AND_LEAVE(OtherStmtASTNode)

//**********************************************************************

void DeltaASTSymbolFinder::AppendSymbol (const String& text, String::value_type type)
{
	if (m_hint.empty() || !STRNCASECMP(text.c_str(), m_hint.c_str(), m_hint.size()))
		m_completionSet.insert(SymbEntry(text, type)).first->Type(type);
}

//**********************************************************************

void DeltaASTSymbolFinder::AppendSymbolDeclaration (const String& text, const Range& range, String::value_type type)
{
	if (m_hint == text)
		m_declarationList.push_back(SymbDeclEntry(text, range, type));
}

//**********************************************************************

const String DeltaASTSymbolFinder::getText (DeltaASTNode* node)
{
	const Range& range = node->GetRange();
	return m_window->GetTextRange((int) range.left, (int) range.right);
}

//**********************************************************************

const String DeltaASTSymbolFinder::getIdKey (DeltaASTNode* node)
{
	const String result = this->getText(node);
	if (IsAlphaChar(result[0]))
		return result;
	else if (result[0] == '.')
		return result.substr(1);
	else if (result[0] == '"' && IsAlphaChar(result[1])) {
		for (size_t i = 2; i < result.size() - 1; ++i)
			if (!IsIdChar(result[i]))
				return String();
		return result.substr(1, result.size() - 2);
	}
	return String();
}

//**********************************************************************

const String DeltaASTSymbolFinder::getStringKey (DeltaASTNode* node)
{
	const String result = this->getText(node);
	if (IsAlphaChar(result[0]))
		return _T("\"") + result + _T("\"");
	else if (result[0] == '.')
		return _T("\"") + result.substr(1) + _T("\"");
	else if (result[0] == '"')
		return result;
	return String();
}

////////////////////////////////////////////////////////////////////////

} // namespace delta
