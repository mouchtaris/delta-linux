/**
 *	ManageExpressions.cpp
 *
 *	-- DMSL Project --
 *
 *	Implementation of the manage functions for the 
 *  DMSL bison grammar expressions.
 *
 *	Giannis Lilis <lilis@ics.forth.gr>
 *	August 2006 (Version 1) - July 2007 (Version 2)
 */

#include "ManageExpressions.h"
#include "ManageFunctionErrorHandling.h"

#include <assert.h>
#include <string.h>

#include "DecisionMaker.h"
#include "BinaryExpressions.h"
#include "LibfuncExpressions.h"
#include "UnaryExpressions.h"
#include "util.h"

extern int DMSL_yylineno;

namespace dmsl {

	/////////////////////////////////
	// Some template helper functions
	//
	template<class T>
	void ManageLibfuncExpressions(DecisionMaker *dm, Expression **expr, ExprList *list, const std::string& name) {
		RETURN_ON_ERROR_UNARY(expr, list);
		bool error = false;
		if(list->size() != 1) {
			SET_ERROR_WITH_THREE_ARGS(dm, "LibfuncIncorrectArgCount", DMSL_yylineno, name.c_str(), list->size());
			error = true;
		}
		else if(!list->front()->IsConst()) {
			SET_ERROR_WITH_FOUR_ARGS(dm, "LibfuncInvalidArg", DMSL_yylineno, name.c_str(), 1, "const");
			error = true;
		}
		else if(!list->front()->IsString()) {
			SET_ERROR_WITH_FOUR_ARGS(dm, "LibfuncInvalidArg", DMSL_yylineno, name.c_str(), 1, "string");
			error = true;
		}
		if(error) {
			util::destroyContainer<ExprList>(list);
			Expression::RemoveList(dm, list);
			*expr = (Expression *) 0;
		}
		else
			*expr = new T(dm, list);	
	}
	
	template<>
	void ManageLibfuncExpressions<ToNumberExpression>(DecisionMaker *dm, Expression **expr, ExprList *list, const std::string& name) {
		RETURN_ON_ERROR_UNARY(expr, list);
		bool error = false;
		if(list->size() != 1) {
			SET_ERROR_WITH_THREE_ARGS(dm, "LibfuncIncorrectArgCount", DMSL_yylineno, name.c_str(), list->size());
			error = true;
		}
		else if(!list->front()->IsString()) {
			SET_ERROR_WITH_FOUR_ARGS(dm, "LibfuncInvalidArg", DMSL_yylineno, name.c_str(), 1, "string");
			error = true;
		}
		if(error) {
			util::destroyContainer<ExprList>(list);
			Expression::RemoveList(dm, list);
			*expr = (Expression *) 0;
		}
		else
			*expr = new ToNumberExpression(dm, list);
	}

	template<>
	void ManageLibfuncExpressions<RandomExpression>(DecisionMaker *dm, Expression **expr, ExprList *list, const std::string& name) {
		RETURN_ON_ERROR_UNARY(expr, list);
		bool error = false;
		if(list->size() > 1) {
			SET_ERROR_WITH_THREE_ARGS(dm, "LibfuncIncorrectArgCount", DMSL_yylineno, name.c_str(), list->size());
			error = true;
		}
		else if(list->size() == 1) {
			if(!list->front()->IsConst()) {
				SET_ERROR_WITH_FOUR_ARGS(dm, "LibfuncInvalidArg", DMSL_yylineno, name.c_str(), 1, "const");
				error = true;
			}
			else if(!list->front()->IsInteger()) {
				SET_ERROR_WITH_FOUR_ARGS(dm, "LibfuncInvalidArg", DMSL_yylineno, name.c_str(), 1, "integer");
				error = true;
			}
		}
		if(error) {
			util::destroyContainer<ExprList>(list);
			Expression::RemoveList(dm, list);
			*expr = (Expression *) 0;
		}
		else
			*expr = new RandomExpression(dm, list);
	}

