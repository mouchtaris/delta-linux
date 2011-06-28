/**
 *	BinaryExpressions.h
 *
 *	-- DMSL Project --
 *
 *	Binary Expression Classes for the rule tree.
 *
 *	Giannis Lilis <lilis@ics.forth.gr>
 *	August 2006 (Version 1) - July 2007 (Version 2)
 */

#ifndef BINARY_EXPRESSIONS_H
#define BINARY_EXPRESSIONS_H

#include "Expressions.h"
#include "ManageFunctionErrorHandling.h"

namespace dmsl {
	////////////////////////////////////////////////////////////////////
	// Binary Expressions. Replicate code elimination with templates

	////////////////////////////////////////////////////////////////////////////////
	// The basic arithmetic operators as functors accepting double type arguments
	//

	//No AddOperator is used as the + is specially handled due to string concatenation

	#define ARITH_OPERATOR_CLASS_DECLARATION(_name, _op)										\
		class _name {																			\
		public:																					\
			double operator	()		(double x, double y)	const { return x _op y;			}	\
			const char*		AnyError(double, double)		const { return (const char *) 0;}	\
			const char*		Id		(void)					const { return #_op;			}	\
			_name(void) {}																		\
		};

	ARITH_OPERATOR_CLASS_DECLARATION(SubOperator, -)
	ARITH_OPERATOR_CLASS_DECLARATION(MulOperator, *)

	//************************************************

	class DivOperator {
	public:
		double operator	()		(double x, double y)	const { return x / y; }
		const char*		AnyError(double, double y)		const
			{ return y == 0 ? DecisionMaker::GetConfiguration().GetStringAttribute("EvaluationZeroDivision").c_str() : (const char *) 0; }
		const char*		Id		(void)					const { return "/";	  }
		DivOperator(void) {}
	};

	//************************************************

	class ModOperator {
	public:
		double operator	()		(double x, double y)	const { return (int) x % (int) y; }
		const char*		AnyError(double x, double y)	const {
			return x != (int) x || y != (int) y ? 
				DecisionMaker::GetConfiguration().GetStringAttribute("EvaluationRealModulo").c_str() : y == 0 ? 
				DecisionMaker::GetConfiguration().GetStringAttribute("EvaluationZeroModulo").c_str() : (const char *) 0; 
		}
		const char*		Id		(void)					const { return "%"; }
		ModOperator(void) {}
	};

	//////////////////////////////////////////////////////////////////////////////
	// The basic relational operators as functors accepting double type arguments
	//
	#define RELATIONAL_OPERATOR_CLASS_DECLARATION(_name, _op)						\
		class _name {																\
		public:																		\
			bool operator	()	(double x, double y)	const { return x _op y; }	\
			const char*		Id	(void)					const { return #_op;	}	\
			_name(void) {}															\
		};

	RELATIONAL_OPERATOR_CLASS_DECLARATION(LessThanOperator, <)
	RELATIONAL_OPERATOR_CLASS_DECLARATION(LessEqualOperator, <=)
	RELATIONAL_OPERATOR_CLASS_DECLARATION(GreaterThanOperator, >)
	RELATIONAL_OPERATOR_CLASS_DECLARATION(GreaterEqualOperator, >=)

	/////////////////////////////////////////////////////////////////////////////
	// The basic equality operators as functors accepting "templatized" arguments
	// The non built-in types should have these equality operators overloaded
	//
	#define EQUALITY_OPERATOR_CLASS_DECLARATION(_name, _cOp, _dmslOp)	\
		class _name {													\
		public:															\
			template<typename T>										\
			bool operator	()	(T x, T y)	const { return x _cOp y; }	\
			const char*		Id	(void)		const { return #_dmslOp; }	\
			_name(void) {}												\
		};

	EQUALITY_OPERATOR_CLASS_DECLARATION(EqualOperator, ==, =)
	EQUALITY_OPERATOR_CLASS_DECLARATION(NotEqualOperator, !=, !=)

