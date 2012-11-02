/**
 *	Statements.cpp
 *
 *	-- DMSL Project --
 *
 *	Implementation of the statement classes.
 *
 *  Version 2 update: Implementation of the Dependency
 *  related methods for alla classes.
 *
 *  Version 3 update: Implementation of the LogicStatement
 *  merging for the incremental rule compilation.
 *
 *	Giannis Lilis <lilis@ics.forth.gr>
 *	August 2006 (Version 1) - July 2007 (Version 2) - January 2008 (Version 3)
 */

#include "Statements.h"
#include "util.h"
#include "ManageFunctionErrorHandling.h"

/////////////////////////////////////////////////////////////////

namespace dmsl {

	Statement::StatementMap Statement::statements;
	Statement::StatementListMap Statement::stmtLists;

	/////////////////////////////////////////////////////////////////

	bool LogicStatement::Evaluate (DecisionMaker *dm) const {
		Statement *stmt = dm->GetSymbolTable().GetComponent(component);
		if(stmt) {
			assert(stmt->GetType() == StmtTypeComponent);
			return stmt->Evaluate(dm);
		}
		else {
			SET_ERROR_WITH_ONE_ARG(dm, "EvaluationComponentNotFound", component.c_str());
			return false;
		}
	}

	DependencyList LogicStatement::CreateDependencies (DecisionMaker *dm) const {
		for(StmtList::iterator i = list->begin(); i != list->end(); ++i) {
			Statement *stmt = *i;
			StmtType type = stmt->GetType();
			assert(type == StmtTypeComponent || type == StmtTypeStereotype || type == StmtTypeDefine);
			if(type == StmtTypeComponent)
				stmt->CreateDependencies(dm);
		}
		return DependencyList();	//dummy return
	}

	const std::string LogicStatement::ConvertToString(unsigned int depth) const {
		std::string str;
		for(StmtList::const_iterator i = list->begin(); i != list->end(); ++i)
			str += (*i)->ConvertToString(depth) + "\n";
		return str;
	}

	void LogicStatement::WriteText (FILE* fp, unsigned int depth) const {
		for(StmtList::const_iterator i = list->begin(); i != list->end(); ++i) {
			(*i)->WriteText(fp, depth);
			fputs("\n", fp);
		}
	}

	//********************************************************

	#define LOGIC_IMPLEMENT_GET_ITEM_FUNCTION(name, type, stmtType)									\
		type * LogicStatement::name(const std::string& name) const {								\
			for(StmtList::const_iterator i = list->begin(); i != list->end(); ++i) {					\
				StmtType t = (*i)->GetType();														\
				assert(t == StmtTypeComponent || t == StmtTypeStereotype || t == StmtTypeDefine);	\
				if(t == stmtType && (static_cast<type*>(*i))->GetName() == name)						\
						return static_cast<type*>(*i);												\
			}																						\
			return (type *) 0;																		\
		}

	LOGIC_IMPLEMENT_GET_ITEM_FUNCTION(GetStereotypeStatement, StereotypeStatement, StmtTypeStereotype)
	LOGIC_IMPLEMENT_GET_ITEM_FUNCTION(GetDefineStatement, DefineStatement, StmtTypeDefine)
	LOGIC_IMPLEMENT_GET_ITEM_FUNCTION(GetComponentStatement, ComponentStatement, StmtTypeComponent)

	//********************************************************

	template<class T> void swapDependencies(T*, T*) {}
	template<> void swapDependencies<ComponentStatement>(ComponentStatement *s1, ComponentStatement *s2)
		{ s1->SwapDependenciesWith(s2); }

