/**
 *	ExprValue.cpp
 *
 *	-- DMSL Project --
 *
 *	Implementation of the Exprvalue class.
 *
 *	Giannis Lilis <lilis@ics.forth.gr>
 *	August 2006 (Version 1) - July 2007 (Version 2)
 */

#include "ExprValue.h"
#include <string.h>

namespace dmsl {

	bool RangeList::IsNormalized(void) const {
		if(empty())
			return true;
		else {
			Range prev = front();
			const_iterator i = begin();
			for(++i; i != end(); ++i)
				if(prev.second >= i->first)
					return false;
				else
					prev = *i;
			return true;
		}
	}

	const RangeList RangeList::Normalize(const RangeList& rangeList) {
		RangeList result;
		RangeList copy = rangeList;
		while(!copy.empty()) {
			double min = copy.front().first;
			iterator minIter = copy.begin();
			for(iterator i = copy.begin(); i != copy.end(); ++i)
				if(i->first < min)
					min = i->first, minIter = i;
			
			result.push_back(*minIter);
			copy.erase(minIter);
			for(iterator i = copy.begin(); i != copy.end(); /*empty*/)
				if(i->first <= result.back().second) {
					if(i->second > result.back().second)
						result.back().second = i->second;
					i = copy.erase(i);
				}
				else
					++i;
		}
		return result;
	}

	//*********************

	inline const std::string RangeList::RangeToString(const Range& range)
		{ return util::MakeString("%lf..%lf", range.first, range.second);	}

	inline const std::string RangeList::toString(void) const 
		{ return util::toString(*this, RangeToString, "[]"); }

	//*********************

	bool RangeList::operator<=(const RangeList& rangeList) {
		assert(IsNormalized() && rangeList.IsNormalized());
		for(const_iterator i = rangeList.begin(); i != rangeList.end(); ++i) {
			const_iterator iter;
			for(iter = begin(); iter != end(); ++iter)
				if(i->first >= iter->first && i->second <= iter->second)
					break;
			if(iter == end())
				return false;
		}
		return true;
	}

	inline bool RangeList::operator==(const RangeList& rangeList)
		{ return *this <= (rangeList) && rangeList <= *this; }

	//////////////////////////////////////////////////////////////////////////////////////////////////

	void ExprValue::Clear (void) {
		if (type == ValueTypeString && content.strVal) {
			delete content.strVal;
			content.strVal = (std::string *) 0;
		}
		else if (type == ValueTypeRange && content.rangeVal) {
			delete content.rangeVal;
			content.rangeVal = (Range *) 0;
		}
		else if (type == ValueTypeRangeList && content.rangeListVal) {
			delete content.rangeListVal;
			content.rangeListVal = (RangeList *) 0;
		}
		else if (type == ValueTypeList && content.listVal) {
			DeleteList(content.listVal);
			content.listVal = (ExprValueList *) 0;
		}
		else if (type == ValueTypeError && content.errorVal) {
			delete content.errorVal;
			content.errorVal = (std::string *) 0;
		}
	}

	//*********************

	double ExprValue::GetNumber (void) const {
		assert(type == ValueTypeInt || type == ValueTypeReal);
		if (type == ValueTypeInt)
			return content.intVal;
		else
			return content.realVal;
	}

	void ExprValue::SetNumber (double val) {
		Clear();
		if (val == (int) val) { // Is actually an integer ?
			type = ValueTypeInt;
			content.intVal = (int) val;
		}
		else {
			type = ValueTypeReal;
			content.realVal = val;
		}
	}

	//*********************

	const std::string& ExprValue::GetString (void) const {
		assert(type == ValueTypeString);
		return *content.strVal;
	}

	void ExprValue::SetString (const std::string& str) {
		Clear();
		type = ValueTypeString;
		content.strVal = new std::string(str);
	}


	//*********************

	bool ExprValue::GetBool (void) const {
		assert(type == ValueTypeBool);
		return content.boolVal;
	}

	void ExprValue::SetBool (bool val) {
		Clear();
		type = ValueTypeBool;
		content.boolVal = val;
	}

	//*********************

	Range& ExprValue::GetRange (void) {
		assert(type == ValueTypeRange);
		return *content.rangeVal;
	}

	const Range& ExprValue::GetRange (void) const {
		assert(type == ValueTypeRange);
		return *content.rangeVal;
	}

