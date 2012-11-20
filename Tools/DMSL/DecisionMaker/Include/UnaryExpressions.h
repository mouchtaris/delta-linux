/**
 *	UnaryExpressions.h
 *
 *	-- DMSL Project --
 *
 *	Unary Expression Classes for the rule tree.
 *
 *	Giannis Lilis <lilis@ics.forth.gr>
 *	August 2006 (Version 1) - July 2007 (Version 2)
 */

#ifndef UNAR_EXPRESSIONS_H
#define UNAR_EXPRESSIONS_H

#include "Expressions.h"
#include "ManageFunctionErrorHandling.h"

namespace dmsl {

	////////////////////////////////////////////////////////////////////////
	// Unary Expressions. Replicate code elimination with templates
	// The unary operators as functors accepting a double type argument.
	#define UNARY_OPERATOR_CLASS_DECLARATION(_name, _op)				\
		class _name {													\
		public:															\
			double operator	() (double x)	const { return _op x; }		\
			const char*		Id (void)		const { return #_op;	}	\
			_name(void) {}												\
		};

	UNARY_OPERATOR_CLASS_DECLARATION(UnaryPlusOperator,	 +)
	UNARY_OPERATOR_CLASS_DECLARATION(UnaryMinusOperator, -)

	/////////////////////////////////////////////////////////////
	// The unary arithmetic operator as a template functor class
	// Used for (unary) +, -
	//
	template <class OpFunctor> class UnaryArithFunctor {
	public:
		bool LegalArgument (const ExprValue& val) const { return val.IsNumber(); }

		void operator()(ExprValue& val, ExprValue* result) const {
			const OpFunctor op;
			result->SetNumber(op(val.GetNumber()));
		}

		const char* Id (void) const { return OpFunctor().Id(); }

		UnaryArithFunctor(void) {}
	};

	///////////////////////////////////////////
	// The (unary) not operator functor class
	//
	class NotFunctor {
	public:
		bool LegalArgument (const ExprValue& val) const { return val.IsBool(); }

		void operator()(ExprValue& val, ExprValue* result) const { result->SetBool(!val.GetBool()); }

		const char* Id (void) const { return "not"; }

		NotFunctor(void) {}
	};

	///////////////////////////////////////////
	// The parenthesis functor class
	//
	class ParenthesisFunctor {
	public:
		bool LegalArgument (const ExprValue&) const { return true; }

		void operator()(ExprValue& val, ExprValue* result) const { *result = val; }

		const char* Id (void) const { assert(false); return (const char *) 0; }
		//Id function should never be called as the parenthesis needs a different output handling

		ParenthesisFunctor(void) {}
	};

	//////////////////////////////////////////////////////
	// The UnaryExpression template class implementing
	// the logic of the unary expressions with the given
	// functor class instances.
	//
	template <const Expression::ExprType type, class OpFunctor> class UnaryExpression : public Expression {
	private:
		Expression* expr;
	protected:
		Expression* GetExpression(void) const { return expr; }
	public:
		ExprType GetType (void) const { return type; }

		virtual const std::string ConvertToString(void) const
			{ return std::string(OpFunctor().Id()) + " " + expr->ConvertToString(); }

		ExprValue* Evaluate (DecisionMaker *dm) const {
			ExprValue *result = expr->Evaluate(dm);
			assert(result);
			ExprValue *ret = new ExprValue;
			const OpFunctor op;
			if (!op.LegalArgument(*result)) {
				std::string error = result->IsError() ? " : " + result->GetError() : "";
				SET_ERROR_WITH_THREE_ARGS(ret, "EvaluationInvalidArguments", this->GetLine(), op.Id(), error.c_str());
			}
			else
				op(*result, ret);
			delete result;
			return ret;
		}

		DependencyList CreateDependencies(void) const { return expr->CreateDependencies(); }

		Expression* Clone(DecisionMaker* dm = (DecisionMaker*) 0) const {
			DecisionMaker* owner = dm ? dm : this->GetDecisionMaker();
			return new UnaryExpression<type, OpFunctor>(owner, expr->Clone(owner));
		}

		UnaryExpression (DecisionMaker *dm, Expression* expr) : Expression(dm), expr(expr) {}
		virtual ~UnaryExpression() { if(!InStaticDestruction(GetDecisionMaker())) delete expr; }
	};

	//////////////////////////////////////////////////////////////
	// The ArithmeticUnaryExpression template class providing
	// distinction between integers and reals.
	//
	template <const Expression::ExprType type, class OpFunctor>
	class ArithmeticUnaryExpression : public UnaryExpression<type, OpFunctor> {
	public:
		bool IsInteger	(void) const { return this->GetExpression()->IsInteger();}
		bool IsReal		(void) const { return this->GetExpression()->IsReal();	}

		ArithmeticUnaryExpression (DecisionMaker *dm, Expression* expr) :
			UnaryExpression<type, OpFunctor>(dm, expr) {}
	};

	//////////////////////////////////////////////////////////
	// Final Declarations of the Unary Expression subclasses
	//
	typedef ArithmeticUnaryExpression< Expression::ExprTypeUplus,	UnaryArithFunctor<UnaryPlusOperator> >  UplusExpression ;
	typedef ArithmeticUnaryExpression< Expression::ExprTypeUminus,	UnaryArithFunctor<UnaryMinusOperator> > UminusExpression;

	typedef UnaryExpression<Expression::ExprTypeNot, NotFunctor> NotExpression;

	////////////////////////////////////////////////////
	// Handle Parenthesis seperatly because of the 
	// difference in WriteText function.
	//
	class ParenthesisExpression : public UnaryExpression<Expression::ExprTypeParenthesis, ParenthesisFunctor> {
	public:

		const std::string ConvertToString(void) const { return std::string("(") + GetExpression()->ConvertToString() + ")"; }

		///////////////////////////////////////
		// Virtualized type checking members
		//

		bool IsConst	(void) const { return GetExpression()->IsConst();		}
		bool IsNumber	(void) const { return GetExpression()->IsNumber();		}
		bool IsInteger	(void) const { return GetExpression()->IsInteger();		}
		bool IsReal		(void) const { return GetExpression()->IsReal();			}
		bool IsString	(void) const { return GetExpression()->IsString();		}
		bool IsBool		(void) const { return GetExpression()->IsBool();			}
		bool IsSet		(void) const { return GetExpression()->IsSet();			}
		bool IsRange	(void) const { return GetExpression()->IsRange();		}
		bool IsRangeList(void) const { return GetExpression()->IsRangeList();	}

		ParenthesisExpression (DecisionMaker *dm, Expression* expr) :
			UnaryExpression<ExprTypeParenthesis, ParenthesisFunctor>(dm, expr) {}
	};
}

#endif