	/////////////////////////////////////////////////////////
	// The arithmetic operator as a template functor class
	// Used for -, *, /, %
	//
	template <class OpFunctor> class ArithFunctor {
	public:
		bool LegalArgumentPair (const ExprValue& left, const ExprValue& right) const { return left.IsNumber() && right.IsNumber(); }

		void operator()(ExprValue& left, ExprValue& right, ExprValue* result) const {
			const OpFunctor op;
			const char* error = op.AnyError(left.GetNumber(), right.GetNumber());
			if (error)
				result->SetError(error);
			else
				result->SetNumber(op(left.GetNumber(), right.GetNumber()));
		}

		const char* Id (void) const { return OpFunctor().Id(); }

		ArithFunctor(void) {}
	};

	//////////////////////////////////////////////////////
	// The bool operator (no need for functor class here)
	// Used for or, and
	//
	#define BOOL_FUNCTOR_CLASS_DECLARATION(_name, _op, _absorbent)			\
		class _name {														\
		public:																\
			bool		Absorbent	(void) const { return _absorbent;	}	\
			const char* Id			(void) const { return #_op;			}	\
			_name(void) {}													\
		};

	BOOL_FUNCTOR_CLASS_DECLARATION(AndFunctor, and, false)
	BOOL_FUNCTOR_CLASS_DECLARATION(OrFunctor, or, true)

	/////////////////////////////////////////////////////////
	// The relational operator as a template functor class
	// Used for <, <=, >, >=
	//
	template <class OpFunctor> class RelationalFunctor {
	public:
		bool LegalArgumentPair (const ExprValue& left, const ExprValue& right) const { return left.IsNumber() && right.IsNumber(); }

		void operator()(ExprValue& left, ExprValue& right, ExprValue* result) const {
			const OpFunctor op;
			result->SetBool(op(left.GetNumber(), right.GetNumber()));
		}

		const char* Id (void) const { return OpFunctor().Id(); }

		RelationalFunctor(void) {}
	};

	/////////////////////////////////////////////////////////
	// The equality operator as a template functor class
	// Used for ==, !=
	//
	template <class OpFunctor> class EqualityFunctor {
	public:
		bool LegalArgumentPair (const ExprValue& left, const ExprValue& right) const
			{ return (left.GetType() == right.GetType() && !left.IsError()) || (left.IsNumber() && right.IsNumber()); }

		void operator()(ExprValue& left, ExprValue& right, ExprValue* result) const {
			const OpFunctor op;
			result->SetBool(op(left, right));
		}

		const char* Id (void) const { return OpFunctor().Id(); }

		EqualityFunctor(void) {}
	};

	/////////////////////////////////////////////////////
	// The plus operator as a functor class used for 
	// numbers and for string concatenating
	//
	class PlusFunctor {
	public:
		bool LegalArgumentPair (const ExprValue& left, const ExprValue& right) const {
			return	!left.IsList() && !right.IsList() && !left.IsError() && !right.IsError() && 
					((left.IsNumber() && right.IsNumber()) || left.IsString() || right.IsString());
		}

		void operator()(ExprValue& left, ExprValue& right, ExprValue* result) const {
			if(left.IsNumber() && right.IsNumber())
				result->SetNumber(left.GetNumber() + right.GetNumber());
			else if(left.IsString() && right.IsNumber())
				result->SetString(left.GetString() + util::toString(right.GetNumber()));
			else if(left.IsString() && right.IsBool())
				result->SetString(left.GetString() + util::toString(right.GetBool()));
			else if(left.IsNumber() && right.IsString())
				result->SetString(util::toString(left.GetNumber()) + right.GetString());
			else if(left.IsBool() && right.IsString())
				result->SetString(util::toString(left.GetBool()) + right.GetString());
			else if(left.IsString() && right.IsString())
				result->SetString(left.GetString() + right.GetString());
			else
				assert(false);
		}

		const char* Id (void) const { return "+"; }

		PlusFunctor(void) {}
	};

	/////////////////////////////////////////////////
	// The range operator as a functor class
	//
	class RangeFunctor {
	public:
		bool LegalArgumentPair (const ExprValue& left, const ExprValue& right) const
			{ return left.IsNumber() && right.IsNumber(); }

