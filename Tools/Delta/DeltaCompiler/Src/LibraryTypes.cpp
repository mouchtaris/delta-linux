// LibraryTypes.cpp
// Types for library functions in Delta.
// ScriptFighter Project.
// A. Savidis, June 2010.
//

#include "LibraryTypes.h"
#include "LibraryDefsParser.h"
#include "ustlhelpers.h"

///////////////////////////////////////////////////////////

DeltaLibraryTypeInfo::DeltaLibraryTypeInfo (const DeltaLibraryTypeInfo& info) {
	metaType	= info.metaType;
	data		= info.data;

	if (info.IsTypeList()) {
		data.typeList = DNEW(TypeList);
		ulistcopy(*DPTR(info.GetTypeList()), *DPTR(data.typeList));
	}
}

//***************************

DeltaLibraryTypeInfo::DeltaLibraryTypeInfo (const TypeList& typeList, bool isInclusive) {
	DASSERT(!typeList.empty());
	DASSERT(
		std::find_if(	// No typelists of typelists allowed.
			typeList.begin(),
			typeList.end(),
			IsTypeListPred()
		) == typeList.end()
	);

	metaType = isInclusive ? MetaTypeInclusiveList : MetaTypeExclusiveList;
	data.typeList = DNEW(TypeList);
	ulistcopy(typeList, *DPTR(data.typeList));
}

//***************************

DeltaLibraryTypeInfo::~DeltaLibraryTypeInfo() {
	if (IsTypeList())
		{ DDELETE(data.typeList); unullify(data.typeList); }
}

///////////////////////////////////////////////////////////

void DeltaLibraryTypeInfo::AddInTypeListIfNotInside (TypeList* l, const DeltaLibraryTypeInfo& info) {

	DASSERT(!info.IsTypeList());

	for (TypeList::iterator i = l->begin(); i != l->end(); ++i)
		if ((info.IsBuiltIn() && i->IsBuiltIn() && i->GetBuiltIn() == info.GetBuiltIn()) ||
			(info.IsUserDefined() && i->IsUserDefined() && i->GetUserDefined() == info.GetUserDefined()))
			return;
	l->push_back(info);
}

void DeltaLibraryTypeInfo::AppendInTypeListIfNotInside (TypeList* l, const TypeList& from) {
	for (TypeList::const_iterator i = from.begin(); i != from.end(); ++i)
		AddInTypeListIfNotInside(l, *i);
}

///////////////////////////////////////////////////////////

const std::string DeltaLibraryTypeInfo::ToString (void) const {
	if (IsBuiltIn())
		return DeltaLibraryDefsParser::TypeTagToTagString(GetBuiltIn());
	else
	if (IsUserDefined())
		return GetUserDefined()->GetFullPath();
	else {
		DASSERT(IsTypeList());

		std::string s(IsInclusiveTypeList() ? "in {" : "not in {");

		TypeList* l = data.typeList;
		for (TypeList::const_iterator i = DPTR(l)->begin(); i != DPTR(l)->end(); ++i) {
			if (i != DPTR(l)->begin())
				s.append(",");
			s.append(i->ToString());
		}
		
		return s + "}";
	}
}

///////////////////////////////////////////////////////////

bool DeltaLibraryTypeInfo::IsInTypeList (DeltaTypeTag builtIn) const {
	DASSERT(IsTypeList());
	TypeList* l = data.typeList;
	for (TypeList::const_iterator i = DPTR(l)->begin(); i != DPTR(l)->end(); ++i)
		if (i->IsBuiltIn() && i->GetBuiltIn() == builtIn)
			return true;
	return false;
}

///////////////////////////////////////////////////////////
// TYPE CHECKER
//
bool DeltaLibraryTypeInfo::Match (
		bool (*matchBuiltinFunc)(DeltaTypeTag actual, DeltaTypeTag formal),
		const DeltaLibraryTypeInfo& formalArg, 
		const DeltaLibraryTypeInfo& actualArg
	) {

	DASSERT(matchBuiltinFunc);													// Feature not implemented yet.
	if (formalArg.IsAnyValue() || formalArg.IsAnyValues())						// Accepts any or ...
		return true;
	else
	if (actualArg.IsAnyValue())													// Argument is var
		return true;
	else
	if (actualArg.IsTypeList()) {
		const TypeList*	typeList = actualArg.GetTypeList();
		for (TypeList::const_iterator i = DPTR(typeList)->begin(); i != DPTR(typeList)->end(); ++i) {
			DASSERT(!i->IsTypeList());
			if (Match(matchBuiltinFunc, formalArg, *i))
				return true;													// If any plausible actual matches, succeed.
		}
		return false;
	}
	else
	if (formalArg.IsBuiltIn())			// BUILT-IN
		if (formalArg.IsExternId())
			return actualArg.IsUserDefined() || actualArg.IsExternId();			// ExternId is supertype of user-defined types.
		else
		if (!actualArg.IsBuiltIn())
			return false;
		else
			return (*matchBuiltinFunc)(actualArg.GetBuiltIn(), formalArg.GetBuiltIn());
	else
	if (formalArg.IsUserDefined())		// USER-DEFINED
		if (actualArg.IsUserDefined())
			if (formalArg.GetUserDefined()->SameAs(actualArg.GetUserDefined()))	// Identicial user-defined types.
				return true;
			else
				return	actualArg.GetUserDefined()->HasBaseType(				// Up casting of user-defined types.
							formalArg.GetUserDefined()
						);	
		else
			return false;														// Can't match.
	else {								// TYPE LIST

		DASSERT(formalArg.IsTypeList());
		const TypeList*	typeList = formalArg.GetTypeList();

		for (TypeList::const_iterator i = DPTR(typeList)->begin(); i != DPTR(typeList)->end(); ++i) {
			DASSERT(!i->IsTypeList());
			if (Match(matchBuiltinFunc, *i, actualArg))
				return !formalArg.IsExclusiveTypeList();					// Inclusive matches, exclusive fails.
		}

		return formalArg.IsExclusiveTypeList();								// Inclusive fails, exclusive matches.
	}
}

///////////////////////////////////////////////////////////

bool DeltaLibraryTypeInfo::MatchAnyTypeTag (
		const TypeList&	types, 
		util_ui8*		tagMatching,
		util_ui32		n,
		std::string*	typesString
	) {

	DASSERT(n == TOTAL_TYPE_TAGS);
	if (typesString)
		*typesString = types.size() > 1 ? "one of '" : "'";

	for (TypeList::const_iterator i = types.begin(); i != types.end(); ++i) {

		for (util_ui32 j = 0; j < n; ++j)
			if (tagMatching[j] == 1) {
				if (i->IsBuiltIn() && ((DeltaTypeTag) j) == i->GetBuiltIn())
					return true;
				else
				if (((DeltaTypeTag) j) == TagExternId && i->IsUserDefined())	// Extern ids match user defined types.
					return true;
			}

		if (typesString) {
			if (i !=  types.begin())
				typesString->append(", ");
			typesString->append(i->ToString());
		}
	}

	return false;
}

///////////////////////////////////////////////////////////
