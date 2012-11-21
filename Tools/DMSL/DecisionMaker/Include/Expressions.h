/**
 *	Expressions.h
 *
 *	-- DMSL Project --
 *
 *	Expression Super class and some misc subclasses.
 *
 *	Giannis Lilis <lilis@ics.forth.gr>
 *	August 2006 (Version 1) - July 2007 (Version 2)
 */

#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H

#include <list>
#include <map>
#include <string>

#include <assert.h>
#include <stdio.h>

#include "ExprValue.h"
#include "util.h"

namespace dmsl {
	//////////////////////////
	// Forward Declarations
	//
	class DecisionMaker;
	class Expression;

	typedef std::list<Expression *>			ExprList;
	typedef std::list<ExprList *>			ExprListList;
	typedef std::pair<std::string, bool>	DependencyPair;	//<name, bool>, profile: true, activation: false
	typedef std::list<DependencyPair>		DependencyList;

	/////////////////////////////////////////////////////////////////
	// The expression super class.
	//

	class Expression : public util::Clonable<Expression, DecisionMaker> {
	private:
		typedef std::map<DecisionMaker *, ExprList> ExpressionMap;
		static ExpressionMap expressions;
		typedef std::map<DecisionMaker *, ExprListList> ExpressionListMap;
		static ExpressionListMap exprLists;
		
		typedef std::map<DecisionMaker *, bool> DestructionMap;
		static DestructionMap staticDestruction;
		DecisionMaker *dm;
		unsigned line;
	protected:
		DecisionMaker *	GetDecisionMaker(void) const { return dm; }
	public:
		enum ExprType {
			ExprTypeInt			= 0,
			ExprTypeReal		= 1,
			ExprTypeBool		= 2,
			ExprTypeString		= 3,

			ExprTypeAdd			= 4,
			ExprTypeSub			= 5,
			ExprTypeMul			= 6,
			ExprTypeDiv			= 7,
			ExprTypeMod			= 8,

			ExprTypeUminus		= 9,
			ExprTypeUplus		= 10,

			ExprTypeAnd			= 11,
			ExprTypeOr			= 12,
			ExprTypeNot			= 13,

			ExprTypeLessThan	= 14,
			ExprTypeLessEqual	= 15,
			ExprTypeGreaterThan	= 16,
			ExprTypeGreaterEqual= 17,

			ExprTypeEqual		= 18,
			ExprTypeNotEqual	= 19,

			ExprTypeIn			= 20,

			ExprTypeSet			= 21,
			ExprTypeRange		= 22,
			ExprTypeRangeList	= 23,
			
			ExprTypeParams		= 24,

			ExprTypeIsActive	= 25,
			ExprTypeToNumber	= 26,
			ExprTypeHasAttr		= 27,
			ExprTypeRandom		= 28,
			ExprTypeCanSupport	= 29,
			ExprTypeLibrary		= 30,

			ExprTypeParenthesis = 31
		};

		/////////////////////////////////////////////////////////////////
		// Type checking members.
		//
		virtual bool IsConst (void) const {
			ExprType type = GetType();
			return	type == ExprTypeInt		||
					type == ExprTypeReal	||
					type == ExprTypeBool	||
					type == ExprTypeString	;
					// no binary or unary operators declared here - virtualized and specified where necessary
		}

		virtual bool IsNumber (void) const {
			ExprType type = GetType();
			return	HasUndefinedType()		||	//it can be a number
					type == ExprTypeInt		||
					type == ExprTypeReal	||
					type == ExprTypeSub		||
					type == ExprTypeMul		||
					type == ExprTypeDiv		||
					type == ExprTypeMod		||
					type == ExprTypeUplus	||
					type == ExprTypeUminus	||
					type == ExprTypeToNumber||
					type == ExprTypeRandom	;
					// no ExprTypeAdd here - virtualized and specified in AddExpression
		}

		virtual bool IsInteger(void) const {
			ExprType type = GetType();
			return	HasUndefinedType()		||	//it can be an integer
					type == ExprTypeInt		||
					type == ExprTypeToNumber||
					type == ExprTypeRandom	;
					// no binary or unary operators declared here - virtualized and specified where necessary
		}

		virtual bool IsReal(void) const {
			ExprType type = GetType();
			return	HasUndefinedType()			||	//it can be a real
					type == ExprTypeReal		||
					type == ExprTypeToNumber;
					// no binary or unary operators declared here - virtualized and specified where necessary
		}
		
		virtual bool IsString (void) const { 
			return	HasUndefinedType()			||	//it can be a string
					GetType() == ExprTypeString	;
					// no ExprTypeAdd here - virtualized and specified in AddExpression
		}