	void LogicStatement::AddLogic(LogicStatement *logic, std::list<std::string>& overwrites) {
		SymbolTable& table = GetDecisionMaker()->GetSymbolTable();
		for(StmtList::iterator i = logic->list->begin(); i != logic->list->end(); ++i) {
			StmtType type = (*i)->GetType();

	#define HANDLE_TYPE(outerType, outerGetFunction, innerType, innerGetFunction, innerSetFunction, replaceCode, insertCode)\
		do {																												\
			outerType *st = static_cast<outerType*>(*i);																	\
			const std::string name = st->GetName();																			\
			if (outerType *previous = outerGetFunction(name)) {																\
				overwrites.push_back(name);																					\
				innerType *tmp = st->innerGetFunction();																		\
				assert(tmp);																								\
				delete tmp;																									\
				innerType *copy = previous->innerGetFunction()->Clone(GetDecisionMaker());									\
				replaceCode;																								\
				st->innerSetFunction(copy);																					\
				swapDependencies(st, previous);																				\
			}																												\
			else {																											\
				outerType *copy = static_cast<outerType*>(st->Clone(GetDecisionMaker()));									\
				list->push_back(copy);																						\
				insertCode;																									\
			}																												\
		} while(false)

	#define EMPTY
	#define REPLACER(func) table.func(name, copy, true)
	#define ADDER(func)	table.func(name, copy->GetExpression())

			switch(type) {
				case StmtTypeComponent:		HANDLE_TYPE(ComponentStatement, GetComponentStatement, Statement,
												GetCompound, SetCompound, EMPTY, table.AddComponent(name, copy));
											break;
				case StmtTypeStereotype:	HANDLE_TYPE(StereotypeStatement, GetStereotypeStatement, Expression, 
												GetExpression, SetExpression, REPLACER(AddStereotype), ADDER(AddStereotype));
											break;
				case StmtTypeDefine:		HANDLE_TYPE(DefineStatement, GetDefineStatement, Expression,
												GetExpression, SetExpression, REPLACER(AddDefine), ADDER(AddDefine));
											break;
				default:					assert(false);
			}

	#undef HANDLE_TYPE
		}
	}

	/////////////////////////////////////////////////////////////////

	bool CompoundStatement::Evaluate (DecisionMaker *dm) const {
		for(StmtList::const_iterator i = list->begin(); i != list->end(); ++i) {
			if(!(*i)->Evaluate(dm))
				return false;
		}
		return true;
	}

	DependencyList CompoundStatement::CreateDependencies (DecisionMaker *dm) const {
		DependencyList ret;
		for(StmtList::const_iterator i = list->begin(); i != list->end(); ++i) {
			DependencyList l((*i)->CreateDependencies(dm));
			ret.merge(l);
		}
		return ret;
	}

	const std::string CompoundStatement::ConvertToString (unsigned int depth) const {
		std::string str;
		Indent(str, depth++);
		if(list->size() == 0)
			str += "[]\n";
		else {
			str += "[\n";
			for(StmtList::const_iterator i = list->begin(); i != list->end(); ++i )
				str += (*i)->ConvertToString(depth);
			Indent(str, --depth);
			str += "]\n";
		}
		return str;
	}

	void CompoundStatement::WriteText (FILE* fp, unsigned int depth) const {
		Indent(fp, depth++);
		if(list->size() == 0)
			fputs("[]\n", fp);
		else {
			fputs("[\n", fp);
			for(StmtList::const_iterator i = list->begin(); i != list->end(); ++i )
				(*i)->WriteText(fp, depth);
			Indent(fp, --depth);
			fputs("]\n", fp);
		}
	}

	/////////////////////////////////////////////////////////////////

	bool ComponentStatement::Evaluate (DecisionMaker *dm) const {
		bool ret = false;
		assert(!evaluating);
		evaluating = true;
		ret = NeverEvaluated() || dm->ForcingEvaluation() || DependenciesChanged() ? compound->Evaluate(dm) : true;
		if(ret)	{	//upon successful evaluation we set the dependencies as non changed
			SetAllProfileDependencies(false);
			SetAllActivationDependencies(false);
			SetNeverEvaluated(false);
		}
		evaluating = false;
		return ret;
	}

	DependencyList ComponentStatement::CreateDependencies (DecisionMaker *dm) const {
		DependencyList ret;
		if(evaluating)
			dm->SetError(
				util::MakeString(
					DecisionMaker::GetConfiguration().GetStringAttribute("EvaluationCircle"),
					name.c_str()
				)
			);
		else {
			evaluating = true;
			ret = compound->CreateDependencies(dm);
			for(DependencyList::const_iterator i = ret.begin(); i != ret.end(); ++i) {
				if(i->second)
					profileDependencies[i->first] = true;
				else
					activationDependencies[i->first] = true;
			}
			evaluating = false;
		}
		return ret;	// return the list for chain evaluation dependencies
	}