		void operator()(ExprValue& left, ExprValue& right, ExprValue* result) const {
			double l = left.GetNumber(), r = right.GetNumber();
			if(l > r)
				SET_ERROR_WITH_TWO_ARGS(result, "EvaluationInvalidRange", l, r);
			else
				result->SetRange(Range(l, r));
		}

		const char* Id (void) const { return ".."; }

		RangeFunctor(void) {}
	};

	/////////////////////////////////////////////////
	// The in operator as a functor class used to test if
	// a number lies in a set, a range os a range list.
	//
	class InFunctor {
	public:
		static bool IsWithinRange(const Range& range, double number)
			{ return number >= range.first && number <= range.second; }

		static bool IsWithinRange(const RangeList& rangeList, double number) {
			for(RangeList::const_iterator i = rangeList.begin(); i != rangeList.end(); ++i)
				if(IsWithinRange(*i, number))
					return true;
			return false;
		}

		static bool IsWithinSet(const ExprValueList& list, const ExprValue& val) {
			bool found = false;
			for(ExprValueList::const_iterator i = list.begin(); i != list.end(); ++i) {
				if((*i)->IsRange()) {
					if(val.IsNumber()) {
						if(IsWithinRange((*i)->GetRange(), val.GetNumber())) {
							found = true;
							break;
						}
						else
							continue;
					}
					else if (val.IsRangeList()) {
						ExprValue list(new RangeList(1, (*i)->GetRange()));
						if(list == val) {
							found = true;
							break;
						}
						else
							continue;
					}
				}
				else if((*i)->IsRangeList()) {
					if(val.IsNumber()) {
						if(IsWithinRange((*i)->GetRangeList(), val.GetNumber())) {
							found = true;
							break;
						}
						else
							continue;
					}
					else if(val.IsRange()) {
						ExprValue list(new RangeList(1, val.GetRange()));
						if(**i == list) {
							found = true;
							break;
						}
						else
							continue;
					}
				}
				
				if(**i == val) {
					found = true;
					break;
				}
			}
			return found;
		}

		bool LegalArgumentPair (const ExprValue& left, const ExprValue& right) const {
			return	(left.IsNumber() && (right.IsRange() || right.IsRangeList()))	||
					(!left.IsError() && !left.IsVoid() && right.IsList())			;
		}

		void operator()(ExprValue& left, ExprValue& right, ExprValue* result) const {
			bool found = false;
			if(right.IsList())
				found = IsWithinSet(right.GetList(), left);
			else {
				double n = left.GetNumber();
				if(right.IsRange())
					found = IsWithinRange(right.GetRange(), n);
				else if(right.IsRangeList())
					found = IsWithinRange(right.GetRangeList(), n);
				else
					assert(false);
			}
			result->SetBool(found);
		}

		const char* Id (void) const { return "in"; }

		InFunctor(void) {}
	};

	//////////////////////////////////////////////////////////////////////////
	// The base template for the binary expressions. Specialized according
	// to the way the evaluation is to be performed (specifically if we
	// have logical operators we have to implement short circuit evaluation).
	// The varying template parameter is the functor class 
	// that supplies the necessary members for the operator and the 
	// expression enumerated constant.
	template <const Expression::ExprType type, class OpFunctor> class BaseBinaryExpression : public Expression {
	private:
		Expression *left, *right;
	protected:
		Expression *GetLeft	(void) const { return left;	 }
		Expression *GetRight(void) const { return right; }
	public:
		ExprType GetType (void) const { return type; }

		virtual bool IsConst (void) const { return left->IsConst() && right->IsConst(); }

		virtual const std::string ConvertToString (void) const
			{ return left->ConvertToString() + " " + OpFunctor().Id() + " " + right->ConvertToString(); }

		DependencyList CreateDependencies(void) const {
			DependencyList ret(left->CreateDependencies());
			DependencyList rightDeps(right->CreateDependencies());
			ret.merge(rightDeps);
			return ret;
		}

		BaseBinaryExpression (DecisionMaker *dm, Expression* left, Expression* right) :
			Expression(dm), left(left), right(right) {}
		virtual ~BaseBinaryExpression() { if(!InStaticDestruction(GetDecisionMaker())) delete left, delete right; }
	};

