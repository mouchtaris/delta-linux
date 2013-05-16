/**
 *	DeltaASTSymbolFinder.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef DELTA_DELTAASTSYMBOLFINDER_H
#define DELTA_DELTAASTSYMBOLFINDER_H

#include "Common.h"
#include "DeltaParser/DeltaASTVisitor.h"
#include <ctype.h>
#include <set>

namespace editor {
	class EditorWindow;
}

namespace delta {

////////////////////////////////////////////////////////////////////////

enum SymbolFlags {
	SYMBFL_KEY	= 1 << 0,
	SYMBFL_ID	= 1 << 1,
	SYMBFL_FUNC = 1 << 2,
	SYMBFL_VAR	= 1 << 3,
	SYMBFL_ATTR	= 1 << 4
};

////////////////////////////////////////////////////////////////////////

class DeltaASTSymbolFinder : public DeltaASTVisitor {
public:
	DeltaASTSymbolFinder (
		editor::EditorWindow*	window,
		const String&			hint,
		long					flags
	);
	~DeltaASTSymbolFinder (void);

	const String GetCompletionList (void) const;
	const String GetDeclarationList (void) const;

	//******************************************************************
	// Utility functions
	//
	inline static bool IsIdChar (int c) { return isalnum(c) != 0; }
	inline static bool IsAlphaChar (int c) { return isalpha(c) != 0; }

protected:
	virtual bool VisitEnter (StmtsASTNode* node);
	virtual bool VisitLeave (StmtsASTNode* node);

	virtual bool VisitEnter (ExpressionListASTNode* node);
	virtual bool VisitLeave (ExpressionListASTNode* node);

	virtual bool VisitEnter (ArgListASTNode* node);
	virtual bool VisitLeave (ArgListASTNode* node);

	virtual bool VisitEnter (UnaryKwdASTNode* node);
	virtual bool VisitLeave (UnaryKwdASTNode* node);

	virtual bool Visit (LeafKwdASTNode* node);

	virtual bool VisitEnter (WhileASTNode* node);
	virtual bool VisitLeave (WhileASTNode* node);

	virtual bool VisitEnter (ForASTNode* node);
	virtual bool VisitLeave (ForASTNode* node);

	virtual bool VisitEnter (ForeachASTNode* node);
	virtual bool VisitLeave (ForeachASTNode* node);

	virtual bool VisitEnter (IfASTNode* node);
	virtual bool VisitLeave (IfASTNode* node);

	virtual bool VisitEnter (ElseASTNode* node);
	virtual bool VisitLeave (ElseASTNode* node);	

	virtual bool VisitEnter (CompoundASTNode* node);
	virtual bool VisitLeave (CompoundASTNode* node);

	virtual bool VisitEnter (TryASTNode* node);
	virtual bool VisitLeave (TryASTNode* node);

	virtual bool VisitEnter (TrapASTNode* node);
	virtual bool VisitLeave (TrapASTNode* node);

	virtual bool VisitEnter (FunctionASTNode* node);
	virtual bool VisitLeave (FunctionASTNode* node);

	virtual bool Visit (FunctionNameASTNode* node);

	virtual bool VisitEnter (TernaryOpASTNode* node);
	virtual bool VisitLeave (TernaryOpASTNode* node);

	virtual bool VisitEnter (PrefixOpASTNode* node);
	virtual bool VisitLeave (PrefixOpASTNode* node);

	virtual bool VisitEnter (SuffixOpASTNode* node);
	virtual bool VisitLeave (SuffixOpASTNode* node);

	virtual bool VisitEnter (BinaryOpASTNode* node);
	virtual bool VisitLeave (BinaryOpASTNode* node);

	virtual bool VisitEnter (UnaryOpASTNode* node);
	virtual bool VisitLeave (UnaryOpASTNode* node);

	virtual bool VisitEnter (CallASTNode* node);
	virtual bool VisitLeave (CallASTNode* node);

	virtual bool Visit (StringifiedIdASTNode* node);
	virtual bool Visit (StdLibfuncASTNode* node);
	virtual bool Visit (LibfuncASTNode* node);
	virtual bool Visit (AttributeASTNode* node);
	virtual bool Visit (VariableASTNode* node);
	virtual bool Visit (ConstASTNode* node);
	virtual bool Visit (ArgASTNode* node);

	virtual bool VisitEnter (NewAttributeASTNode* node);
	virtual bool VisitLeave (NewAttributeASTNode* node);

	virtual bool VisitEnter (TableElemASTNode* node);
	virtual bool VisitLeave (TableElemASTNode* node);

	virtual bool VisitEnter (TableElemsASTNode* node);
	virtual bool VisitLeave (TableElemsASTNode* node);

	virtual bool VisitEnter (TableIndexListASTNode* node);
	virtual bool VisitLeave (TableIndexListASTNode* node);

	virtual bool VisitEnter (TableConstructASTNode* node);
	virtual bool VisitLeave (TableConstructASTNode* node);

	virtual bool Visit (TableConstKeyASTNode* node);

	virtual bool VisitEnter (OtherStmtASTNode* node);
	virtual bool VisitLeave (OtherStmtASTNode* node);

	virtual bool VisitEnter (UsingASTNode* node);
	virtual bool VisitLeave (UsingASTNode* node);
	
	virtual bool VisitEnter (QuotedElementsASTNode* node);
	virtual bool VisitLeave (QuotedElementsASTNode* node);

private:
	struct SymbEntry {
		SymbEntry (const String& symbol, tchar type) :
			symbol(symbol), type(type) {}

		bool operator< (const SymbEntry& e) const { return symbol < e.symbol; }
		bool operator== (const SymbEntry& e) const { return symbol == e.symbol; }

		void Type (tchar t) const { type = std::min(type, t); }

		String						symbol;
		mutable tchar	type;
	};
	typedef std::set<const SymbEntry> StringSet;

	struct SymbDeclEntry {
		SymbDeclEntry (const String& symbol, const Range& range, tchar type) :
			symbol(symbol), range(range), type(type) {}

		void Type (tchar t) const { type = std::min(type, t); }

		String						symbol;
		Range						range;
		mutable tchar	type;
	};
	typedef std::list<const SymbDeclEntry> StringList;

	inline void			AppendSymbol (const String& text, tchar type);
	inline void			AppendSymbolDeclaration (const String& text, const Range& range, tchar type);
	inline const String	getText (DeltaASTNode* node);
	inline const String getIdKey (DeltaASTNode* node);
	inline const String getStringKey (DeltaASTNode* node);

	editor::EditorWindow*	m_window;
	long					m_flags;
	const String			m_hint;
	StringSet				m_completionSet;
	StringList				m_declarationList;
};

////////////////////////////////////////////////////////////////////////

} // namespace delta

#endif // DELTA_DELTAASTSYMBOLFINDER_H