		virtual bool IsBool	(void) const { 
			ExprType type = GetType();
			return	HasUndefinedType()			||	//it can be a bool
					type == ExprTypeBool		||
					type == ExprTypeAnd			||
					type == ExprTypeOr			||
					type == ExprTypeNot			||
					type == ExprTypeLessThan	||
					type == ExprTypeLessEqual	||
					type == ExprTypeGreaterThan	||
					type == ExprTypeGreaterEqual||
					type == ExprTypeEqual		||
					type == ExprTypeNotEqual	||
					type == ExprTypeIn			||
					type == ExprTypeIsActive	||
					type == ExprTypeCanSupport	||
					type == ExprTypeHasAttr		;
		}

		virtual bool IsSet (void) const {
			return	HasUndefinedType()			||	//it can be a set
					GetType() == ExprTypeSet; 
		}

		virtual bool IsRange (void) const {
			return	HasUndefinedType()			||	//it can be a range
					GetType() == ExprTypeRange; 
		}

		virtual bool IsRangeList (void) const {
			return	HasUndefinedType()			||	//it can be a range list
					GetType() == ExprTypeRangeList; 
		}

		bool HasUndefinedType (void) const { return GetType() == ExprTypeParams; }

		virtual void WriteText (FILE* fp) { fputs(ConvertToString().c_str(), fp); }

		/////////////////////////////////////////////////////////////////
		// Static members.
		//
		static bool HaveComparableType(Expression *e1, Expression *e2) {
			return	e1->GetType() == e2->GetType()				||
					e1->HasUndefinedType()						||
					e2->HasUndefinedType()						||
					(e1->IsNumber()		&& e2->IsNumber())		||
					(e1->IsString()		&& e2->IsString())		||
					(e1->IsBool()		&& e2->IsBool())			||
					(e1->IsRange()		&& e2->IsRange())		||
					(e1->IsSet()		&& e2->IsSet())				||
					(e1->IsRangeList()	&& e2->IsRangeList())	;
		}

		static Expression* CreateConstExpression(DecisionMaker *dm, ExprValue *v);

		///////////////////////////////////////////////////////////////////////////
		// Static Destruction is a guard for static Destrucion upon parse errors
		// This is used so as not to allow tree nodes to delete their children.
		//
		static void SetStaticDestruction(DecisionMaker *dm, bool val)	{ staticDestruction[dm] = val;	}
		static bool InStaticDestruction	(DecisionMaker *dm)	{
			DestructionMap::const_iterator i = staticDestruction.find(dm);
			return i == staticDestruction.end() ? false : i->second;
		}

		static void AddList		(DecisionMaker *dm, ExprList *l)	{ exprLists[dm].push_back(l);	}
		static void RemoveList	(DecisionMaker *dm, ExprList *l)	{ exprLists[dm].remove(l);		}

		static void Cleanup (DecisionMaker *dm) {
			ExprList& expressions = Expression::expressions[dm];
			ExprList deleteList;
			std::for_each(expressions.begin(), expressions.end(), util::ListCopyFunctor<ExprList>(deleteList));
			std::for_each(deleteList.begin(), deleteList.end(), util::deleter<Expression *>());
			expressions.clear();
			deleteList.clear();

			ExprListList& exprLists = Expression::exprLists[dm];
			std::for_each(exprLists.begin(), exprLists.end(), util::deleter<ExprList *>());
			exprLists.clear();
		}

		static void RemoveDecisionMaker(DecisionMaker *dm) {
			ExpressionMap::iterator iter = expressions.find(dm);
			if (iter != expressions.end()) {
				assert(iter->second.empty());
				expressions.erase(iter);
			}
			ExpressionListMap::iterator listIter = exprLists.find(dm);
			if (listIter != exprLists.end()) {
				assert(listIter->second.empty());
				exprLists.erase(listIter);
			}
			DestructionMap::iterator i = staticDestruction.find(dm);
			if (i != staticDestruction.end()) {
				assert(!i->second);
				staticDestruction.erase(i);
			}
		}

		/////////////////////////////////////////////////////////////////
		// Line support
		unsigned	GetLine (void) const	{ return line; }
		void		SetLine	(unsigned line) { this->line = line; }

		/////////////////////////////////////////////////////////////////
		// Pure virtual members.
		//
		virtual ExprType			GetType				(void)				const = 0;
		virtual ExprValue*			Evaluate			(DecisionMaker *dm)	const = 0;
		virtual DependencyList		CreateDependencies	(void)				const = 0;
		virtual const std::string	ConvertToString		(void)				const = 0;

		Expression& operator= (const Expression& expr) { line = expr.line; return *this; }

		Expression (DecisionMaker *dm) : dm(dm), line(0)	{ expressions[dm].push_back(this);	}
		virtual ~Expression()								{ expressions[dm].remove(this);		}
	};