	//////////////////////////////////////////////////////////////////////
	// The key template class that implements the logic of a binary 
	// operations. The varying template parameter is the functor class 
	// that supplies the necessary members for the operator and the 
	// expression enumerated constant.
	//
	template <const Expression::ExprType type, class OpFunctor> class BinaryExpression : public BaseBinaryExpression<type, OpFunctor> {
	public:
		virtual ExprValue* Evaluate (DecisionMaker *dm) const {
			ExprValue *leftResult = this->GetLeft()->Evaluate(dm);
			ExprValue *rightResult = this->GetRight()->Evaluate(dm);
			assert(leftResult && rightResult);
			ExprValue *result = new ExprValue;
			const OpFunctor op;
			if (!op.LegalArgumentPair(*leftResult, *rightResult)) {
				std::string error = "";
				if(leftResult->IsError() && rightResult->IsError())
					error += " : " + leftResult->GetError() + ", " + rightResult->GetError();
				else if(leftResult->IsError() || rightResult->IsError()) {
					error += " : ";
					error += leftResult->IsError() ? leftResult->GetError() : "";
					error += rightResult->IsError() ? rightResult->GetError() : "";
				}
				SET_ERROR_WITH_TWO_ARGS(result, "EvaluationInvalidArguments", op.Id(), error.c_str());
			}
			else
				op(*leftResult, *rightResult, result);
			delete leftResult;
			delete rightResult;
			return result;
		}

		virtual Expression* Clone(void) const {
			return new BinaryExpression<type, OpFunctor>(
				this->GetDecisionMaker(),
				this->GetLeft()->Clone(),
				this->GetRight()->Clone()
			);
		}

		BinaryExpression (DecisionMaker *dm, Expression* left, Expression* right) :
			BaseBinaryExpression<type, OpFunctor>(dm, left, right) {}
		virtual ~BinaryExpression() {}
	};

	//////////////////////////////////////////////////////////////////////
	// Specialization for the binary expressions to support short circuit
	// evaluation for the binary operators 'and' and 'or'.
	// The varying template parameter is the functor class 
	// that supplies the necessary members for the operator and the 
	// expression enumerated constant.
	//
	template <const Expression::ExprType type, class OpFunctor>
	class LogicalBinaryExpression : public BaseBinaryExpression<type, OpFunctor> {
	public:
		virtual ExprValue* Evaluate (DecisionMaker *dm) const {
			const OpFunctor op;
			bool error = false;
			ExprValue *leftResult = (ExprValue *) 0, *rightResult = (ExprValue *) 0, *result;
			leftResult = this->GetLeft()->Evaluate(dm);
			assert(leftResult);
			if(!leftResult->IsBool())
				error = true;
			else if(leftResult->GetBool() == op.Absorbent())
				result = new ExprValue(*leftResult);
			else {
				rightResult = this->GetRight()->Evaluate(dm);
				assert(rightResult);
				if(!rightResult->IsBool())
					error = true;
				else
					result = new ExprValue(*rightResult);
			}
			if(error)
			{
				std::string error = "";
				if(leftResult && leftResult->IsError() && rightResult && rightResult->IsError())
					error += " : " + leftResult->GetError() + ", " + rightResult->GetError();
				else if((leftResult && leftResult->IsError()) || (rightResult && rightResult->IsError())) {
					error += " : ";
					error += leftResult && leftResult->IsError() ? leftResult->GetError() : "";
					error += rightResult && rightResult->IsError() ? rightResult->GetError() : "";
				}
				result = new ExprValue;
				SET_ERROR_WITH_TWO_ARGS(result, "EvaluationInvalidArguments", op.Id(), error.c_str());
			}
			delete leftResult;
			delete rightResult;
			return result;
		}

		virtual Expression* Clone(void) const {
			return new LogicalBinaryExpression<type, OpFunctor>(
				this->GetDecisionMaker(),
				this->GetLeft()->Clone(),
				this->GetRight()->Clone()
			);
		}