	template<>
	void ManageLibfuncExpressions<CanSupportExpression>(DecisionMaker *dm, Expression **expr, ExprList *list, const std::string& name) {
		RETURN_ON_ERROR_UNARY(expr, list);
		bool error = false;
		if(list->size() != 2) {
			SET_ERROR_WITH_THREE_ARGS(dm, "LibfuncIncorrectArgCount", DMSL_yylineno, name.c_str(), list->size());
			error = true;
		}
		else if(!list->front()->IsString() || !list->back()->IsString()) {
			int argNo = !list->front()->IsString() ? 1 : 2;
			SET_ERROR_WITH_FOUR_ARGS(dm, "LibfuncInvalidArg", DMSL_yylineno, name.c_str(), argNo, "string");
			error = true;
		}
		if(error) {
			util::destroyContainer<ExprList>(list);
			Expression::RemoveList(dm, list);
			*expr = (Expression *) 0;
		}
		else
			*expr = new CanSupportExpression(dm, list);
	}

	//************************************************************

	template<class T>
	void ManageUnaryArithmeticExpressions(DecisionMaker *dm, Expression **expr, Expression *e, const std::string& op) {
		RETURN_ON_ERROR_UNARY(expr, e);
		if(!e->IsNumber()) {
			SET_ERROR_WITH_THREE_ARGS(dm, "UnaryOperatorTypeMismatch", DMSL_yylineno, op.c_str(), "arithmetic");
			delete e;
			*expr = (Expression *) 0;
		}
		else
			*expr = new T(dm, e);
	}

	///////////////////////////////////////////////////
	// Helper functions for the binary expresions
	//
	namespace BinaryExpressions {
		//****************************
		//Forward declarations
		template<class T> static void ManageBoolExpressions(DecisionMaker *dm, Expression **expr, Expression *e1, Expression *e2, const std::string& op);
		template<class T> static void ManageEqualityExpression(DecisionMaker *dm, Expression **expr, Expression *e1, Expression *e2, const std::string& op);
		template<class T> static void ManageNumberExpressions(DecisionMaker *dm, Expression **expr, Expression *e1, Expression *e2, const std::string& op);
		template<class T> static void ManageBinaryExpression(DecisionMaker *dm, Expression **expr, Expression *e1, Expression *e2);
		template<class T> void ManageBinaryOperatorsExpressions(DecisionMaker *dm, Expression **expr, Expression *e1, Expression *e2, const std::string& op, bool absorbent);
		static Expression* ManageBinaryOperatorWithConstExpression(DecisionMaker *dm, Expression *expr, bool absorbentElement);

		//****************************
		//Implementation

		template<class T>
		static void ManageBoolExpressions(DecisionMaker *dm, Expression **expr, Expression *e1, Expression *e2, const std::string& op) {
			RETURN_ON_ERROR_BINARY(expr, e1, e2);
			if(!e1->IsBool() || !e2->IsBool()) {
				SET_ERROR_WITH_THREE_ARGS(dm, "BinaryOperatorTypeMismatch", DMSL_yylineno, op.c_str(), "boolean");
				delete e1;	delete e2;
				*expr = (Expression *) 0;
			}
			else
				ManageBinaryExpression<T>(dm, expr, e1, e2);
		}
		
		//****************************

		template<class T>
		static void ManageEqualityExpression(DecisionMaker *dm, Expression **expr, Expression *e1, Expression *e2, const std::string& op) {
			RETURN_ON_ERROR_BINARY(expr, e1, e2);
			if(e1->IsNumber() && e2->IsNumber())
				ManageNumberExpressions<T>(dm, expr, e1, e2, op);
			else if(!Expression::HaveComparableType(e1, e2)) {
				SET_ERROR_WITH_ONE_ARG(dm, "EqualityDiffentTypes", DMSL_yylineno);
				delete e1;	delete e2;
				*expr = (Expression *) 0;
			}
			else
				ManageBinaryExpression<T>(dm, expr, e1, e2);
		}

		//****************************

