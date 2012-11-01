/**
 *	Statements.h
 *
 *	-- DMSL Project --
 *
 *	Statement Super class and subclasses for the rule tree.
 *
 *  Version 2 update: ComponentStatement class now contains
 *  information about the profile parameters and activations
 *  that each component depends on in order to avoid evaluation
 *  of non-changed components.
 *
 *  Version 3 update: Added functionality to merge LogicStatements
 *  in order to support incremental rule compilation.
 *
 *	Giannis Lilis <lilis@ics.forth.gr>
 *	August 2006 (Version 1) - July 2007 (Version 2) - January 2008 (Version 3)
 *  Added case statement (January 2009).
 */

#ifndef STATEMENT_H
#define STATEMENT_H

#include <list>
#include <map>
#include <string>

#include <assert.h>

#include "DecisionMaker.h"
#include "Expressions.h"
#include "ManageFunctionErrorHandling.h"

namespace dmsl {

	/////////////////////////////////////////////////////////////////
	// Forward Declarations
	//
	class Statement;
	class ComponentStatement;
	class StereotypeStatement;
	class DefineStatement;

	typedef std::list<Statement *>	StmtList;
	typedef std::list<StmtList *>	StmtListList;

	/////////////////////////////////////////////////////////////////
	// The statement super class.
	//

	class Statement : public util::Clonable<Statement, DecisionMaker> {
	private:
		typedef std::map<DecisionMaker *, StmtList> StatementMap;
		static StatementMap statements;
		typedef std::map<DecisionMaker *, StmtListList> StatementListMap;
		static StatementListMap stmtLists;
		DecisionMaker *dm;
	protected:
		DecisionMaker *	GetDecisionMaker(void) const { return dm; }
	public:
		enum StmtType {
			StmtTypeActivate	= 0,
			StmtTypeCancel		= 1,
			StmtTypeEvaluate	= 2,
			StmtTypeIf			= 3,
			StmtTypeCompound	= 4,
			StmtTypeLogic		= 5,
			StmtTypeComponent	= 6,
			StmtTypeStereotype	= 7,
			StmtTypeDefine		= 8,
			StmtTypeExpression	= 9,
			StmtTypeEmpty		= 10,
			StmtTypeWhen		= 11,
			StmtTypeCase		= 12
		};

		/////////////////////////////////////////////////////////////////
		// Static members.
		//
		static void Indent(FILE *fp, unsigned int depth) {
			for(unsigned int i = 0; i < depth; ++i)
				fprintf(fp, "\t");
		}

		static void Indent(std::string& str, unsigned int depth) {
			for(unsigned int i = 0; i < depth; ++i)
				str += "\t";
		}

		static void AddList		(DecisionMaker *dm, StmtList *l)	{ stmtLists[dm].push_back(l);	}
		static void RemoveList	(DecisionMaker *dm, StmtList *l)	{ stmtLists[dm].remove(l);		}

		static void Cleanup (DecisionMaker *dm) {
			StmtList& statements = Statement::statements[dm];
			StmtList deleteList;
			std::for_each(statements.begin(), statements.end(), util::ListCopyFunctor<StmtList>(deleteList));
			std::for_each(deleteList.begin(), deleteList.end(), util::deleter<Statement *>());	
			statements.clear();
			deleteList.clear();

			StmtListList& stmtLists = Statement::stmtLists[dm];
			std::for_each(stmtLists.begin(), stmtLists.end(), util::deleter<StmtList *>());
			stmtLists.clear();
		}

		static void RemoveDecisionMaker(DecisionMaker *dm) {
			StatementMap::iterator iter = statements.find(dm);
			if (iter != statements.end()) {
				assert(iter->second.empty());
				statements.erase(iter);
			}
			StatementListMap::iterator listIter = stmtLists.find(dm);
			if (listIter != stmtLists.end()) {
				assert(listIter->second.empty());
				stmtLists.erase(listIter);
			}
		}

		/////////////////////////////////////////////////////////////////
		// Pure abstract members.
		//
		virtual StmtType			GetType				(void)							const = 0;
		virtual bool				Evaluate			(DecisionMaker *dm)				const = 0;
		virtual void				WriteText			(FILE* fp, unsigned int depth)	const = 0;
		virtual DependencyList		CreateDependencies	(DecisionMaker *dm)				const = 0;
		virtual const std::string	ConvertToString		(unsigned int depth)			const = 0;

		Statement (DecisionMaker *dm) : dm(dm) 	{ statements[dm].push_back(this);	}
		virtual ~Statement()					{ statements[dm].remove(this);		}
	};

	////////////////////////////////////////////////////////////////////////////////////////////

