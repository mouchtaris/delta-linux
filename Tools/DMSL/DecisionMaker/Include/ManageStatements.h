/**
 *	ManageStatements.h
 *
 *	-- DMSL Project --
 *
 *	Manage functions for the DMSL bison grammar statements.
 *
 *	Giannis Lilis <lilis@ics.forth.gr>
 *	August 2006 (Version 1) - July 2007 (Version 2)
 */

#ifndef MANAGE_STATEMENTS_H
#define MANAGE_STATEMENTS_H

#include "Statements.h"

//****************************

namespace dmsl {

	void Manage_logic_declList	(DecisionMaker *dm, Statement **stmt, StmtList *l);
	void Manage_logic_empty		(DecisionMaker *dm, Statement **stmt);

	//****************************

	void Manage_declList_declList	(DecisionMaker *dm, StmtList **list, StmtList *l, Statement *s);
	void Manage_declList_decl		(DecisionMaker *dm, StmtList **list, Statement *s);

	//****************************

	void Manage_decl_component	(DecisionMaker *dm, Statement **stmt, const char *id, Statement *s );
	void Manage_decl_stereotype	(DecisionMaker *dm, Statement **stmt, const char *id, Expression *e);
	void Manage_decl_define		(DecisionMaker *dm, Statement **stmt, const char *id, Expression *e);

	//****************************

	void Manage_compound(DecisionMaker *dm, Statement **stmt, StmtList *l);

	//****************************

	void Manage_stmts_stmtList	(DecisionMaker *dm, StmtList **list, StmtList *l);
	void Manage_stmts_empty		(DecisionMaker *dm, StmtList **list);

	//****************************

	void Manage_stmtList_stmtList	(DecisionMaker *dm, StmtList **list, StmtList *l, Statement *s);
	void Manage_stmtList_stmt		(DecisionMaker *dm, StmtList **list, Statement *s);

	//****************************

	void Manage_stmt_if			(DecisionMaker *dm, Statement **stmt, Expression *e, Statement *s1, Statement *s2);
	void Manage_stmt_case		(DecisionMaker *dm, Statement **stmt, Expression *e, StmtList  *l,	Statement *s );
	void Manage_stmt_activate	(DecisionMaker *dm, Statement **stmt, Expression *e );
	void Manage_stmt_cancel		(DecisionMaker *dm, Statement **stmt, Expression *e );
	void Manage_stmt_evaluate	(DecisionMaker *dm, Statement **stmt, Expression *e );
	void Manage_stmt_library	(DecisionMaker *dm, Statement **stmt, ExprList *list);
	void Manage_stmt_compound	(DecisionMaker *dm, Statement **stmt, Statement  *s );

	//****************************

	void Manage_elseSt_else	(DecisionMaker *dm, Statement **stmt, Statement *s);
	void Manage_elseSt_empty(DecisionMaker *dm, Statement **stmt);

	//****************************

	void Manage_whenList_whenListNonEmpty	(DecisionMaker *dm, StmtList **list, StmtList *l);
	void Manage_whenList_empty				(DecisionMaker *dm, StmtList **list);

	//****************************

	void Manage_whenListNonEmpty_whenListNonEmpty	(DecisionMaker *dm, StmtList **list, StmtList *l, Statement *s);
	void Manage_whenListNonEmpty_when				(DecisionMaker *dm, StmtList **list, Statement *s);

	//****************************

	void Manage_when(DecisionMaker *dm, Statement **stmt, Expression *e, Statement *s);

	//****************************

	void Manage_otherwise_otherwise	(DecisionMaker *dm, Statement **stmt, Statement *s);
	void Manage_otherwise_empty		(DecisionMaker *dm, Statement **stmt);
}

#endif