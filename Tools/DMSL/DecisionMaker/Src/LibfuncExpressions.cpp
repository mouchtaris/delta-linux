/**
 *	LibfuncExpressions.cpp
 *
 *	-- DMSL Project --
 *
 *	Implementation of the Library function Expression Classes.
 *
 *	Giannis Lilis <lilis@ics.forth.gr>
 *	August 2006 (Version 1) - July 2007 (Version 2)
 *	Updated December 2008 for the cansupport libfunc.
 */

#include "LibfuncExpressions.h"
#include "ManageFunctionErrorHandling.h"

namespace dmsl {

	int RandomExpression::lastRandomValue = -1;

	////////////////////////////////////////////////////
	// The string argument libfuncs as functor classes
	//
	void IsActiveFunctor::operator()(DecisionMaker *dm, ExprValue& val, ExprValue* result) const
		{ result->SetBool(dm->IsActiveComponent(val.GetString())); }

	DependencyList IsActiveFunctor::CreateDependencies (Expression *expr) const {
		assert(expr->IsConst());
		ExprValue *val = expr->Evaluate((DecisionMaker *) 0);	//arg will never be used as this is const
		assert(val && val->IsString());
		const std::string component = val->GetString();
		delete val;
		return DependencyList(1, DependencyPair(component, false));
	}

	//*********************

	void ToNumberFunctor::operator()(DecisionMaker *, ExprValue& val, ExprValue* result) const
		{ result->SetNumber(atof(val.GetString().c_str())); }

	//*********************

	void HasAttrFunctor::operator()(DecisionMaker *dm, ExprValue& val, ExprValue* result) const {
		const DecisionMaker::ProfileList& profiles = dm->GetProfileList();
		assert(!profiles.empty());
		bool found = false;
		for(DecisionMaker::ProfileList::const_iterator i = profiles.begin(); i != profiles.end(); ++i)
			if((*i)->GetAttribute(val.GetString())) {
				found = true;
				break;
			}
		result->SetBool(found);
	}

	////////////////////////////////////////////////////////
	// Random expression libfunc evaluation implementation
	//
	ExprValue* RandomExpression::Evaluate (DecisionMaker *dm) const {
		assert(list->size() <= 1);
		ExprValue *ret = new ExprValue;
		if(list->empty()) {
			if(lastRandomValue == -1)
				SET_ERROR_WITH_ZERO_ARGS(ret, "EvaluationRandomInvalidCall");
			else
				ret->SetNumber(lastRandomValue);
		}
		else {
			assert(list->front()->IsInteger() && list->front()->IsConst());
			ExprValue *result = list->front()->Evaluate(dm);
			assert(result && result->IsInteger());
			ret->SetNumber(lastRandomValue = (rand() % (int) result->GetNumber()));
			delete result;
		}
		return ret;
	}

	////////////////////////////////////////////////////////
	// CanSupport expression libfunc evaluation implementation
	//
	ExprValue* CanSupportExpression::Evaluate (DecisionMaker *dm) const {
		const char *name = "cansupport";
		assert(list->size() == 2);
		ExprValue *ret = new ExprValue;
		ExprValue *comp = list->front()->Evaluate(dm);
		if(!comp->IsString()) {
			const std::string error = comp->IsError() ? " : " + comp->GetError() : "";
			SET_ERROR_WITH_FOUR_ARGS(ret, "EvaluationArgumentExpected", name, 1, "string", error.c_str());
		}
		else {
			ExprValue *style = list->back()->Evaluate(dm);
			if(!style->IsString()) {
				const std::string error = style->IsError() ? " : " + style->GetError() : "";
				SET_ERROR_WITH_FOUR_ARGS(ret, "EvaluationArgumentExpected", name, 2, "string", error.c_str());
			}
			else {
				DecisionMaker::PlausibilityFunction f = dm->GetPlausibilityFunction();
				if(f)
					ret->SetBool((*f)(comp->GetString(), style->GetString()));
				else
					SET_ERROR_WITH_ONE_ARG(ret, "EvaluationNoPlausibilityFunction", name);
			}
			delete style;
		}
		delete comp;
		return ret;
	}

	////////////////////////////////////////////////////////
	// Library expression libfunc evaluation implementation
	//
	ExprValue* LibraryExpression::Evaluate (DecisionMaker *dm) const {
		const char *name = "library";
		assert(list->size() == 1 || list->size() == 2);
		ExprValue *ret = new ExprValue;
		ExprValue *func = list->front()->Evaluate(dm);
		if(!func->IsString()) {
			const std::string error = func->IsError() ? " : " + func->GetError() : "";
			SET_ERROR_WITH_FOUR_ARGS(ret, "EvaluationArgumentExpected", name, 1, "string", error.c_str());
		}
		else {
			bool status = true;
			std::string data = "";
			if(list->size() == 2) {
				ExprValue *arg = list->back()->Evaluate(dm);
				if(!arg->IsString()) {
					const std::string error = arg->IsError() ? " : " + arg->GetError() : "";
					SET_ERROR_WITH_FOUR_ARGS(ret, "EvaluationArgumentExpected", name, 2, "string", error.c_str());
					status = false;
				}
				else
					data = arg->GetString();
				delete arg;
			}
			if(status) {
				const std::string funcName = func->GetString();
				DecisionMaker::LibraryFunction f = dm->GetLibraryFunction(funcName);
				if(f) {
					(*f)(data);
					ret->SetVoid();
				}
				else
					SET_ERROR_WITH_TWO_ARGS(ret, "EvaluationLibraryUndeclaredFunction", name, funcName.c_str());
			}
		}
		delete func;
		return ret;
	}
}