	/////////////////////////////////////////////////////////////////
	// Constant Value Expressions
	//
	template<typename T, Expression::ExprType Type> class ConstExpression : public Expression {
	protected:
		T value;

	private:
		//////////////////////////
		//Printing functions
		//
		void print(FILE *fp, int				val) const	{ fprintf(fp, "%d", val); }
		void print(FILE *fp, double				val) const	{ fprintf(fp, "%lf", val); }
		void print(FILE *fp, bool				val) const	{ fprintf(fp, "%s", util::toString(val).c_str()); }
		void print(FILE *fp, const std::string&	val) const	{ fprintf(fp, "\"%s\"", val.c_str()); }

	public:

		ExprType			GetType				(void)				const { return Type; }
		const std::string	ConvertToString		(void)				const { return util::toString(value); }
		ExprValue*			Evaluate			(DecisionMaker *)	const { return new ExprValue(value); }
		DependencyList		CreateDependencies	(void)				const { return DependencyList(); }

		Expression* Clone(DecisionMaker* dm = (DecisionMaker*) 0) const {
			Expression* result = new ConstExpression<T, Type>(dm ? dm : GetDecisionMaker(), value);
			*result = *this;
			return result;
		}

		ConstExpression(DecisionMaker *dm, T value) : Expression(dm), value(value) {}
		virtual ~ConstExpression() {}
	};

	typedef ConstExpression<int,		 Expression::ExprTypeInt>	 ConstIntExpression;
	typedef ConstExpression<double,		 Expression::ExprTypeReal>	 ConstRealExpression;
	typedef ConstExpression<bool,		 Expression::ExprTypeBool>	 ConstBoolExpression;
	typedef ConstExpression<std::string, Expression::ExprTypeString> ConstStringExpression;

	////////////////////////////////////////////////////////////////////////
	// Set expression
	//
	class SetExpression : public Expression {
	private:
		ExprList *list;
	public:
		ExprType			GetType				(void)				const { return ExprTypeSet; }
		const std::string	ConvertToString		(void)				const;
		ExprValue*			Evaluate			(DecisionMaker *dm) const;
		DependencyList		CreateDependencies	(void)				const;

		Expression* Clone(DecisionMaker* dm = (DecisionMaker*) 0) const {
			DecisionMaker* owner = dm ? dm : GetDecisionMaker();
			Expression* result = new SetExpression(
				owner,
				util::cloneContainer<ExprList>(list, std::bind2nd(std::mem_fun(&Expression::Clone), owner))
			);
			*result = *this;
			return result;
		}

		SetExpression(DecisionMaker *dm, ExprList *list) : Expression(dm), list(list) {}
		virtual ~SetExpression() {
			DecisionMaker *dm = GetDecisionMaker();
			if(!InStaticDestruction(dm)) {
				util::destroyContainer<ExprList>(list);
				RemoveList(dm, list);
			}
		}
	};

	////////////////////////////////////////////////////////////////////////
	// RangeList expression
	//
	class RangeListExpression : public Expression {
	private:
		ExprList *list;
	public:
		ExprType			GetType				(void)				const { return ExprTypeRangeList; }
		const std::string	ConvertToString		(void)				const;
		ExprValue*			Evaluate			(DecisionMaker *dm) const;
		DependencyList		CreateDependencies	(void)				const;

		Expression* Clone(DecisionMaker* dm = (DecisionMaker*) 0) const {
			DecisionMaker* owner = dm ? dm : GetDecisionMaker();
			Expression* result = new RangeListExpression(
				owner,
				util::cloneContainer<ExprList>(list, std::bind2nd(std::mem_fun(&Expression::Clone), owner))
			);
			*result = *this;
			return result;
		}

		RangeListExpression(DecisionMaker *dm, ExprList *list) : Expression(dm), list(list) {}
		virtual ~RangeListExpression() {
			DecisionMaker *dm = GetDecisionMaker();
			if(!InStaticDestruction(dm)) {
				util::destroyContainer<ExprList>(list);
				RemoveList(dm, list);
			}
		}
	};

	////////////////////////////////////////////////////////////////////////
	// Params expression
	//
	class ParamsExpression : public Expression {
	private:
		std::string name;
	public:
		void				WriteText		(FILE* fp)			const;
		ExprType			GetType			(void)				const { return ExprTypeParams; }
		const std::string ConvertToString	(void)				const;
		ExprValue*		Evaluate			(DecisionMaker *dm) const;
		DependencyList	CreateDependencies	(void)				const;

		Expression* Clone(DecisionMaker* dm = (DecisionMaker*) 0) const {
			Expression* result = new ParamsExpression(dm ? dm : GetDecisionMaker(), name);
			*result = *this;
			return result;
		}

		ParamsExpression (DecisionMaker *dm, const std::string& name) : Expression(dm), name(name) {}
		~ParamsExpression() {}
	};
}

#endif