		LogicalBinaryExpression (DecisionMaker *dm, Expression* left, Expression* right) :
			BaseBinaryExpression<type, OpFunctor>(dm, left, right) {}
		virtual ~LogicalBinaryExpression() {}
	};

	//////////////////////////////////////////////////////////
	// Specialization for the arithmetic binary expressions
	// to support dinstinction between integers and reals.
	//
	template <const Expression::ExprType type, class OpFunctor>
	class ArithmeticBinaryExpression : public BinaryExpression<type, OpFunctor> {
	public:
		bool IsInteger	(void) const { return this->GetLeft()->IsInteger() && this->GetRight()->IsInteger(); }
		bool IsReal		(void) const { return this->GetLeft()->IsReal() || this->GetRight()->IsReal(); }

		virtual Expression* Clone(void) const {
			return new ArithmeticBinaryExpression<type, OpFunctor>(
				this->GetDecisionMaker(),
				this->GetLeft()->Clone(),
				this->GetRight()->Clone()
			);
		}

		ArithmeticBinaryExpression (DecisionMaker *dm, Expression* left, Expression* right) :
			BinaryExpression<type, OpFunctor>(dm, left, right) {}
	};

	//////////////////////////////////////////////////////////
	// Final Declarations of the Binary Expression subclasses
	//
	typedef ArithmeticBinaryExpression< Expression::ExprTypeSub, ArithFunctor<SubOperator> > SubExpression;
	typedef ArithmeticBinaryExpression< Expression::ExprTypeMul, ArithFunctor<MulOperator> > MulExpression;
	typedef ArithmeticBinaryExpression< Expression::ExprTypeDiv, ArithFunctor<DivOperator> > DivExpression;
	typedef ArithmeticBinaryExpression< Expression::ExprTypeMod, ArithFunctor<ModOperator> > ModExpression;

	//Support short curcuit evaluation for 'and' and 'or' operators
	typedef LogicalBinaryExpression<Expression::ExprTypeAnd, AndFunctor> AndExpression;
	typedef LogicalBinaryExpression<Expression::ExprTypeOr,  OrFunctor>  OrExpression ;

	typedef BinaryExpression< Expression::ExprTypeLessThan,	   RelationalFunctor<LessThanOperator> >		LessThanExpression;
	typedef BinaryExpression< Expression::ExprTypeLessEqual,	RelationalFunctor<LessEqualOperator> >		LessEqualExpression;
	typedef BinaryExpression< Expression::ExprTypeGreaterThan,  RelationalFunctor<GreaterThanOperator> >	GreaterThanExpression;
	typedef BinaryExpression< Expression::ExprTypeGreaterEqual, RelationalFunctor<GreaterEqualOperator> >	GreaterEqualExpression;

	typedef BinaryExpression< Expression::ExprTypeEqual,		EqualityFunctor<EqualOperator> >	EqualExpression;
	typedef BinaryExpression< Expression::ExprTypeNotEqual,	EqualityFunctor<NotEqualOperator> >		NotEqualExpression;

	typedef BinaryExpression<Expression::ExprTypeRange, RangeFunctor>	RangeExpression;
	typedef BinaryExpression<Expression::ExprTypeIn,	InFunctor>		InExpression;

	///////////////////////////////////////////////////////////////////////
	// Take special care for + as it can be used for string concatenation 
	// so we have to specify the IsNumber and IsString Expression methods
	//
	class AddExpression : public ArithmeticBinaryExpression<Expression::ExprTypeAdd, PlusFunctor> {
	public:
		bool IsNumber	(void) const { return GetLeft()->IsNumber() && GetRight()->IsNumber(); }
		bool IsString	(void) const { return GetLeft()->IsString() || GetRight()->IsString(); }

		virtual Expression* Clone(void) const
			{ return new AddExpression(GetDecisionMaker(), GetLeft()->Clone(), GetRight()->Clone()); }

		AddExpression (DecisionMaker *dm, Expression* left, Expression* right) :
			ArithmeticBinaryExpression<Expression::ExprTypeAdd, PlusFunctor>(dm, left, right) {}
	};
}
#endif