	class EmptyStatement : public Statement {
	public:
		StmtType			GetType				(void)					const	{ return StmtTypeEmpty;	}
		bool				Evaluate			(DecisionMaker *)		const	{ return true;			}
		void				WriteText			(FILE*, unsigned int)	const	{}
		DependencyList		CreateDependencies	(DecisionMaker *)		const	{ return DependencyList(); }
		const std::string	ConvertToString		(unsigned int depth)	const	{ return ""; }

		Statement* Clone(DecisionMaker* dm = (DecisionMaker*) 0) const
			{ return new EmptyStatement(dm ? dm : GetDecisionMaker()); }

		EmptyStatement(DecisionMaker *dm) : Statement(dm) {}
	};

	////////////////////////////////////////////////////////////////////////////////////////////

	template<const Statement::StmtType type> class ListStatement : public Statement {
	protected:
		StmtList *list;
	public:
		StmtType	GetType				(void)	const	{ return type;	}
		const		StmtList& GetList	(void)	const	{ return *list;	}

		ListStatement (DecisionMaker *dm, StmtList *list) : Statement(dm), list(list) {}
		virtual ~ListStatement() {
			if(!Expression::InStaticDestruction(GetDecisionMaker())) {
				util::destroyContainer<StmtList>(list);
				RemoveList(GetDecisionMaker(), list);
			}
		}
	};

	class LogicStatement : public ListStatement<Statement::StmtTypeLogic> {
	private:
		std::string component;
		ComponentStatement	* GetComponentStatement	(const std::string& name) const;
		StereotypeStatement * GetStereotypeStatement(const std::string& name) const;
		DefineStatement		* GetDefineStatement	(const std::string& name) const;
	public:
		bool				Evaluate			(DecisionMaker *dm)				const;
		void				WriteText			(FILE* fp, unsigned int depth)	const;
		DependencyList		CreateDependencies	(DecisionMaker *dm)				const;
		const std::string	ConvertToString		(unsigned int depth)			const;

		void				AddLogic			(LogicStatement *logic, std::list<std::string>& overwrites);
		void				SetEvaluationTarget	(const std::string& component = "")	{ this->component = component; }

		Statement* Clone(DecisionMaker* dm = (DecisionMaker*) 0) const {
			DecisionMaker* owner = dm ? dm : GetDecisionMaker();
			return new LogicStatement(
				owner,
				util::cloneContainer<StmtList>(list, std::bind2nd(std::mem_fun(&Statement::Clone), owner))
			);
		}

		LogicStatement (DecisionMaker *dm, StmtList* list) :
			ListStatement<Statement::StmtTypeLogic>(dm, list) {}
	};

	class CompoundStatement : public ListStatement<Statement::StmtTypeCompound> {
	public:
		bool				Evaluate			(DecisionMaker *dm)				const;
		void				WriteText			(FILE* fp, unsigned int depth)	const;
		DependencyList		CreateDependencies	(DecisionMaker *dm)				const;
		const std::string	ConvertToString		(unsigned int depth)			const;

		Statement* Clone(DecisionMaker* dm = (DecisionMaker*) 0) const {
			DecisionMaker* owner = dm ? dm : GetDecisionMaker();
			return new CompoundStatement(
				owner,
				util::cloneContainer<StmtList>(list, std::bind2nd(std::mem_fun(&Statement::Clone), owner))
			);
		}

		CompoundStatement (DecisionMaker *dm, StmtList* list) :
			ListStatement<Statement::StmtTypeCompound>(dm, list) {}
	};

	////////////////////////////////////////////////////////////////////////////////////////////

	class ComponentStatement : public Statement {
	private:
		mutable bool evaluating, neverEvaluated;
		std::string name;
		Statement *compound;

		typedef std::map<std::string, bool> StateMap;	//<dependency, changed>
		mutable StateMap profileDependencies, activationDependencies;

	public:
		StmtType			GetType				(void)					const { return StmtTypeComponent; }
		bool				Evaluate			(DecisionMaker *dm)		const;
		DependencyList		CreateDependencies	(DecisionMaker *dm)		const;

		const std::string	ConvertToString		(unsigned int depth)	const
			{ return std::string("component \"") + name + "\"\n" + compound->ConvertToString(depth); }

		void WriteText (FILE* fp, unsigned int depth) const {
			fputs(("component \"" + name + "\"\n").c_str(), fp);
			compound->WriteText(fp, depth);
		}

		Statement * GetCompound(void) const		{ return compound;	}
		void		SetCompound(Statement *st)	{ compound = st;	}

		const std::string& GetName (void) { return name; }