		template<class T>
		static void ManageNumberExpressions(DecisionMaker *dm, Expression **expr, Expression *e1, Expression *e2, const std::string& op) {
			RETURN_ON_ERROR_BINARY(expr, e1, e2);
			if(!e1->IsNumber() || !e2->IsNumber()) {
				SET_ERROR_WITH_THREE_ARGS(dm, "BinaryOperatorTypeMismatch", DMSL_yylineno, op.c_str(), "number");
				delete e1;	delete e2;
				*expr = (Expression *) 0;
			}
			else
				ManageBinaryExpression<T>(dm, expr, e1, e2);
		}

		//****************************

		template<class T>
		static void ManageBinaryExpression(DecisionMaker *dm, Expression **expr, Expression *e1, Expression *e2) {
			RETURN_ON_ERROR_BINARY(expr, e1, e2);
			Expression *tmp = new T(dm, e1, e2);
			if(e1->IsConst() && e2->IsConst()) {
				tmp->SetLine(DMSL_yylineno);		//to have line information in a possible evaluation error
				ExprValue *val = tmp->Evaluate(dm);
				if(val->IsError()) {
					*expr = (Expression *) 0;
					SET_ERROR_WITH_TWO_ARGS(dm, "ConstExpressionEvaluationError", DMSL_yylineno, val->GetError().c_str());
				}
				else
					*expr = Expression::CreateConstExpression(dm, val);
				delete val;	delete tmp;
			}
			else
				*expr = tmp;
		}

		//****************************

		template<class T> void ManageBinaryOperatorsExpressions(
			DecisionMaker *dm, 
			Expression **expr, 
			Expression *e1, 
			Expression *e2,
			const std::string& op,
			bool absorbent) 
		{
			RETURN_ON_ERROR_BINARY(expr, e1, e2);
			ManageBoolExpressions<T>(dm, expr, e1, e2, op);
			if(!*expr)
				return;	//cleanup already performed by ManageBoolExpressions
			else if(!(*expr)->IsConst()) {
				Expression *tmp = ManageBinaryOperatorWithConstExpression(dm, e1, absorbent);
				if(!tmp) {
					assert(!e2->IsConst());
					tmp = e2->Clone();	//upon expr deletion e2 is deleted as well so we need to clone it
					delete *expr;	*expr = tmp;
				}
				else if(tmp != e1) {
					delete *expr;	*expr = tmp;	//a ConstBoolExpression
					APPEND_WARNING_WITH_TWO_ARGS(dm, "BoolValueConstEvaluation", DMSL_yylineno, util::toString(absorbent).c_str());
				}
				else {
					tmp = ManageBinaryOperatorWithConstExpression(dm, e2, absorbent);
					if(!tmp) {
						assert(!e1->IsConst());
						tmp = e1->Clone();	//upon expr deletion e1 is deleted as well so we need to clone it
						delete *expr;	*expr = tmp;
					}
					else if(tmp != e2) {
						delete *expr;	*expr = tmp;	//a ConstBoolExpression
						APPEND_WARNING_WITH_TWO_ARGS(dm, "BoolValueConstEvaluation", DMSL_yylineno, util::toString(absorbent).c_str());
					}
					//else nothing more we can do, it will be evaluated later on
				}
			}												
		}

		////////////////////////////////////////////////////////////////////////
		// Absorbent element for operator 'or' is true and for 'and' is false.
		// Returns a new BoolExpression when only that is needed, NULL when only
		// the other expression is needed or the given expr if it is necesary.
		//
		static Expression* ManageBinaryOperatorWithConstExpression(DecisionMaker *dm, Expression *expr, bool absorbentElement) {
			Expression *tmp = expr;
			if(expr->IsBool() && expr->IsConst()) {
				ExprValue *val = expr->Evaluate(dm);
				if(val->GetBool() == absorbentElement)
					tmp = new ConstBoolExpression(dm, absorbentElement);
				else
					tmp = (Expression *) 0;
				delete val;
			}
			return tmp;
		}
	};

	/////////////////////////////////////////////////////////////////
	// Manage functions implementation
	//

	void Manage_dottedId_dottedId	(DecisionMaker *, const char **id, const char *id1, const char *id2) {
		*id = util::concat(3, id1, ".", id2);
		delete[] id1;
		delete[] id2;
	}
	void Manage_dottedId_identifier	(DecisionMaker *, const char **id, const char *i)	{ *id = i; }

