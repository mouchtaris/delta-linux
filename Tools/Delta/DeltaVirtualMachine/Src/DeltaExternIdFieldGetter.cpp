// DeltaExternIdFieldGetter.cpp
// A method to allow extern id support field get (read only)
// used mainly for debugger purposes.
// ScriptFigher Project.
// A. Savidis, July 2008.
//

#include "DeltaExternIdFieldGetter.h"
#include "DeltaValue.h"
#include "DeltaExternId.h"
#include "ustlhelpers.h"
#include <algorithm>

////////////////////////////////////////////////////////////

DeltaExternIdFieldGetter::DeltaExternIdFieldGetter (ValueValidationFunc f) :
	getByNumber				((GetByNumberFunc) 0),
	getBinaryDataFunc		((GetBinaryDataFunc)0),
	getByStringDynamic		((GetByStringDynamicFunc) 0),
	addStringKeysDynamic	((AddStringKeysDynamicFunc)0),
	getSize					((GetByNumberSizeFunc) 0),
	validateValue			(f),
	includeReservedKeys		(true)
	{}

DeltaExternIdFieldGetter::~DeltaExternIdFieldGetter() {}

////////////////////////////////////////////////////////////

void DeltaExternIdFieldGetter::SetGetByString (const GetByStringFuncEntry* entries, util_ui32 n) {
	DASSERT(n && entries);
	for(; n--; ++entries)
		SetGetByString(entries->func, entries->id, entries->precond);
}

void DeltaExternIdFieldGetter::SetGetByString (GetByStringFunc get, const std::string& key, GetByStringPrecondFunc precond) {
	DASSERT(!DeltaExternId::IsReservedFieldKey(key));
	if (getByString.find(key) == getByString.end())
		keys.push_back(key);
	getByString[key] = GetByStringPair(get, precond);
}

void DeltaExternIdFieldGetter::SetAddStringKeysDynamic (AddStringKeysDynamicFunc addKeys)
	{ addStringKeysDynamic = addKeys; }

void DeltaExternIdFieldGetter::SetGetByStringDynamic (GetByStringDynamicFunc get)
	{ getByStringDynamic = get; }

void DeltaExternIdFieldGetter::SetGetByNumber(GetByNumberFunc get, GetByNumberSizeFunc getsize) {
	getByNumber = get;
	getSize		= getsize;
}

void DeltaExternIdFieldGetter::SetGetBinaryData (GetBinaryDataFunc func)
	{ getBinaryDataFunc = func; }

////////////////////////////////////////////////////////////

void DeltaExternIdFieldGetter::SetKeyToGetKeys (const std::string& s)
	{ keyToGetKeys = s; }

bool DeltaExternIdFieldGetter::HasKeyToGetKeys (void) const
	{ return !keyToGetKeys.empty(); }

const std::string DeltaExternIdFieldGetter::GetKeyToGetKeys (void) const
	{ return keyToGetKeys; }

////////////////////////////////////////////////////////////

void* DeltaExternIdFieldGetter::GetBinaryData (void* val, util_ui32* size, bool* wasCopied) const {
	if (!getBinaryDataFunc) {
		*size		= 0;
		*wasCopied	= false;
		return (void*) 0;
	}
	else {
		void* data;
		(*getBinaryDataFunc)(val, &data, size, wasCopied);		
		if (*wasCopied)
			data = memcpy(malloc(*size), data, *size);
		return data;
	}
}

////////////////////////////////////////////////////////////

void DeltaExternIdFieldGetter::ClearBinaryData (void* data) {
	DASSERT(data);
	free(data);
}

////////////////////////////////////////////////////////////

bool DeltaExternIdFieldGetter::Get (void* val, const std::string& key, DeltaValue* at) const {
	DASSERT(IsValidValue(val));
	DASSERT(!DeltaExternId::IsReservedFieldKey(key));
	std::map<std::string, GetByStringPair>::const_iterator i = getByString.find(key);
	return	i != getByString.end()	? (*i->second.first)(val, at) : 
			getByStringDynamic		? (*getByStringDynamic)(val, key, at) : false;
}

bool DeltaExternIdFieldGetter::Get (void* val, util_ui32 index, DeltaValue* at) const {
	DASSERT(IsValidValue(val));
	return getByNumber ? (*getByNumber)(val, index, at) : false;
}

////////////////////////////////////////////////////////////

void DeltaExternIdFieldGetter::AddStringKeys (std::list<std::string>& l, void* val) const {
	for (	std::map<std::string, GetByStringPair>::const_iterator i = getByString.begin();
			i != getByString.end();
			++i	)
			if (!i->second.second || (*i->second.second)(val))
				l.push_back(i->first);
	if (addStringKeysDynamic)
		(*addStringKeysDynamic)(val, l);
}

////////////////////////////////////////////////////////////

bool DeltaExternIdFieldGetter::GetAllKeys (void* val, DeltaValue* at) const {

	DASSERT(IsValidValue(val));

	std::list<std::string> allKeys;
	DeltaExternId::AddReservedFieldKeys(&allKeys);

	if (getSize) {
		util_ui32 n = (*getSize)(val);
		allKeys.push_back(
			!n ? "[]" : n == 1 ? "[0]" : uconstructstr("[%d..%d]", 0, n - 1)
		);
	}

	AddStringKeys(allKeys, val);

	bool first = true;
	std::string s;
	for (std::list<std::string>::iterator i = allKeys.begin(); i != allKeys.end(); ++i, first = false)
		s += std::string(!first ? "," : "") + *i;

	DPTR(at)->FromString(s);
	return true;
}

////////////////////////////////////////////////////////////

util_ui32 DeltaExternIdFieldGetter::GetNumericKeys (void* val) const 
	{ return getSize ? (*getSize)(val) : 0; }

////////////////////////////////////////////////////////////

bool DeltaExternIdFieldGetter::IsValidValue (void* val) const
	{ return !validateValue || (*validateValue)(val); }

////////////////////////////////////////////////////////////
