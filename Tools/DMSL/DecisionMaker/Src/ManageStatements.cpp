/**
 *	ManageStatements.cpp
 *
 *	-- DMSL Project --
 *
 *	Implementation of the manage functions for the 
 *  DMSL bison grammar statements.
 *
 *	Giannis Lilis <lilis@ics.forth.gr>
 *	August 2006 (Version 1) - July 2007 (Version 2)
 */

#include "ManageStatements.h"
#include "ManageFunctionErrorHandling.h"
#include "LibfuncExpressions.h"
#include "DecisionMaker.h"
#include "SymbolTable.h"
#include "util.h"

extern int DMSL_yylineno;

namespace dmsl {

	void Manage_logic_declList	(DecisionMaker *dm, Statement **stmt, StmtList *l) {
		RETURN_ON_ERROR_UNARY(stmt, l);
		*stmt = new LogicStatement(dm, l);
	}

	void Manage_logic_empty		(DecisionMaker *dm, Statement **stmt)	{
		StmtList *l = new StmtList;
		Statement::AddList(dm, l);
		*stmt = new LogicStatement(dm, l);
	}

	//****************************

	void Manage_declList_declList	(DecisionMaker *dm, StmtList **list, StmtList *l, Statement *s) {
		RETURN_ON_ERROR_LIST(dm, list, l, s);
		l->push_back(s);
		*list = l;
	}
	void Manage_declList_decl		(DecisionMaker *dm, StmtList **list, Statement *s) {
		RETURN_ON_ERROR_UNARY(list, s);
		*list = new StmtList(1, s);
		Statement::AddList(dm, *list);
	}

	//****************************

	void Manage_decl_component	(DecisionMaker *dm, Statement **stmt, const char *id, Statement *s) {
		RETURN_ON_ERROR_UNARY(stmt, s);
		*stmt = new ComponentStatement(dm, id, s);
		if(!dm->GetSymbolTable().AddComponent(id, *stmt)) {
			delete *stmt;
			SET_ERROR_WITH_TWO_ARGS(dm, "SymbolRedeclaration", DMSL_yylineno, id);
			*stmt = (Statement *) 0;
		}
		else
			delete[] id;	//delete id only upon normal execution; upon error bison will clean it up
	}

	void Manage_decl_stereotype	(DecisionMaker *dm, Statement **stmt, const char *id, Expression *e) {
		RETURN_ON_ERROR_UNARY(stmt, e);
		if(!dm->GetSymbolTable().AddStereotype(id, e)) {
			delete e;
			SET_ERROR_WITH_TWO_ARGS(dm, "SymbolRedeclaration", DMSL_yylineno, id);
			*stmt = (Statement *) 0;
		}
		else if(!e->IsBool()) {
			delete e;
			SET_ERROR_WITH_THREE_ARGS(dm, "ExpressionExpected", DMSL_yylineno, "stereotype", "boolean");
			*stmt = (Statement *) 0;
		}
		else {
			*stmt = new StereotypeStatement(dm, id, e);
			delete[] id;	//delete id only upon normal execution; upon error bison will clean it up
		}
	}

	void Manage_decl_define	(DecisionMaker *dm, Statement **stmt, const char *id, Expression *e) {
		RETURN_ON_ERROR_UNARY(stmt, e);
		if(!dm->GetSymbolTable().AddDefine(id, e)) {
			delete e;
			SET_ERROR_WITH_TWO_ARGS(dm, "SymbolRedeclaration", DMSL_yylineno, id);
			*stmt = (Statement *) 0;
		}
		else {
			*stmt = new DefineStatement(dm, id, e);
			delete[] id;	//delete id only upon normal execution; upon error bison will clean it up
		}
	}

	//****************************

	void Manage_compound(DecisionMaker *dm, Statement **stmt, StmtList *l) {
		RETURN_ON_ERROR_UNARY(stmt, l);
		*stmt = new CompoundStatement(dm, l);
	}

	//****************************

	void Manage_stmts_stmtList	(DecisionMaker *, StmtList **list, StmtList *l)	{ *list = l; }

	void Manage_stmts_empty		(DecisionMaker *dm, StmtList **list) {
		*list = new StmtList;
		Statement::AddList(dm, *list);
	}

	//****************************

	void Manage_stmtList_stmtList	(DecisionMaker *dm, StmtList **list, StmtList *l, Statement *s) {
		RETURN_ON_ERROR_LIST(dm, list, l, s);
		l->push_back(s);
		*list = l;
	}
	void Manage_stmtList_stmt		(DecisionMaker *dm, StmtList **list, Statement *s) {
		RETURN_ON_ERROR_UNARY(list, s);
		*list = new StmtList(1, s);
		Statement::AddList(dm, *list);
	}

	//****************************

	void Manage_stmt_if(DecisionMaker *dm, Statement **stmt, Expression *e, Statement *s1, Statement *s2) {
		RETURN_ON_ERROR_TRIADIC(stmt, e, s1, s2);
		if(!e->IsBool()) {
			SET_ERROR_WITH_THREE_ARGS(dm, "ExpressionExpected", DMSL_yylineno, "if", "boolean");
			delete e;	delete s1;	delete s2;
			*stmt = (Statement *) 0;
		}
		else
			*stmt = new IfStatement(dm, e, s1, s2);
	}