	void Manage_identifier_string	(DecisionMaker *, const char **id, const char *i)	{ *id = i; }
	void Manage_identifier_id		(DecisionMaker *, const char **id, const char *i)	{ *id = i; }

	//****************************

	void Manage_expr_boolexpr	(DecisionMaker *, Expression **expr, Expression *e)	{ *expr = e; }
	void Manage_expr_arithexpr	(DecisionMaker *, Expression **expr, Expression *e)	{ *expr = e; }

	void Manage_expr_doubledot	(DecisionMaker *dm, Expression **expr, Expression *e1, Expression *e2) {
		RETURN_ON_ERROR_BINARY(expr, e1, e2);
		if(e1->IsNumber() && e2->IsNumber()) {
			if(e1->IsConst() && e2->IsConst()) {
				ExprValue *left = e1->Evaluate((DecisionMaker *) 0);
				ExprValue *right = e2->Evaluate((DecisionMaker *) 0);
				assert(left && left->IsNumber() && right && right->IsNumber());
				double leftVal = left->GetNumber(), rightVal = right->GetNumber();
				delete left;
				delete right;
				if(leftVal > rightVal) {
					SET_ERROR_WITH_THREE_ARGS(dm, "InvalidRange", DMSL_yylineno, leftVal, rightVal);
					delete e1;	delete e2;
					*expr = (Expression *) 0;
					return;
				}
			}
			*expr = new RangeExpression(dm, e1, e2);
		}
		else {
			SET_ERROR_WITH_THREE_ARGS(dm, "BinaryOperatorTypeMismatch", DMSL_yylineno, "..", "number");
			delete e1;	delete e2;
			*expr = (Expression *) 0;
		}
	}

	void Manage_expr_set(DecisionMaker *dm, Expression **expr, ExprList	*l)	{
		RETURN_ON_ERROR_UNARY(expr, l);
		*expr = new SetExpression(dm, l);
	}

	void Manage_expr_rangelist(DecisionMaker *dm, Expression **expr, ExprList	*l)	{
		RETURN_ON_ERROR_UNARY(expr, l);
		unsigned count = 1;
		for(ExprList::const_iterator i = l->begin(); i != l->end(); ++i, ++count)
			if(!(*i)->IsRange()) {
				SET_ERROR_WITH_TWO_ARGS(dm, "RangeListTypeMismatch", DMSL_yylineno, count);
				util::destroyContainer<ExprList>(l);
				Expression::RemoveList(dm, l);
				*expr = (Expression *) 0;
				return;
			}

		ExprList *consts = new ExprList;
		for(ExprList::iterator i = l->begin(); i != l->end(); /*empty*/)
			if((*i)->IsConst())
				consts->push_back(*i), i = l->erase(i);
			else
				++i;

		if(consts->size() == 1) {
			l->push_front(consts->front());
			consts->clear();
			delete consts;
		}
		else if(!consts->empty()) {
			Expression::AddList(dm, consts);
			ExprValue* val = RangeListExpression(dm, consts).Evaluate((DecisionMaker *) 0);
			assert(val && val->IsRangeList());
			const RangeList& rangeList = val->GetRangeList();
			for(RangeList::const_reverse_iterator i = rangeList.rbegin(); i != rangeList.rend(); ++i)
				l->push_front(
					new RangeExpression(
						dm,
						new ConstRealExpression(dm, i->first),
						new ConstRealExpression(dm, i->second)
					)
				);
			delete val;
		}
		
		*expr = new RangeListExpression(dm, l);
	}

	//****************************

	void Manage_exprList_exprListNonEmpty	(DecisionMaker *, ExprList **list, ExprList *l)	{ *list = l; }
	void Manage_exprList_empty				(DecisionMaker *dm, ExprList **list)
		{ Expression::AddList(dm, *list = new ExprList); }

	//****************************

