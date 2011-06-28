// LibraryTypes.h
// Types for library functions in Delta.
// ScriptFighter Project.
// A. Savidis, June 2010.
//

#ifndef	LIBRARYTYPES_H
#define	LIBRARYTYPES_H

#include "DDebug.h"
#include "utypes.h"
#include "TypeTag.h"
#include <string>
#include <list>
#include <algorithm>

///////////////////////////////////////////////////////////

enum DeltaLibraryMetaType {
	MetaTypeBuiltIn			= 0,
	MetaTypeUserDefined		= 1,
	MetaTypeInclusiveList	= 2,
	MetaTypeExclusiveList	= 3
};

///////////////////////////////////////////////////////////

class DeltaLibraryUserDefinedType {

	private:
	typedef ubag<const DeltaLibraryUserDefinedType*> BaseBag;
	BaseBag		baseTypes;
	std::string	name;
	std::string	fullPath;

	//***************************

	public:
	bool				SameAs (const DeltaLibraryUserDefinedType* type) const
							{ return this == DNULLCHECK(type) || fullPath ==  type->fullPath; }

	bool				SameAs (const DeltaLibraryUserDefinedType& type) const
							{ return SameAs(&type); }

	bool				HasDirectBaseType (const DeltaLibraryUserDefinedType* baseType) const
							{ return baseTypes.in(baseType); }

	const BaseBag&		GetBaseTypes (void) const 
							{ return baseTypes; }

	bool				HasBaseType (const DeltaLibraryUserDefinedType* baseType) const {
							if (HasDirectBaseType(baseType))
								return true;
							for (BaseBag::const_iterator i = baseTypes.begin(); i != baseTypes.end(); ++i)
								if (i->first->HasBaseType(baseType))
									return true;
							return false;
						}
	
	void				AddBaseType (const DeltaLibraryUserDefinedType* baseType)
							{ DASSERT(baseType != this); baseTypes.insert(baseType); }

	const std::string&	GetName (void) const 
							{ return name; }

	const std::string&	GetFullPath (void) const 
							{ return fullPath; }

	DeltaLibraryUserDefinedType (const std::string& _name, const std::string& _fullPath) :
		name(_name), fullPath(_fullPath){}
	~DeltaLibraryUserDefinedType(){}
};

///////////////////////////////////////////////////////////

class DeltaLibraryTypeInfo {

	public:
	typedef std::list<DeltaLibraryTypeInfo> TypeList;

	//***************************

	private:
	DeltaLibraryMetaType metaType;
	union {
		DeltaTypeTag						builtIn;
		const DeltaLibraryUserDefinedType*	userDefined;
		TypeList*							typeList;
	} data;

	struct IsTypeListPred : public std::unary_function<DeltaLibraryTypeInfo, bool> {
		bool operator()(const DeltaLibraryTypeInfo& info) const
			{ return info.IsTypeList(); }
	};

	//***************************

	public:
	bool			IsBuiltIn (void) const
						{ return metaType == MetaTypeBuiltIn; }
	bool			IsExternId (void) const
						{ return metaType == MetaTypeBuiltIn && data.builtIn == TagExternId; }
	bool			IsVoid (void) const
						{ return metaType == MetaTypeBuiltIn && data.builtIn == TagVoid; }
	bool			IsAnyValue (void) const
						{ return metaType == MetaTypeBuiltIn && data.builtIn == TagAnyValue; }
	bool			IsAnyValues (void) const
						{ return metaType == MetaTypeBuiltIn && data.builtIn == TagAnyValues; }
	DeltaTypeTag	GetBuiltIn (void) const 
						{ DASSERT(metaType == MetaTypeBuiltIn); return data.builtIn; }

	bool			IsUserDefined (void) const
						{ return metaType == MetaTypeUserDefined; }
	const DeltaLibraryUserDefinedType*
					GetUserDefined (void) const
						{ DASSERT(metaType == MetaTypeUserDefined); return data.userDefined; }
	bool			IsInTypeList (DeltaTypeTag builtIn) const;
	bool			IsTypeList (void) const
						{ return metaType == MetaTypeInclusiveList || metaType == MetaTypeExclusiveList; }
	bool			IsInclusiveTypeList (void) const
						{ return metaType == MetaTypeInclusiveList; }
	bool			IsExclusiveTypeList (void) const
						{ return metaType == MetaTypeExclusiveList; }
	const TypeList*	GetTypeList (void) const
						{ DASSERT(IsTypeList()); return data.typeList; }
	static void		AddInTypeListIfNotInside (TypeList* typeList, const DeltaLibraryTypeInfo& info);
	static void		AppendInTypeListIfNotInside (TypeList* typeList, const TypeList& from);

	const std::string
					ToString (void) const;

	//***************************
	// Primary type matching among a library-function formal argument
	// and an actual argument coming from Delta (libfunc invocation).

	static bool		Match (
						bool (*matchBuiltinFunc)(DeltaTypeTag formalArg, DeltaTypeTag actualArg),
						const DeltaLibraryTypeInfo& formalArg, 
						const DeltaLibraryTypeInfo& actualArg
					);

	static bool		MatchAnyTypeTag (
						const TypeList&	types, 
						util_ui8*		tagMatching,
						util_ui32		n,
						std::string*	typesString = (std::string*) 0
					);

	//***************************

	void			Set (const DeltaLibraryTypeInfo& info)
						{ this->~DeltaLibraryTypeInfo(); new (this) DeltaLibraryTypeInfo(info); }
	void			Set (DeltaTypeTag builtIn)
						{ this->~DeltaLibraryTypeInfo(); new (this) DeltaLibraryTypeInfo(builtIn); }
	void			Set (const DeltaLibraryUserDefinedType* userDefined)
						{ this->~DeltaLibraryTypeInfo(); new (this) DeltaLibraryTypeInfo(userDefined); }
	void			Set (const TypeList& typeList, bool isInclusive)
						{ this->~DeltaLibraryTypeInfo(); new (this) DeltaLibraryTypeInfo(typeList, isInclusive); }

	//***************************

	DeltaLibraryTypeInfo (const DeltaLibraryTypeInfo& info);

	DeltaLibraryTypeInfo (DeltaTypeTag builtIn): metaType(MetaTypeBuiltIn) 
		{ data.builtIn = builtIn; }

	DeltaLibraryTypeInfo (const DeltaLibraryUserDefinedType* userDefined): metaType(MetaTypeUserDefined) 
		{ data.userDefined = userDefined; }

	DeltaLibraryTypeInfo (const TypeList& typeList, bool isInclusive);

	~DeltaLibraryTypeInfo();
};

///////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
