/**
 *	Expressions.h
 *
 *	-- DMSL Project --
 *
 *	Implementation of the Expression Classes defined in the header.
 *
 *	Giannis Lilis <lilis@ics.forth.gr>
 *	August 2006 (Version 1) - July 2007 (Version 2)
 */

#include "Expressions.h"
#include "DecisionMaker.h"
#include "ManageFunctionErrorHandling.h"

namespace dmsl {

	Expression::ExpressionMap Expression::expressions;
	Expression::ExpressionListMap Expression::exprLists;
	Expression::DestructionMap Expression::staticDestruction;

	/////////////////////////////////////////////////////////////////

	Expression* Expression::CreateConstExpression(DecisionMaker *dm, ExprValue *value) {
		switch(value->GetType()) {
			case ValueTypeInt:		return new ConstIntExpression(dm, (int) value->GetNumber());
			case ValueTypeReal:		return new ConstRealExpression(dm, value->GetNumber());
			case ValueTypeBool:		return new ConstBoolExpression(dm, value->GetBool());
			case ValueTypeString:	return new ConstStringExpression(dm, value->GetString());
			default:				assert(false);	return (Expression *) 0;
		}
	}

	/////////////////////////////////////////////////////////////////

	const std::string SetExpression::ConvertToString (void) const
		{ return util::toString(*list, std::mem_fun(&Expression::ConvertToString));	}

	DependencyList SetExpression::CreateDependencies (void) const {
		DependencyList ret;
		for(ExprList::const_iterator i = list->begin(); i != list->end(); ++i) {
			DependencyList l = (*i)->CreateDependencies();
			ret.merge(l);
		}
		return ret;
	}

	ExprValue* SetExpression::Evaluate (DecisionMaker *dm) const {
		ExprValueList *l = new ExprValueList;
		unsigned count = 1;
		for(ExprList::const_iterator i = list->begin(); i != list->end(); ++i, ++count) {
			ExprValue *val = (*i)->Evaluate(dm);
			assert(val);
			if(val->IsError()) {
				util::destroyContainer(l);
				const std::string error = " : " + val->GetError();
				SET_ERROR_WITH_THREE_ARGS(val, "EvaluationSetInvalidArguments", GetLine(), count, error.c_str());
				return val;
			}
			else
				l->push_back(val);
		}
		return new ExprValue(l);
	}

	/////////////////////////////////////////////////////////////////

	const std::string RangeListExpression::ConvertToString (void) const {
		return list->size() == 1 ? list->front()->ConvertToString() : 
			util::toString(*list, std::mem_fun(&Expression::ConvertToString), "[]");
	}

	DependencyList RangeListExpression::CreateDependencies (void) const {
		DependencyList ret;
		for(ExprList::const_iterator i = list->begin(); i != list->end(); ++i) {
			DependencyList l = (*i)->CreateDependencies();
			ret.merge(l);
		}
		return ret;
	}

	ExprValue* RangeListExpression::Evaluate (DecisionMaker *dm) const {
		RangeList *l = new RangeList;
		unsigned count = 1;
		for(ExprList::const_iterator i = list->begin(); i != list->end(); ++i, ++count) {
			ExprValue *val = (*i)->Evaluate(dm);
			assert(val);
			if(val->IsRange()) {
				l->push_back(val->GetRange());
				delete val;
			}
			else {
				delete l;
				const std::string error = val->IsError() ? " : " + val->GetError() : "";
				SET_ERROR_WITH_THREE_ARGS(val, "EvaluationRangeListInvalidArguments", GetLine(), count, error.c_str());
				return val;
			}
		}
		l->Normalize();
		return new ExprValue(l);
	}

	/////////////////////////////////////////////////////////////////

	void ParamsExpression::WriteText (FILE* fp)	const
		{ fputs((std::string("params.\"") + name + "\"").c_str(), fp); }

	const std::string ParamsExpression::ConvertToString	(void) const
		{ return std::string("params.\"") + name + "\""; }

	ExprValue* ParamsExpression::Evaluate (DecisionMaker *dm) const {
		const DecisionMaker::ProfileList& profiles = dm->GetProfileList();
		assert(!profiles.empty());
		ExprValue *ret = (ExprValue *) 0;
		//traverse profile list in reverse order (newer to older)
		for(DecisionMaker::ProfileList::const_reverse_iterator i = profiles.rbegin(); i != profiles.rend(); ++i)
			if((ret = (*i)->GetAttribute(name)))
				break;
		if(ret)
			ret = ret->Clone();
		else {
			ret = new ExprValue;
			SET_ERROR_WITH_TWO_ARGS(ret, "EvaluationNoParamInProfile", GetLine(), name.c_str());
		}
		return ret;
	}

	DependencyList ParamsExpression::CreateDependencies (void) const
		{ return DependencyList(1, DependencyPair(name, true)); }
}