	void Manage_stmt_case(DecisionMaker *dm, Statement **stmt, Expression *e, StmtList *l, Statement *s) {
		RETURN_ON_ERROR_TRIADIC(stmt, e, l, s);
		if(l->empty())
			APPEND_WARNING_WITH_TWO_ARGS(
				dm,
				"CaseStatementNoClauses",
				DMSL_yylineno,
				s->GetType() == Statement::StmtTypeEmpty ? "case or otherwise" : "case"
			);
		*stmt = new CaseStatement(dm, e, l, s);
	}

	template<class T>
	void ManageAction(DecisionMaker *dm, Statement **stmt, Expression *e, const std::string& action) {
		RETURN_ON_ERROR_UNARY(stmt, e);
		if(!e->IsString()) {
			SET_ERROR_WITH_THREE_ARGS(dm, "ExpressionExpected", DMSL_yylineno, action.c_str(), "string");
			delete e;
			*stmt = (Statement *) 0;
		}
		else
			*stmt = new T(dm, e);
	}

	template<>
	void ManageAction<EvaluateStatement>(DecisionMaker *dm, Statement **stmt, Expression *e, const std::string& action) {
		RETURN_ON_ERROR_UNARY(stmt, e);
		if(!e->IsString()) {
			SET_ERROR_WITH_THREE_ARGS(dm, "ExpressionExpected", DMSL_yylineno, action.c_str(), "string");
			delete e;
			*stmt = (Statement *) 0;
		}
		else if(!e->IsConst()) {
			SET_ERROR_WITH_THREE_ARGS(dm, "ExpressionExpected", DMSL_yylineno, action.c_str(), "const");
			delete e;
			*stmt = (Statement *) 0;
		}
		else
			*stmt = new EvaluateStatement(dm, e);
	}

	void Manage_stmt_activate (DecisionMaker *dm, Statement **stmt, Expression *e)
		{ ManageAction<ActivateStatement>(dm, stmt, e, "activate"); }

	void Manage_stmt_cancel	(DecisionMaker *dm, Statement **stmt, Expression *e)
		{ ManageAction<CancelStatement>(dm, stmt, e, "cancel"); }

	void Manage_stmt_evaluate (DecisionMaker *dm, Statement **stmt, Expression *e)
		{ ManageAction<EvaluateStatement>(dm, stmt, e, "evaluate"); }

	void Manage_stmt_library	(DecisionMaker *dm, Statement **stmt, ExprList *list) {
		RETURN_ON_ERROR_UNARY(stmt, list);
		const char *name = "library";
		bool error = false;
		if(list->size() == 1) {
			if(!list->front()->IsString()) {
				SET_ERROR_WITH_FOUR_ARGS(dm, "LibfuncInvalidArg", DMSL_yylineno, name, 1, "string");
				error = true;
			}
		}
		else if(list->size() == 2) {
			if(!list->front()->IsString() || !list->back()->IsString()) {
				int argNo = !list->front()->IsString() ? 1 : 2;
				SET_ERROR_WITH_FOUR_ARGS(dm, "LibfuncInvalidArg", DMSL_yylineno, name, argNo, "string");
				error = true;
			}
		}
		else {
			SET_ERROR_WITH_THREE_ARGS(dm, "LibfuncIncorrectArgCount", DMSL_yylineno, name, list->size());
			error = true;
		}
		if(error) {
			util::destroyContainer<ExprList>(list);
			Expression::RemoveList(dm, list);
			*stmt = (Statement *) 0;
		}
		else
			*stmt = new ExpressionStatement(dm, new LibraryExpression(dm, list));
	}

	void Manage_stmt_compound	(DecisionMaker *, Statement **stmt, Statement  *s) { *stmt = s; }

	//****************************

	void Manage_elseSt_else	(DecisionMaker *, Statement **stmt, Statement *s)	{ *stmt = s;						}
	void Manage_elseSt_empty(DecisionMaker *dm, Statement **stmt)				{ *stmt = new EmptyStatement(dm);	}

	//****************************

	void Manage_whenList_whenListNonEmpty	(DecisionMaker *dm, StmtList **list, StmtList *l) { *list = l; }
	void Manage_whenList_empty				(DecisionMaker *dm, StmtList **list)
		{ Statement::AddList(dm, *list = new StmtList); }

	//****************************

	void Manage_whenListNonEmpty_whenListNonEmpty	(DecisionMaker *dm, StmtList **list, StmtList *l, Statement *s) {
		RETURN_ON_ERROR_LIST(dm, list, l, s);
		l->push_back(s);
		*list = l;
	}
	void Manage_whenListNonEmpty_when(DecisionMaker *dm, StmtList **list, Statement *s) {
		RETURN_ON_ERROR_UNARY(list, s);
		Statement::AddList(dm, *list = new StmtList(1, s));
	}

	//****************************

	void Manage_when(DecisionMaker *dm, Statement **stmt, Expression *e, Statement *s) {
		RETURN_ON_ERROR_BINARY(stmt, e, s);
		*stmt = new WhenStatement(dm, e, s);
	}

	//****************************

	void Manage_otherwise_otherwise	(DecisionMaker *, Statement **stmt, Statement *s)	{ *stmt = s;						}
	void Manage_otherwise_empty		(DecisionMaker *dm, Statement **stmt)				{ *stmt = new EmptyStatement(dm);	}
}