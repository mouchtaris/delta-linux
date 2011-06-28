/**
 *	ProgramDescription.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef DELTA_PROGRAMDESCRIPTION_H
#define DELTA_PROGRAMDESCRIPTION_H

#include "Common.h"
#include "RangeList.h"
#include "DeltaASTFwdDecls.h"
#include "DeltaAST.h"
#include "ucallbacks.h"

namespace delta {

////////////////////////////////////////////////////////////////////////

struct ErrorRange : public Range {
	ErrorRange (uint left = 0, uint right = 0, const String& msg = String()) :
		Range(left, right), errorMsg(msg) {}

	String errorMsg;
};

////////////////////////////////////////////////////////////////////////

class ProgramDescription {

	/////////////////////////////////////////////////////////
	// i-sense required API.

	public:
	typedef void (*OnFocusChanged)(DeltaASTNode* focus, void* closure);
	typedef void (*OnReplaced)(DeltaASTNode* old, DeltaASTNode* replacement, void* closure);
	typedef void (*OnDeleted)(DeltaASTNode* deleted, void* closure);
	typedef void (*OnInserted)(DeltaASTNode* parent, DeltaASTNode* after, DeltaASTNode* inserted, void* closure);
	
	//**************************

	protected:
	ucallbackwithclosure<OnFocusChanged>							onFocusChanged;
	ucallbackwithclosure<OnReplaced>								onReplaced;
	ucallbackwithclosure<OnDeleted>									onDeleted;
	ucallbackwithclosure<OnInserted>								onInserted;

	public:
	void		SetOnFocusChanged (OnFocusChanged f, void* c = (void*) 0)
					{ onFocusChanged.set(f,c); } 
	void		ResetOnFocusChanged (void)
					{ onFocusChanged.reset(); } 
	void		SetOnReplaced (OnReplaced f, void* c = (void*) 0)
					{ onReplaced.set(f,c); } 
	void		ResetOnReplaced (void)
					{ onReplaced.reset(); } 
	void		SetOnDeleted (OnDeleted f, void* c = (void*) 0)
					{ onDeleted.set(f,c); } 
	void		ResetOnDeleted (void)
					{ onDeleted.reset(); } 
	void		SetOnInserted (OnInserted f, void* c = (void*) 0)
					{ onInserted.set(f,c); } 
	void		ResetOnInserted (void)
					{ onInserted.reset(); } 

	//**************************

	protected:
	void		NotifyFocusChanged (DeltaASTNode* focus)
					{ if (onFocusChanged) onFocusChanged(focus); }
	void		NotifyReplaced (DeltaASTNode* old, DeltaASTNode* replacement)
					{ if (onReplaced) onReplaced(old, replacement); }
	void		NotifyDeleted (DeltaASTNode* deleted)
					{ if (onDeleted) onDeleted(deleted); }
	void		NotifyInserted (DeltaASTNode* parent, DeltaASTNode* after, DeltaASTNode* inserted)
					{ if (onInserted) onInserted(parent, after, inserted); }

	/////////////////////////////////////////////////////////

public:
	typedef RangeListT<Range>				RangeList;
	typedef RangeListT<ErrorRange>			ErrorRangeList;
	typedef std::pair<Range, StmtsASTNode*> Slice;

	////////////////////////////////////////////////////////////////////

	class View {
	public:
		View (ProgramDescription* progDesc) : pm_progDesc(progDesc) {}
		virtual ~View (void) {}

		virtual void OnSetAST (StmtsASTNode* ast) = 0;

		virtual void OnMerge (
				StmtsASTNode*			parent,
				StmtsASTNode::Iterator	end,
				StmtsASTNode*			src
			) = 0;
		virtual void OnRemove (StmtsASTNode* parent, const Range& range) = 0;
		virtual void OnRemove (DeltaASTNode* node) = 0;
		virtual void OnRefreshChildren (DeltaASTNode* node) = 0;

	protected:
		ProgramDescription* pm_progDesc;
	};

	////////////////////////////////////////////////////////////////////

	ProgramDescription (void);
	~ProgramDescription (void);

	//******************************************************************

	void AddView (View* view);
	void RemoveView (View* view);
	void ClearViews (void);

	//******************************************************************

	const Slice	GetAffectedSliceAfterAdd (const Range& range);
	const Slice	GetAffectedSliceAfterRemove (const Range& range);

	void Merge (const Slice& slice, ProgramDescription& other);
	void Remove (const Slice& slice);
	void ReplaceNode (DeltaASTNode* node, DeltaASTNode* replacement);

	//******************************************************************

	void SetAST (StmtsASTNode* ast);

	// Added for i-sense, partial selective parsing (AS).
	void SetStmtAST (DeltaASTNode* stmt) { m_stmtAst = stmt; }
	void SetNamespaceAST (DeltaASTNode* ns) { m_namespaceAst = ns;  }
	void SetFunctionObjectAST (DeltaASTNode* func) { m_funcAst = func; }
	void SetTableObjectAST (DeltaASTNode* table) { m_tableAst = table; }

	void AddParseError (const Range& range, const String& msg);
	void AddSemanticError (const Range& range, const String& msg);
	void AddComment (const Range& range);
	void AddCpp (const Range& range);
	void AddNotParsed (const Range& range);

	void ClearAST (void);
	void ClearParseErrors (void);
	void ClearSemanticErrors (void);
	void ClearComments (void);
	void ClearCpp (void);
	void ClearNotParsed (void);
	void ClearAll (void);

	StmtsASTNode*	GetAST (void) { return m_programAst; }
	// Added for i-sense (AS).
	DeltaASTNode*	GetStmtAST (void) { return m_stmtAst; }
	DeltaASTNode*	GetFuncAST (void) { return m_funcAst; }
	DeltaASTNode*	GetTableAST (void) { return m_tableAst; }
	DeltaASTNode*	GetNamespaceAST (void) { return m_namespaceAst; }

	ErrorRangeList&	GetParseErrors (void) { return m_parseErrors; }
	ErrorRangeList&	GetSemanticErrors (void) { return m_semanticErrors; }
	RangeList&		GetComments (void) { return m_comments; }
	RangeList&		GetCpp (void) { return m_cpp; }
	RangeList&		GetNotParsed (void) { return m_notParsed; }

	void UpdateASTPositions (uint atPos, int offset);
	void UpdateParseErrorPositions (uint atPos, int offset);
	void UpdateSemanticErrorPositions (uint atPos, int offset);
	void UpdateCommentPositions (uint atPos, int offset);
	void UpdateCppPositions (uint atPos, int offset);
	void UpdateNotParsedPositions (uint atPos, int offset);
	void UpdateAllPositions (uint atPos, int offset);

	// Added for i-sense (AS).
	DeltaASTNode*		GetCursorNode (uint atPos);
	void				UpdadeFocusNode (uint atPos)
							{ m_focusNode = GetCursorNode(atPos); }
	const std::string	GetFocusNodeInformation (void) const;

	DeltaASTNode*		GetNode (uint atPos);
	const ErrorRange&	GetParseError (uint atPos);
	const ErrorRange&	GetSemanticError (uint atPos);
	const Range&		GetComment (uint atPos);
	const Range&		GetCpp (uint atPos);
	const Range&		GetNotParsed (uint atPos);

	DeltaASTNode* GetNodeAbove (uint pos);

#ifndef NDEBUG
	static void Test (void);
#endif

private:
	typedef std::pair<DeltaASTNode*, DeltaASTNode*> StmtsResult;
	typedef std::vector<View*>						Views;

	inline DeltaASTNode*	getTopLevelStmtInRange (const Range& range);
	inline DeltaASTNode*	getCurrentStmt (StmtsASTNode* node, uint pos);
	inline DeltaASTNode*	getNextStmt (StmtsASTNode* node, uint pos);
	inline DeltaASTNode*	getPrevStmt (StmtsASTNode* node, uint pos);

	inline void removeStmts (StmtsASTNode* node, const Range& range);
	inline void removeNode (DeltaASTNode* node);
	inline void replaceNode (DeltaASTNode* node, DeltaASTNode* replacement);
	inline void	mergeStmts (StmtsASTNode* src, StmtsASTNode* dest);

	StmtsASTNode*	m_programAst;

	// For partial selective parsing (AS).
	DeltaASTNode*	m_stmtAst;
	DeltaASTNode*	m_funcAst;
	DeltaASTNode*	m_tableAst;
	DeltaASTNode*	m_namespaceAst;
	DeltaASTNode*	m_focusNode;

	ErrorRangeList	m_parseErrors;
	ErrorRangeList	m_semanticErrors;
	RangeList		m_comments;
	RangeList		m_cpp;
	RangeList		m_notParsed;

	Views			m_views;
};

////////////////////////////////////////////////////////////////////////

} // namespace delta

#endif // DELTA_PROGRAMDESCRIPTION_H