		////////////////////////////////////////
		// Dependency Related Members
		//

		bool DependenciesChanged(void) const;

		void SwapDependenciesWith(ComponentStatement *stmt);

		//*******************************************************

		void SetAllProfileDependencies		(bool val) const;
		void SetAllActivationDependencies	(bool val) const;

		//*******************************************************

		bool NeverEvaluated		(void)		const	{ return neverEvaluated;}
		void SetNeverEvaluated	(bool val)	const	{ neverEvaluated = val;	}
		
		//*******************************************************
		
		void SetProfileDependency	(const std::string& name, bool val) {
			assert(DependsOnProfile(name));
			profileDependencies[name] = val;
		}
		
		void SetActivationDependency(const std::string& name, bool val) {
			assert(DependsOnActivation(name));
			activationDependencies[name] = val;
		}

		//*******************************************************

		bool DependsOnProfile	(const std::string& name)	const
			{ return profileDependencies.find(name) != profileDependencies.end(); }

		bool DependsOnActivation(const std::string& name)	const
			{ return activationDependencies.find(name) != activationDependencies.end(); }

		//*******************************************************

		Statement* Clone(DecisionMaker* dm = (DecisionMaker*) 0) const {
			DecisionMaker* owner = dm ? dm : GetDecisionMaker();
			return new ComponentStatement(owner, name, compound->Clone(owner));
		}

		//*******************************************************

		ComponentStatement (DecisionMaker *dm, const std::string& name, Statement* compound) : Statement(dm),
			evaluating(false), neverEvaluated(true), name(name), compound(compound) {}
		~ComponentStatement() {	if(!Expression::InStaticDestruction(GetDecisionMaker())) delete compound; }
	};


	////////////////////////////////////////////////////////////////////////////////////////////

	class StereotypeStatement : public Statement {
	private:
		std::string name;
		Expression *expr;
	public:
		StmtType			GetType				(void)					const	{ return StmtTypeStereotype;	}
		bool				Evaluate			(DecisionMaker *)		const	{ assert(false); return false;	}	//never to be called
		DependencyList		CreateDependencies	(DecisionMaker *)		const	{ assert(false); return DependencyList(); }	//never to be called
		
		const std::string	ConvertToString		(unsigned int)			const
			{ return std::string("stereotype \"") + name + "\" : " + expr->ConvertToString() + "\n"; }

		void WriteText (FILE* fp, unsigned int) const {
			fputs(("stereotype \"" + name + "\" : ").c_str(), fp);
			expr->WriteText(fp);
			fputs("\n", fp);
		}

		Expression *	GetExpression(void) const		{ return expr;	}
		void			SetExpression(Expression *e)	{ expr = e;		}

		const std::string& GetName (void) const { return name; }

		Statement* Clone(DecisionMaker* dm = (DecisionMaker*) 0) const {
			DecisionMaker* owner = dm ? dm : GetDecisionMaker();
			return new StereotypeStatement(owner, name, expr->Clone(owner));
		}

		StereotypeStatement (DecisionMaker *dm, const std::string& name, Expression* expr) :
			Statement(dm), name(name), expr(expr) {}
		~StereotypeStatement() { if(!Expression::InStaticDestruction(GetDecisionMaker())) delete expr; }
	};

	////////////////////////////////////////////////////////////////////////////////////////////


	class DefineStatement : public Statement {
	private:
		std::string name;
		Expression *expr;
	public:
		StmtType			GetType				(void)					const	{ return StmtTypeDefine;		}
		bool				Evaluate			(DecisionMaker *)		const	{ assert(false); return false;	}	//never to be called
		DependencyList		CreateDependencies	(DecisionMaker *)		const	{ assert(false); return DependencyList(); }	//never to be called

		const std::string	ConvertToString		(unsigned int)			const
			{ return std::string("def \"") + name + "\" " + expr->ConvertToString() + "\n"; }

		void WriteText (FILE* fp, unsigned int) const {
			fputs(("def \"" + name + "\" ").c_str(), fp);
			expr->WriteText(fp);
			fputs("\n", fp);
		}

		Expression *	GetExpression(void) const		{ return expr;	}
		void			SetExpression(Expression *e)	{ expr = e;		}

		const std::string& GetName (void) const { return name; }

		Statement* Clone(DecisionMaker* dm = (DecisionMaker*) 0) const {
			DecisionMaker* owner = dm ? dm : GetDecisionMaker();
			return new DefineStatement(owner, name, expr->Clone(owner));
		}

		DefineStatement (DecisionMaker *dm, const std::string& name, Expression* expr) :
			Statement(dm), name(name), expr(expr) {}
		~DefineStatement() { if(!Expression::InStaticDestruction(GetDecisionMaker())) delete expr; }
	};