	//********************************************************

	bool ComponentStatement::DependenciesChanged (void) const {
		StateMap::const_iterator i;
		for(i = profileDependencies.begin(); i != profileDependencies.end(); ++i)
			if(i->second)
				return true;
		for(i = activationDependencies.begin(); i != activationDependencies.end(); ++i)
			if(i->second)
				return true;
		return false;
	}

	void ComponentStatement::SwapDependenciesWith (ComponentStatement *stmt) {
		StateMap temp = profileDependencies;
		profileDependencies = stmt->profileDependencies;
		stmt->profileDependencies = temp;

		temp = activationDependencies;
		activationDependencies = stmt->activationDependencies;
		stmt->activationDependencies = temp;
	}

	//********************************************************

	void ComponentStatement::SetAllProfileDependencies (bool val) const {
		for(StateMap::iterator i = profileDependencies.begin(); i != profileDependencies.end(); ++i)
			i->second = val;
	}

	void ComponentStatement::SetAllActivationDependencies (bool val) const {
		for(StateMap::iterator i = activationDependencies.begin(); i != activationDependencies.end(); ++i)
			i->second = val;
	}

	/////////////////////////////////////////////////////////////////

	bool IfStatement::shouldDecreaseIndent(Statement *stmt) {
		return	stmt->GetType() == Statement::StmtTypeCompound &&
				!static_cast<CompoundStatement *>(stmt)->GetList().empty();
	}

	bool IfStatement::Evaluate (DecisionMaker *dm) const {
		bool ret;
		ExprValue *result = expr->Evaluate(dm);
		assert(result);
		if(!result->IsBool()) {
			std::string error = result->IsError() ? " : " + result->GetError() : "";
			SET_ERROR_WITH_THREE_ARGS(dm, "EvaluationExpressionExpected", "if", "boolean", error.c_str());
			ret = false;
		}
		else if(result->GetBool())
			ret = ifSt->Evaluate(dm);
		else
			ret = elseSt->Evaluate(dm);
		delete result;
		return ret;
	}

	DependencyList IfStatement::CreateDependencies (DecisionMaker *dm) const {
		DependencyList ret(expr->CreateDependencies());
		DependencyList ifStDeps(ifSt->CreateDependencies(dm));
		DependencyList elseStDeps(elseSt->CreateDependencies(dm));
		ret.merge(ifStDeps);
		ret.merge(elseStDeps);
		return ret;
	}

	const std::string IfStatement::ConvertToString (unsigned int depth) const {
		std::string str;
		Indent(str, depth++);
		str += "if " + expr->ConvertToString() + " then\n" + 
			ifSt->ConvertToString(shouldDecreaseIndent(ifSt) ? --depth : depth--);
		if(elseSt->GetType() != StmtTypeEmpty) {
			Indent(str, depth++);
			str += "else\n" + elseSt->ConvertToString(shouldDecreaseIndent(elseSt) ? --depth : depth--);
		}
		return str;
	}

	void IfStatement::WriteText (FILE* fp, unsigned int depth) const {
		Indent(fp, depth++);
		fputs("if ", fp);
		expr->WriteText(fp);
		fputs(" then\n", fp);
		ifSt->WriteText(fp, shouldDecreaseIndent(ifSt) ? --depth : depth--);
		if(elseSt->GetType() != StmtTypeEmpty) {
			Indent(fp, depth++);
			fputs("else\n", fp);
			elseSt->WriteText(fp, shouldDecreaseIndent(elseSt) ? --depth : depth--);
		}
	}

	/////////////////////////////////////////////////////////////////

	bool WhenStatement::shouldDecreaseIndent(Statement *stmt) {
		return	stmt->GetType() == Statement::StmtTypeCompound &&
				!static_cast<CompoundStatement *>(stmt)->GetList().empty();
	}

	bool WhenStatement::Evaluate (DecisionMaker *dm) const { return stmt->Evaluate(dm); }

	DependencyList WhenStatement::CreateDependencies (DecisionMaker *dm) const {
		DependencyList ret(expr->CreateDependencies());
		DependencyList stmtDeps(stmt->CreateDependencies(dm));
		ret.merge(stmtDeps);
		return ret;
	}