	void Manage_exprListNonEmpty_exprListNonEmpty	(DecisionMaker *dm, ExprList **list, ExprList *l, Expression *e) {
		if(!l || !e) {
			util::destroyContainer<ExprList>(l);
			Expression::RemoveList(dm, l);
			delete e;
			*list = (ExprList *) 0;
			return;
		}
		l->push_back(e);
		*list = l;
	}

	void Manage_exprListNonEmpty_expr(DecisionMaker *dm, ExprList **list, Expression *e) {
		RETURN_ON_ERROR_UNARY(list, e);
		Expression::AddList(dm, *list = new ExprList(1, e));
	}

	////////////////////////////////////
	// Manage_boolexpr_ functions
	//

	void Manage_boolexpr_or	(DecisionMaker *dm, Expression **expr, Expression *e1, Expression *e2)
		{ BinaryExpressions::ManageBinaryOperatorsExpressions<OrExpression>(dm, expr, e1, e2, "or", true); }

	void Manage_boolexpr_and(DecisionMaker *dm, Expression **expr, Expression *e1, Expression *e2)
		{ BinaryExpressions::ManageBinaryOperatorsExpressions<AndExpression>(dm, expr, e1, e2, "and", false); }

	//****************************

	#define MANAGE_BOOLEXPR_RELATIONAL_DECLARATION(_suffix, _type, _op)											\
		void Manage_boolexpr_##_suffix (DecisionMaker *dm, Expression **expr, Expression *e1, Expression *e2) {	\
			BinaryExpressions::ManageNumberExpressions<_type>(dm, expr, e1, e2, #_op);							\
		}

	MANAGE_BOOLEXPR_RELATIONAL_DECLARATION(lessThan, LessThanExpression, <)
	MANAGE_BOOLEXPR_RELATIONAL_DECLARATION(lessEqual, LessEqualExpression, <=)
	MANAGE_BOOLEXPR_RELATIONAL_DECLARATION(greaterThan, GreaterThanExpression, >)
	MANAGE_BOOLEXPR_RELATIONAL_DECLARATION(greaterEqual, GreaterEqualExpression, >=)

	//****************************

	void Manage_boolexpr_equal(DecisionMaker *dm, Expression **expr, Expression *e1, Expression *e2)
		{ BinaryExpressions::ManageEqualityExpression<EqualExpression>(dm, expr, e1, e2, "="); }

	void Manage_boolexpr_notEqual(DecisionMaker *dm, Expression **expr, Expression *e1, Expression *e2)
		{ BinaryExpressions::ManageEqualityExpression<NotEqualExpression>(dm, expr, e1, e2, "!="); }

	//****************************

	void Manage_boolexpr_in	(DecisionMaker *dm, Expression **expr, Expression *e1, Expression *e2) {
		RETURN_ON_ERROR_BINARY(expr, e1, e2);
		if(!e2->IsSet() && !e2->IsRange() && !e2->IsRangeList()) {
			SET_ERROR_WITH_ONE_ARG(dm, "OperatorInTypeMismatch", DMSL_yylineno);
			delete e1;	delete e2;
			*expr = (Expression *) 0;
		}
		else
			*expr = new InExpression(dm, e1, e2);
	}

	//****************************

	void Manage_boolexpr_not (DecisionMaker *dm, Expression **expr, Expression *e)	{
		RETURN_ON_ERROR_UNARY(expr, e);
		if(!e->IsBool()) {
			SET_ERROR_WITH_THREE_ARGS(dm, "UnaryOperatorTypeMismatch", DMSL_yylineno, "not", "boolean");
			delete e;
			*expr = (Expression *) 0;
		}
		else
			*expr = new NotExpression(dm, e);
	}

	//****************************

	void Manage_boolexpr_isactive	(DecisionMaker *dm, Expression **expr, ExprList *list)
		{ ManageLibfuncExpressions<IsActiveExpression>(dm, expr, list, "isactive"); }

	void Manage_boolexpr_hasattr	(DecisionMaker *dm, Expression **expr, ExprList *list)
		{ ManageLibfuncExpressions<HasAttrExpression>(dm, expr, list, "hasattr"); }