	////////////////////////////////////////////////
	// Action statements as functor classes.
	//
	class ActivateFunctor {
	public:
		bool operator()(DecisionMaker *dm, ExprValue& val) const {
			dm->SetActiveComponent(val.GetString(), true);
			return true;
		}
		DependencyList	CreateDependencies	(DecisionMaker *, Expression *)	const	{ return DependencyList();	}
		const char* Id						(void)							const	{ return "activate";		}
		ActivateFunctor(void) {}
	};

	//****************************

	class CancelFunctor {
	public:
		bool operator()(DecisionMaker *dm, ExprValue& val) const {
			dm->SetActiveComponent(val.GetString(), false);
			return true;
		}
		DependencyList	CreateDependencies	(DecisionMaker *,Expression *)	const	{ return DependencyList();	}
		const char* Id						(void)							const	{ return "cancel";			}
		CancelFunctor(void) {}
	};

	//****************************

	class EvaluateFunctor {
	public:
		bool operator()(DecisionMaker *dm, ExprValue& val) const {
			Statement *stmt = dm->GetSymbolTable().GetComponent(val.GetString());
			if(stmt)
				return stmt->Evaluate(dm);
			else {
				SET_ERROR_WITH_ONE_ARG(dm, "EvaluationUndeclaredComponent", val.GetString().c_str());
				return false;
			}
		}

		DependencyList CreateDependencies (DecisionMaker *dm, Expression *expr) const {
			assert(expr->IsConst());
			ExprValue *val = expr->Evaluate((DecisionMaker *) 0);	//arg will never be used as this is const
			assert(val && val->IsString());
			Statement *stmt = dm->GetSymbolTable().GetComponent(val->GetString());
			delete val;
			if (stmt) {
				assert(stmt->GetType() == Statement::StmtTypeComponent);
				return static_cast<ComponentStatement *>(stmt)->CreateDependencies(dm);
			}
			else
				return DependencyList();
		}

		const char* Id (void) const { return "evaluate"; }
		EvaluateFunctor(void) {}
	};

	//////////////////////////////////////////////////////
	// The ActionStatement template class implementing
	// the logic of the action statements with the given
	// functor class instances.
	//
	template <const Statement::StmtType type, class OpFunctor> class ActionStatement : public Statement {
	protected:
		Expression* expr;
	public:
		StmtType GetType (void) const { return type; }

		bool Evaluate (DecisionMaker *dm) const {
			bool ret;
			ExprValue *result = expr->Evaluate(dm);
			assert(result);
			const OpFunctor op;
			if (!result->IsString()) {	//only string allowed here so make the check here and not in op
				const std::string error = result->IsError() ? " : " + result->GetError() : "";
				SET_ERROR_WITH_THREE_ARGS(dm, "EvaluationExpressionExpected", op.Id(), "string", error.c_str());
				ret = false;
			}
			else
				ret = op(dm, *result);
			delete result;
			return ret;
		}

		DependencyList CreateDependencies (DecisionMaker *dm) const
			{ return OpFunctor().CreateDependencies(dm, expr); }

		const std::string ConvertToString (unsigned int depth) const {
			std::string str;
			Indent(str, depth);
			return str + std::string(OpFunctor().Id()) + " " + expr->ConvertToString() + "\n";
		}

		void WriteText (FILE* fp, unsigned int depth) const {
			Indent(fp, depth);
			fputs(OpFunctor().Id(), fp);
			fputs(" ", fp);
			expr->WriteText(fp);
			fputs("\n", fp);
		}

		Statement* Clone(DecisionMaker* dm = (DecisionMaker*) 0) const {
			DecisionMaker* owner = dm ? dm : GetDecisionMaker();
			return new ActionStatement(owner, expr->Clone(owner));
		}

		ActionStatement (DecisionMaker *dm, Expression* expr) : Statement(dm), expr(expr) {}
		virtual ~ActionStatement() { if(!Expression::InStaticDestruction(GetDecisionMaker())) delete expr; }
	};

	typedef ActionStatement<Statement::StmtTypeActivate, ActivateFunctor> ActivateStatement;
	typedef ActionStatement<Statement::StmtTypeCancel,	 CancelFunctor>	  CancelStatement;
	typedef ActionStatement<Statement::StmtTypeEvaluate, EvaluateFunctor> EvaluateStatement;

	////////////////////////////////////////////////////////////////////////////////////////////

