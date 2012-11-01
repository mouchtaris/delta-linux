/**
 *	LibfuncExpressions.h
 *
 *	-- DMSL Project --
 *
 *	Library function Expression Classes for the rule tree.
 *
 *	Giannis Lilis <lilis@ics.forth.gr>
 *	August 2006 (Version 1) - July 2007 (Version 2)
 *	Updated December 2008 for the cansupport libfunc.
 */

#ifndef LIBFUNC_EXPR_H
#define LIBFUNC_EXPR_H

#include <stdlib.h>

#include "Expressions.h"
#include "ManageFunctionErrorHandling.h"
#include "DecisionMaker.h"

namespace dmsl {
	
	/////////////////////////////////////////////////////////////////
	// BUILT-IN FUNCTIONS.
	//

	template<class T> class LibfuncExpression : public Expression {
	protected:
		ExprList *list;
	private:
		std::string funcname;
	public:
		void WriteText (FILE* fp) const {
			fputs((funcname + "(").c_str(), fp);
			for(ExprList::const_iterator i = list->begin(); i != list->end(); ++i) {
				if(i != list->begin())
					fputs(", ", fp);
				(*i)->WriteText(fp);
			}
			fputs(")", fp);
		}

		const std::string ConvertToString(void) const {
			std::string ret = funcname + "(";
			for(ExprList::const_iterator i = list->begin(); i != list->end(); ++i) {
				if(i != list->begin())
					ret += ", ";
				ret += (*i)->ConvertToString();
			}
			return ret + ")";
		}

		Expression* Clone(DecisionMaker* dm = (DecisionMaker*) 0) const {
			DecisionMaker* owner = dm ? dm : GetDecisionMaker();
			return new T(
				owner,
				util::cloneContainer<ExprList>(list, std::bind2nd(std::mem_fun(&Expression::Clone), owner))
			);
		}

		LibfuncExpression (DecisionMaker *dm, const std::string& funcname, ExprList* list) :
			Expression(dm), list(list), funcname(funcname) {}
		virtual ~LibfuncExpression() {
			DecisionMaker *dm = GetDecisionMaker();
			if(!InStaticDestruction(dm)) {
				util::destroyContainer<ExprList>(list);
				RemoveList(dm, list);
			}
		}
	};

	////////////////////////////////////////////////////
	// The string argument libfuncs as functor classes
	//
	class IsActiveFunctor {
	public:
		void operator()(DecisionMaker *dm, ExprValue& val, ExprValue* result) const;
		DependencyList	CreateDependencies	(Expression *expr)	const;
		const char* Id						(void)				const { return "isactive";	}
		IsActiveFunctor(void) {}
	};

	//*********************

	class ToNumberFunctor {
	public:
		void operator()(DecisionMaker *dm, ExprValue& val, ExprValue* result) const;
		DependencyList	CreateDependencies	(Expression *)	const	{ return DependencyList();	}
		const char* Id						(void)			const	{ return "tonumber";		}
		ToNumberFunctor(void) {}
	};

	//*********************

	class HasAttrFunctor {
	public:
		void operator()(DecisionMaker *dm, ExprValue& val, ExprValue* result) const;
		DependencyList	CreateDependencies	(Expression *)	const	{ return DependencyList();	}
		const char* Id						(void)			const	{ return "hasattr";			}
		HasAttrFunctor(void) {}
	};

	/////////////////////////////////////////////////////////////
	// The LibfuncExpressionWithStringArgument template class 
	// implementing the logic of the string argument taking 
	// built-in function with the given functor class instances.
	//
	template <const Expression::ExprType type, class OpFunctor>
	class LibfuncExpressionWithStringArgument :
		public LibfuncExpression< LibfuncExpressionWithStringArgument<type, OpFunctor> > {
	public:
		Expression::ExprType GetType (void) const { return type; }

		ExprValue* Evaluate (DecisionMaker *dm) const {
			assert(this->list->size() == 1 && this->list->front()->IsString());
			ExprValue *result = this->list->front()->Evaluate(dm);
			assert(result);
			ExprValue *ret = new ExprValue;
			const OpFunctor op;
			if(!result->IsString()) {	//only string allowed here so check here and not in op
				const std::string error = result->IsError() ? " : " + result->GetError() : "";
				SET_ERROR_WITH_FOUR_ARGS(ret, "EvaluationArgumentExpected", op.Id(), 1, "string", error.c_str());
			}
			else
				op(dm, *result, ret);
			delete result;
			return ret;
		}

		DependencyList CreateDependencies(void) const {
			assert(this->list->size() == 1 && this->list->front()->IsString());
			return OpFunctor().CreateDependencies(this->list->front());
		}

		LibfuncExpressionWithStringArgument(DecisionMaker *dm, ExprList* list) :
			LibfuncExpression< LibfuncExpressionWithStringArgument<type, OpFunctor> >(dm, OpFunctor().Id(), list) {}
		~LibfuncExpressionWithStringArgument() {}
	};

	/////////////////////////////////////////////////////////////
	// Final Declarations of the Libfunc Expression subclasses
	// (the ones taking one string argument)
	//

	typedef LibfuncExpressionWithStringArgument<Expression::ExprTypeIsActive, IsActiveFunctor> IsActiveExpression;
	typedef LibfuncExpressionWithStringArgument<Expression::ExprTypeToNumber, ToNumberFunctor> ToNumberExpression;
	typedef LibfuncExpressionWithStringArgument<Expression::ExprTypeHasAttr,  HasAttrFunctor > HasAttrExpression ;

	/////////////////////////////////////////////////////////////

	class RandomExpression : public LibfuncExpression<RandomExpression> {
		static int lastRandomValue;
	public:
		ExprType GetType (void) const { return ExprTypeRandom; }

		ExprValue* Evaluate (DecisionMaker *dm) const;

		DependencyList CreateDependencies(void) const { return DependencyList(); }

		RandomExpression(DecisionMaker *dm, ExprList* list) : LibfuncExpression<RandomExpression>(dm, "random", list) {}
		~RandomExpression() {}
	};

	/////////////////////////////////////////////////////////////

	class CanSupportExpression : public LibfuncExpression<CanSupportExpression> {
		static int lastRandomValue;
	public:
		ExprType GetType (void) const { return ExprTypeCanSupport; }

		ExprValue* Evaluate (DecisionMaker *dm) const;

		DependencyList CreateDependencies(void) const { return DependencyList(); }

		CanSupportExpression(DecisionMaker *dm, ExprList* list) : LibfuncExpression<CanSupportExpression>(dm, "cansupport", list) {}
		~CanSupportExpression() {}
	};

	/////////////////////////////////////////////////////////////

	class LibraryExpression : public LibfuncExpression<LibraryExpression> {
	public:
		ExprType GetType (void) const { return ExprTypeLibrary; }

		ExprValue* Evaluate (DecisionMaker *) const;

		DependencyList CreateDependencies(void) const { return DependencyList(); }

		LibraryExpression(DecisionMaker *dm, ExprList* list) : LibfuncExpression<LibraryExpression>(dm, "library", list) {}
		~LibraryExpression() {}
	};
}

#endif