	void Manage_boolexpr_cansupport	(DecisionMaker *dm, Expression **expr, ExprList *list)
		{ ManageLibfuncExpressions<CanSupportExpression>(dm, expr, list, "cansupport"); }

	void Manage_boolexpr_bool	(DecisionMaker *dm, Expression **expr, bool val)
		{ *expr = new ConstBoolExpression(dm, val);	}

	//****************************

	void Manage_boolexpr_parenthesis(DecisionMaker *dm, Expression **expr, Expression *e)	{
		RETURN_ON_ERROR_UNARY(expr, e);
		*expr = new ParenthesisExpression(dm, e);
	}

	//****************************

	void Manage_boolexpr_dottedId (DecisionMaker *dm, Expression **expr, const char* val) {
		Expression *tmp = dm->GetSymbolTable().GetStereotype(val);
		if(!tmp)
			tmp = dm->GetSymbolTable().GetDefine(val);
		*expr = tmp ? tmp->Clone() : new ConstStringExpression(dm, val);
		delete[] val;
	}

	//****************************

	void Manage_boolexpr_params (DecisionMaker *dm, Expression **expr, const char *id) {
		*expr = new ParamsExpression(dm, id);
		delete[] id;
	}

	////////////////////////////////////
	// Manage_arithexpr_ functions
	//

	#define MANAGE_ARITHEXPR_BINARY_DECLARATION(_suffix, _type, _op)											\
		void Manage_arithexpr_##_suffix(DecisionMaker *dm, Expression **expr, Expression *e1, Expression *e2)	\
			{ BinaryExpressions::ManageNumberExpressions<_type>(dm, expr, e1, e2, #_op); }
		
	MANAGE_ARITHEXPR_BINARY_DECLARATION(sub, SubExpression, -)
	MANAGE_ARITHEXPR_BINARY_DECLARATION(mul, MulExpression, *)
	MANAGE_ARITHEXPR_BINARY_DECLARATION(div, DivExpression, /)
	MANAGE_ARITHEXPR_BINARY_DECLARATION(mod, ModExpression, %)

	/////////////////////////////
	// Again special care for +
	//

	void Manage_arithexpr_add(DecisionMaker *dm, Expression **expr, Expression *e1, Expression *e2) {
		RETURN_ON_ERROR_BINARY(expr, e1, e2);
		if(	e1->GetType() != Expression::ExprTypeSet && e2->GetType() != Expression::ExprTypeSet &&	//not definite sets
			((e1->IsNumber() && e2->IsNumber()) || e1->IsString() || e2->IsString())
		)
			BinaryExpressions::ManageBinaryExpression<AddExpression>(dm, expr, e1, e2);
		else {
			SET_ERROR_WITH_ONE_ARG(dm, "OperatorPlusIncompatibleTypes", DMSL_yylineno);
			delete e1;	delete e2;
			*expr = (Expression *) 0;
		}
	}

	//****************************

	void Manage_arithexpr_uplus	(DecisionMaker *dm, Expression **expr, Expression *e)
		{ ManageUnaryArithmeticExpressions<UplusExpression>(dm, expr, e, "+"); }

	void Manage_arithexpr_uminus(DecisionMaker *dm, Expression **expr, Expression *e)
		{ ManageUnaryArithmeticExpressions<UminusExpression>(dm, expr, e, "-"); }

	void Manage_arithexpr_tonumber(DecisionMaker *dm, Expression **expr, ExprList *list)
		{ ManageLibfuncExpressions<ToNumberExpression>(dm, expr, list, "tonumber"); }

	void Manage_arithexpr_random (DecisionMaker *dm, Expression **expr, ExprList *list)
		{ ManageLibfuncExpressions<RandomExpression>(dm, expr, list, "random"); }

	//****************************

	void Manage_arithexpr_int	(DecisionMaker *dm, Expression **expr, int	 val) { *expr = new ConstIntExpression	(dm, val);	}
	void Manage_arithexpr_real	(DecisionMaker *dm, Expression **expr, double	 val) { *expr = new ConstRealExpression	(dm, val);	}
}