	const std::string WhenStatement::ConvertToString (unsigned int depth) const {
		std::string str;
		Indent(str, depth++);
		return str + "when " + expr->ConvertToString() + "\n" +
			stmt->ConvertToString(shouldDecreaseIndent(stmt) ? --depth : depth--);
	}

	void WhenStatement::WriteText (FILE* fp, unsigned int depth) const {
		Indent(fp, depth++);
		fputs("when ", fp);
		expr->WriteText(fp);
		fputs("\n", fp);
		stmt->WriteText(fp, shouldDecreaseIndent(stmt) ? --depth : depth--);
	}

	/////////////////////////////////////////////////////////////////

	bool CaseStatement::shouldDecreaseIndent(Statement *stmt) {
		return	stmt->GetType() == Statement::StmtTypeCompound &&
				!static_cast<CompoundStatement *>(stmt)->GetList().empty();
	}

	bool CaseStatement::Evaluate (DecisionMaker *dm) const {
		bool ret = true;
		ExprValue *caseValue = expr->Evaluate(dm);
		assert(caseValue);
		StmtList::const_iterator i;
		for(i = list->begin(); i != list->end(); ++i) {
			assert((*i)->GetType() == Statement::StmtTypeWhen);
			WhenStatement *s = static_cast<WhenStatement*>(*i);
			ExprValue *whenValue = s->GetExpression()->Evaluate(dm);
			assert(whenValue);
			if(*caseValue == *whenValue) {
				delete whenValue;
				ret = s->Evaluate(dm);
				break;
			}
			else
				delete whenValue;
		}
		if(i == list->end())
			ret = otherwise->Evaluate(dm);
		delete caseValue;
		return ret;
	}

	DependencyList CaseStatement::CreateDependencies (DecisionMaker *dm) const {
		DependencyList ret(expr->CreateDependencies());
		for(StmtList::const_iterator i = list->begin(); i != list->end(); ++i) {
			DependencyList l((*i)->CreateDependencies(dm));
			ret.merge(l);
		}
		DependencyList otherwiseDeps(otherwise->CreateDependencies(dm));
		ret.merge(otherwiseDeps);
		return ret;
	}

	const std::string CaseStatement::ConvertToString (unsigned int depth) const {
		std::string str;
		Indent(str, depth);
		str += "case " + expr->ConvertToString() + "\n";
		Indent(str, depth++);
		str += "[\n";
		for(StmtList::const_iterator i = list->begin(); i != list->end(); ++i )
			str += (*i)->ConvertToString(depth);
		if(otherwise->GetType() != StmtTypeEmpty)
		{
			Indent(str, depth++);
			str += "otherwise\n" + otherwise->ConvertToString(shouldDecreaseIndent(otherwise) ? --depth : depth--);
		}
		Indent(str, --depth);
		return str + "]\n";
	}

	void CaseStatement::WriteText (FILE* fp, unsigned int depth) const {
		Indent(fp, depth);
		fputs("case ", fp);
		expr->WriteText(fp);
		fputs("\n", fp);
		Indent(fp, depth++);
		fputs("[\n", fp);
		for(StmtList::const_iterator i = list->begin(); i != list->end(); ++i )
			(*i)->WriteText(fp, depth);
		if(otherwise->GetType() != StmtTypeEmpty)
		{
			Indent(fp, depth++);
			fputs("otherwise\n", fp);
			otherwise->WriteText(fp, shouldDecreaseIndent(otherwise) ? --depth : depth--);
		}
		Indent(fp, --depth);
		fputs("]\n", fp);
	}

	/////////////////////////////////////////////////////////////////

	bool ExpressionStatement::Evaluate (DecisionMaker *dm) const {
		bool ret = true;
		ExprValue *result = expr->Evaluate(dm);
		assert(result && (result->IsError() || result->IsVoid()));
		if(result->IsError()) {
			dm->SetError(result->GetError());
			ret = false;
		}
		delete result;
		return ret;
	}

	const std::string ExpressionStatement::ConvertToString (unsigned int depth) const {
		std::string str;
		Indent(str, depth);
		return str + expr->ConvertToString() + "\n";
	}

	void ExpressionStatement::WriteText (FILE* fp, unsigned int depth) const {
		Indent(fp, depth);
		expr->WriteText(fp);
		fputs("\n", fp);
	}
}
