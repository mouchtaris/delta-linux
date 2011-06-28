// LibraryFuncSignatures.cpp
// Support for library functions signatures in Delta.
// ScriptFighter Project.
// A. Savidis, September 2009.
//

#include "LibraryFuncSignatures.h"
#include "LibraryDefsParser.h"
#include "ustrings.h"
#include "ustlhelpers.h"

//---------------------------------------------------------
// LIBFUNC SIGNATURE

void DeltaLibraryFuncSignatures::AddAllPlausibleReturnTypes (TypeList* typeList) const {
	for (Sigs::const_iterator i = sigs.begin(); i != sigs.end(); ++i)
		DeltaLibraryTypeInfo::AddInTypeListIfNotInside(typeList, i->retType);
}

///////////////////////////////////////////////////////////

DeltaLibraryFuncSignatures* DeltaLibraryFuncSignatures::New (const std::string& sigsDef, std::string* error) {

	DeltaLibraryFuncSignatures* sig = DNEW(DeltaLibraryFuncSignatures);

	DeltaLibraryDefsParser parser;
	parser.SetOnNewArg		(&OnNewArg,		sig);
	parser.SetOnNewSig		(&OnNewSig,		sig);
	parser.SetOnDoneSig		(&OnDoneSig,	sig);
	parser.SetOnDoneSigs	(&OnDoneSigs,	sig);

	parser.ParseSigDefinition(sigsDef);

	if (parser.HasError()) 
		{ *error = parser.GetError(); udelete(sig); }
	else
	if (DPTR(sig)->HasError())
		{ *error = DPTR(sig)->GetError(); udelete(sig); }

	return sig;
}

///////////////////////////////////////////////////////////

static DeltaTypeTag illegalReturnTags[] = { TagAnyValues, TagNil };

void DeltaLibraryFuncSignatures::SetReturnType (const DeltaLibraryTypeInfo& type) {
	if (CheckType(type, "return type", illegalReturnTags, uarraysize(illegalReturnTags)))
		sigs.back().retType.Set(type);
}

///////////////////////////////////////////////////////////

bool DeltaLibraryFuncSignatures::CheckType (
		const DeltaLibraryTypeInfo& type, 
		const std::string&			id,
		const DeltaTypeTag*			excludedTags,
		util_ui32					n
	) {

	typedef DeltaLibraryTypeInfo::TypeList TypeList;
	TypeList typeList;

	if (type.IsBuiltIn())
		typeList.push_back(type);
	else
	if (type.IsInclusiveTypeList())
		ulistcopy(*DPTR(type.GetTypeList()), typeList);
	for (; n--; ++excludedTags)
		for (TypeList::const_iterator i = typeList.begin(); i != typeList.end(); ++i)
			if (i->IsBuiltIn() && i->GetBuiltIn() == *excludedTags) {
				Error(
					uconstructstr(
						"illegal use of '%s' type tag for %s in signature",
						DeltaLibraryDefsParser::TypeTagToTagString(i->GetBuiltIn()).c_str(),
						id.c_str()
					)		
				);
				return false;
			}
	return true;
}

//******************************

static DeltaTypeTag illegalArgTags[] = { TagVoid };

void DeltaLibraryFuncSignatures::NewArgument (const DeltaLibraryTypeInfo& type, const std::string& id) {
	DASSERT(!sigs.empty());
	if (CheckType(type, uconstructstr("argument '%s'", id.c_str()), illegalArgTags, uarraysize(illegalArgTags))) {
		Sig& sig = sigs.back();
		if (sig.HasVarArgs())
			Error(uconstructstr("argument '%s' can't follow '...' in signature", id.c_str()));
		else 
		if (sig.HasArg(id))
			Error(uconstructstr("argument '%s' already defined in signature", id.c_str()));
		else {
			if (type.IsExclusiveTypeList()) {							// Explicitly exclude illegal types (if not excluded).
				DeltaLibraryTypeInfo::TypeList typeList;
				ulistcopy(*DPTR(type.GetTypeList()), typeList);
				DeltaLibraryTypeInfo::AddInTypeListIfNotInside(&typeList, DeltaLibraryTypeInfo(TagVoid));
				DeltaLibraryTypeInfo::AddInTypeListIfNotInside(&typeList, DeltaLibraryTypeInfo(TagNil));
				sig.AddArg(id, DeltaLibraryTypeInfo(typeList, false));	// Make new exclusive type list.
			}
			else
				sig.AddArg(id, type);
		}
	}
}

//---------------------------------------------------------
