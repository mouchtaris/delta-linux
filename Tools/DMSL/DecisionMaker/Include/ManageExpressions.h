/**
 *	ManageExpressions.h
 *
 *	-- DMSL Project --
 *
 *	Manage functions for the DMSL bison grammar expressions.
 *
 *	Giannis Lilis <lilis@ics.forth.gr>
 *	August 2006 (Version 1) - July 2007 (Version 2)
 */

#ifndef MANAGE_EXPRESSIONS_H
#define MANAGE_EXPRESSIONS_H

#include "Expressions.h"

//****************************

namespace dmsl {

	void Manage_dottedId_dottedId	(DecisionMaker *dm, const char **id, const char *id1, const char *id2);
	void Manage_dottedId_identifier	(DecisionMaker *dm, const char **id, const char *i);

	void Manage_identifier_string	(DecisionMaker *dm, const char **id, const char *i);
	void Manage_identifier_id		(DecisionMaker *dm, const char **id, const char *i);

	//****************************

	void Manage_expr_boolexpr	(DecisionMaker *dm, Expression **expr, Expression *e );
	void Manage_expr_arithexpr	(DecisionMaker *dm, Expression **expr, Expression *e );
	void Manage_expr_doubledot	(DecisionMaker *dm, Expression **expr, Expression *e1, Expression *e2);
	void Manage_expr_set		(DecisionMaker *dm, Expression **expr, ExprList *list);
	void Manage_expr_rangelist	(DecisionMaker *dm, Expression **expr, ExprList *list);

	//****************************

	void Manage_exprList_exprListNonEmpty	(DecisionMaker *dm, ExprList **list, ExprList *l);
	void Manage_exprList_empty				(DecisionMaker *dm, ExprList **list);

	//****************************

	void Manage_exprListNonEmpty_exprListNonEmpty	(DecisionMaker *dm, ExprList **list, ExprList *l, Expression *e);
	void Manage_exprListNonEmpty_expr				(DecisionMaker *dm, ExprList **list, Expression *e);

	//****************************

	void Manage_boolexpr_or				(DecisionMaker *dm, Expression **expr, Expression *e1, Expression *e2);
	void Manage_boolexpr_and			(DecisionMaker *dm, Expression **expr, Expression *e1, Expression *e2);
	void Manage_boolexpr_lessThan		(DecisionMaker *dm, Expression **expr, Expression *e1, Expression *e2);
	void Manage_boolexpr_lessEqual		(DecisionMaker *dm, Expression **expr, Expression *e1, Expression *e2);
	void Manage_boolexpr_greaterThan	(DecisionMaker *dm, Expression **expr, Expression *e1, Expression *e2);
	void Manage_boolexpr_greaterEqual	(DecisionMaker *dm, Expression **expr, Expression *e1, Expression *e2);
	void Manage_boolexpr_equal			(DecisionMaker *dm, Expression **expr, Expression *e1, Expression *e2);
	void Manage_boolexpr_notEqual		(DecisionMaker *dm, Expression **expr, Expression *e1, Expression *e2);
	void Manage_boolexpr_in				(DecisionMaker *dm, Expression **expr, Expression *e1,	Expression *e2);
	void Manage_boolexpr_not			(DecisionMaker *dm, Expression **expr, Expression *e  );
	void Manage_boolexpr_isactive		(DecisionMaker *dm, Expression **expr, ExprList *list );
	void Manage_boolexpr_hasattr		(DecisionMaker *dm, Expression **expr, ExprList *list );
	void Manage_boolexpr_cansupport		(DecisionMaker *dm, Expression **expr, ExprList *list );
	void Manage_boolexpr_bool			(DecisionMaker *dm, Expression **expr, bool		   val);
	void Manage_boolexpr_parenthesis	(DecisionMaker *dm, Expression **expr, Expression *e  );
	void Manage_boolexpr_dottedId		(DecisionMaker *dm, Expression **expr, const char *val);
	void Manage_boolexpr_params			(DecisionMaker *dm, Expression **expr, const char *id );

	//****************************

	void Manage_arithexpr_add		(DecisionMaker *dm, Expression **expr, Expression *e1, Expression *e2);
	void Manage_arithexpr_sub		(DecisionMaker *dm, Expression **expr, Expression *e1, Expression *e2);
	void Manage_arithexpr_mul		(DecisionMaker *dm, Expression **expr, Expression *e1, Expression *e2);
	void Manage_arithexpr_div		(DecisionMaker *dm, Expression **expr, Expression *e1, Expression *e2);
	void Manage_arithexpr_mod		(DecisionMaker *dm, Expression **expr, Expression *e1, Expression *e2);
	void Manage_arithexpr_uplus		(DecisionMaker *dm, Expression **expr, Expression *e );
	void Manage_arithexpr_uminus	(DecisionMaker *dm, Expression **expr, Expression *e );
	void Manage_arithexpr_tonumber	(DecisionMaker *dm, Expression **expr, ExprList *list);
	void Manage_arithexpr_random	(DecisionMaker *dm, Expression **expr, ExprList *list);
	void Manage_arithexpr_int		(DecisionMaker *dm, Expression **expr, int		  val);
	void Manage_arithexpr_real		(DecisionMaker *dm, Expression **expr, double	  val);
}

#endif