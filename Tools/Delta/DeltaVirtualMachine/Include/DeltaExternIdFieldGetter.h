// DeltaExternIdFieldGetter.h
// A method to allow extern id support field get (read only)
// used mainly for debugger purposes.
// ScriptFigher Project.
// A. Savidis, July 2008.
//

#ifndef	DELTAEXTERNIDFIELDGETTER_H
#define	DELTAEXTERNIDFIELDGETTER_H

#include "utypes.h"
#include "DeltaStdDefs.h"
#include "DeltaValueTypes.h"
#include <string>
#include <map>
#include <list>

////////////////////////////////////////////////////////////

#define	DELTA_GETBYSTRING_NO_PRECOND \
			(DeltaExternIdFieldGetter::GetByStringPrecondFunc) 0

#define	DELTA_EXTERNID_NO_FIELD_GETTER	(DeltaExternIdFieldGetter*) 0

class DVM_CLASS DeltaExternIdFieldGetter {

	public:
	typedef bool		(*GetBinaryDataFunc)(void* nativeValue, void** data, util_ui32* size, bool* shouldCopy);
	typedef bool 		(*GetByStringFunc) (void* nativeVal, DeltaValue* at);
	typedef bool 		(*GetByStringDynamicFunc) (void* nativeVal, const std::string& key, DeltaValue* at);
	typedef void		(*AddStringKeysDynamicFunc) (void* nativeVal, std::list<std::string>& l);
	typedef bool 		(*GetByStringPrecondFunc) (void* nativeVal);
	typedef bool 		(*GetByNumberFunc) (void* nativeVal, util_ui32 index, DeltaValue* at);
	typedef util_ui32	(*GetByNumberSizeFunc) (void* nativeVal);
	typedef bool		(*ValueValidationFunc)(void* nativeVal);

	struct GetByStringFuncEntry {
		const char*				id;
		GetByStringFunc			func;
		GetByStringPrecondFunc	precond;
	};

	////////////////////////////////////////////////////////////

	private:
	typedef std::pair<GetByStringFunc, GetByStringPrecondFunc> GetByStringPair;
	std::map<std::string, GetByStringPair>	getByString;
	mutable std::list<std::string>			keys;
	GetByNumberFunc							getByNumber;
	GetBinaryDataFunc						getBinaryDataFunc;
	GetByStringDynamicFunc					getByStringDynamic;
	AddStringKeysDynamicFunc				addStringKeysDynamic;
	GetByNumberSizeFunc						getSize;
	std::string								keyToGetKeys;
	ValueValidationFunc						validateValue;
	bool									includeReservedKeys;

	////////////////////////////////////////////////////////////

	public:
	void							SetGetByString (const GetByStringFuncEntry* entries, util_ui32 n);
	void							SetGetByStringDynamic (GetByStringDynamicFunc get);
	void							SetAddStringKeysDynamic (AddStringKeysDynamicFunc addKeys);
	void							SetGetByString (
										GetByStringFunc			get, 
										const std::string&		key, 
										GetByStringPrecondFunc	precond = DELTA_GETBYSTRING_NO_PRECOND
									);
	void							SetGetByNumber (GetByNumberFunc get, GetByNumberSizeFunc getsize);
	void							SetGetBinaryData (GetBinaryDataFunc func);

	void*							GetBinaryData (void* val, util_ui32* size, bool* wasCopied) const;
	static void						ClearBinaryData (void* data);
	bool							Get (void* val, const std::string& key, DeltaValue* at) const;
	bool							Get (void* val, util_ui32 index, DeltaValue* at) const;
	bool							GetAllKeys (void* val, DeltaValue* at) const; // Returns true if can produce keys.
	
	void							SetKeyToGetKeys (const std::string& s);
	bool							HasKeyToGetKeys (void) const;
	const std::string				GetKeyToGetKeys (void) const;

	void							AddStringKeys (std::list<std::string>& l, void* val) const;
	util_ui32						GetNumericKeys (void* val) const;

	bool							IsValidValue (void* val) const;
	void							SetIncludeReservedKeys (bool val) { includeReservedKeys = val; }
	bool							GetIncludeReservedKeys (void) const { return includeReservedKeys; }

	DeltaExternIdFieldGetter (ValueValidationFunc f = (ValueValidationFunc) 0);
	~DeltaExternIdFieldGetter();
};

////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.