	void ExprValue::SetRange (const Range& range) {
		Clear();
		type = ValueTypeRange;
		content.rangeVal = new Range(range);
	}

	//*********************

	RangeList& ExprValue::GetRangeList (void) {
		assert(type == ValueTypeRangeList);
		return *content.rangeListVal;
	}

	const RangeList& ExprValue::GetRangeList (void) const {
		assert(type == ValueTypeRangeList);
		return *content.rangeListVal;
	}

	void ExprValue::SetRangeList (const RangeList& rangeList) {
		Clear();
		type = ValueTypeRangeList;
		content.rangeListVal = rangeList.Clone();
	}

	//*********************

	ExprValueList& ExprValue::GetList (void) {
		assert(type == ValueTypeList);
		return *content.listVal;
	}

	const ExprValueList& ExprValue::GetList (void) const {
		assert(type == ValueTypeList);
		return *content.listVal;
	}

	void ExprValue::SetList (const ExprValueList& list) {
		Clear();
		type = ValueTypeList;
		content.listVal = CloneList(list);
	}

	//*********************

	const std::string ExprValue::GetError(void) const {
		assert(type == ValueTypeError);
		return *content.errorVal;
	}

	void ExprValue::SetError (const std::string& error) {
		Clear();
		type = ValueTypeError;
		content.errorVal = new std::string(error);
	}

	//*********************

	void ExprValue::SetVoid (void) {
		Clear();
		type = ValueTypeVoid;
	}

	//*********************

	static bool isSubList(const ExprValueList& list1, const ExprValueList& list2) {
		for(ExprValueList::const_iterator i = list1.begin(); i != list1.end(); ++i) {
			ExprValueList::const_iterator iter;
			for(iter = list2.begin(); iter != list2.end(); ++iter)
				if(**i == **iter)
					break;
			if(iter == list2.end())
				return false;
		}
		return true;
	}

	static bool compareListsAsSets(const ExprValueList& list1, const ExprValueList& list2)
		{ return isSubList(list1, list2) && isSubList(list2, list1); }

	bool ExprValue::operator ==(const ExprValue& expr) {
		if(IsNumber() && expr.IsNumber())
			return GetNumber() == expr.GetNumber();
		else if(type != expr.type)
			return false;
		else
			switch(type) {
				case ValueTypeBool:		return content.boolVal	 == expr.content.boolVal;
				case ValueTypeString:	return *content.strVal	 == *expr.content.strVal;
				case ValueTypeError:	return *content.errorVal == *expr.content.errorVal;
				case ValueTypeRange:	return *content.rangeVal == *expr.content.rangeVal;
				case ValueTypeRangeList:return *content.rangeListVal == *expr.content.rangeListVal;
				case ValueTypeList:		return compareListsAsSets(*content.listVal, *expr.content.listVal);
				case ValueTypeVoid:		return true;
				default:				assert(false);	return false;	
			}
	}

	//*********************

	const std::string ExprValue::toString(void) const {
		switch(type) {
			case ValueTypeInt:
			case ValueTypeReal:		return util::toString(GetNumber());
			case ValueTypeBool:		return util::toString(GetBool());
			case ValueTypeString:	return "\"" + GetString() + "\"";
			case ValueTypeError:	return "\"" + GetError() + "\"";
			case ValueTypeRange:	return RangeList::RangeToString(*content.rangeVal);
			case ValueTypeRangeList:return content.rangeListVal->toString();
			case ValueTypeList:		return util::toString(*content.listVal, std::mem_fun(&ExprValue::toString));
			case ValueTypeVoid:		return "void";
			default:				assert(false);	return "";
		}
	}

	//*********************

	ExprValue *ExprValue::Clone(void) const { return new ExprValue(*this); }

	ExprValue::ExprValue (const ExprValue& expr) {
		//shallow copy at first
		memcpy(this, &expr, sizeof(ExprValue));
		//then copy deep if necessary
		if(type == ValueTypeString)
			content.strVal = new std::string(*content.strVal);
		else if(type == ValueTypeRange)
			content.rangeVal = new Range(*content.rangeVal);
		else if(type == ValueTypeRangeList)
			content.rangeListVal = content.rangeListVal->Clone();
		else if(type == ValueTypeError)
			content.errorVal = new std::string(*content.errorVal);
		else if(type == ValueTypeList)
			content.listVal = CloneList(*content.listVal);
	}
}