	class IfStatement : public Statement {
	private:
		Expression *expr;
		Statement *ifSt, *elseSt;
		static bool shouldDecreaseIndent(Statement *stmt);
	public:
		StmtType			GetType				(void)							const { return StmtTypeIf; }
		bool				Evaluate			(DecisionMaker *dm)				const;
		DependencyList		CreateDependencies	(DecisionMaker *dm)				const;
		const std::string	ConvertToString		(unsigned int depth)			const;
		void				WriteText			(FILE* fp, unsigned int depth)	const;

		Statement* Clone(DecisionMaker* dm = (DecisionMaker*) 0) const {
			DecisionMaker* owner = dm ? dm : GetDecisionMaker();
			return new IfStatement(owner, expr->Clone(owner), ifSt->Clone(owner), elseSt->Clone(owner));
		}

		IfStatement (DecisionMaker *dm, Expression* expr, Statement* ifSt, Statement* elseSt) :
			Statement(dm), expr(expr), ifSt(ifSt), elseSt(elseSt) {}
		~IfStatement() { if(!Expression::InStaticDestruction(GetDecisionMaker())) delete expr, delete ifSt, delete elseSt; }
	};

	////////////////////////////////////////////////////////////////////////////////////////////

	class WhenStatement : public Statement {
	private:
		Expression *expr;
		Statement *stmt;
		static bool shouldDecreaseIndent(Statement *stmt);
	public:
		Expression*			GetExpression		(void)							const { return expr; }
		StmtType			GetType				(void)							const { return StmtTypeWhen; }
		bool				Evaluate			(DecisionMaker *dm)				const;
		DependencyList		CreateDependencies	(DecisionMaker *dm)				const;
		const std::string	ConvertToString		(unsigned int depth)			const;
		void				WriteText			(FILE* fp, unsigned int depth)	const;

		Statement* Clone(DecisionMaker* dm = (DecisionMaker*) 0) const {
			DecisionMaker* owner = dm ? dm : GetDecisionMaker();
			return new WhenStatement(owner, expr->Clone(owner), stmt->Clone(owner));
		}

		WhenStatement (DecisionMaker *dm, Expression* expr, Statement* stmt) :
			Statement(dm), expr(expr), stmt(stmt) {}
		~WhenStatement() { if(!Expression::InStaticDestruction(GetDecisionMaker())) delete expr, delete stmt; }
	};

	////////////////////////////////////////////////////////////////////////////////////////////

	class CaseStatement : public ListStatement<Statement::StmtTypeCase> {
	private:
		Expression *expr;
		Statement *otherwise;
		static bool shouldDecreaseIndent(Statement *stmt);
	public:
		StmtType			GetType				(void)							const { return StmtTypeCase; }
		bool				Evaluate			(DecisionMaker *dm)				const;
		DependencyList		CreateDependencies	(DecisionMaker *dm)				const;
		const std::string	ConvertToString		(unsigned int depth)			const;
		void				WriteText			(FILE* fp, unsigned int depth)	const;

		Statement* Clone(DecisionMaker* dm = (DecisionMaker*) 0) const {
			DecisionMaker* owner = dm ? dm : GetDecisionMaker();
			return new CaseStatement(
				owner,
				expr->Clone(owner),
				util::cloneContainer<StmtList>(list, std::bind2nd(std::mem_fun(&Statement::Clone), owner)),
				otherwise->Clone(owner)
			);
		}

		CaseStatement (DecisionMaker *dm, Expression* expr, StmtList* list, Statement* otherwise) :
			ListStatement<Statement::StmtTypeCase>(dm, list), expr(expr), otherwise(otherwise) {}
		~CaseStatement() { if(!Expression::InStaticDestruction(GetDecisionMaker())) delete expr, delete otherwise; }
	};

	////////////////////////////////////////////////////////////////////////////////////////////

	class ExpressionStatement : public Statement {
	private:
		Expression *expr;
	public:
		StmtType			GetType				(void)							const { return StmtTypeExpression; }
		bool				Evaluate			(DecisionMaker *dm)				const;
		DependencyList		CreateDependencies	(DecisionMaker *)				const { return DependencyList(); }
		const std::string	ConvertToString		(unsigned int depth)			const;
		void				WriteText			(FILE* fp, unsigned int depth)	const;

		Statement* Clone(DecisionMaker* dm = (DecisionMaker*) 0) const {
			DecisionMaker* owner = dm ? dm : GetDecisionMaker();
			return new ExpressionStatement(owner, expr->Clone(owner));
		}

		ExpressionStatement (DecisionMaker *dm, Expression* expr) : Statement(dm), expr(expr) {}
		~ExpressionStatement() { if(!Expression::InStaticDestruction(GetDecisionMaker())) delete expr; }
	};
}

#endif
