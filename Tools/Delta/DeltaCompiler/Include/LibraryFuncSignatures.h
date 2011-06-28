// LibraryFuncSignatures.h
// Support for library functions signatures in Delta.
// ScriptFighter Project.
// A. Savidis, September 2009.
//

#ifndef	LIBRARYFUNCSIGNATURES_H
#define	LIBRARYFUNCSIGNATURES_H

#include "DDebug.h"
#include "utypes.h"
#include "ucallbacks.h"
#include "ustlhelpers.h"
#include "LibraryTypes.h"
#include <string>
#include <list>
#include <algorithm>

///////////////////////////////////////////////////////////

class DeltaLibraryFuncSignatures {

	typedef std::list<DeltaLibraryTypeInfo> TypeList;
	public:
	struct Sig {
		TypeList				types;
		std::list<std::string>	names;
		DeltaLibraryTypeInfo	retType;

		bool		HasArg (const std::string& name) const
						{ return std::find(names.begin(), names.end(), name) != names.end(); }
		void		AddArg (const std::string& name, const DeltaLibraryTypeInfo& type)
						{ types.push_back(type); names.push_back(name); }
		bool		MatchesTotalActuals (util_ui32 n) const 
						{ util_ui32 m = TotalExplicitArgs(); return n == m || (n > m && HasVarArgs()); }
		util_ui32	TotalExplicitArgs (void) const
						{ return (util_ui32) types.size() - (HasVarArgs() ? 1 : 0); }
		bool		HasVarArgs (void) const 
						{ return types.size() && types.back().IsBuiltIn() && types.back().GetBuiltIn() == TagAnyValues; }

		Sig (void) : retType(TagVoid) {}
		Sig (const Sig& sig) : 
			types(sig.types), 
			names(sig.names), 
			retType(sig.retType){}
		~Sig(){}
	};

	typedef std::list<Sig> Sigs;

	///////////////////////////////////////////////////////////

	private:
	bool				postErrorOnTypeConflict;
	Sigs				sigs;
	bool				hasError;
	std::string			error;

	void				Error (const std::string& msg) 	{ if (!hasError) { error = msg; hasError = true; } }
	bool				HasError (void) const 			{ return hasError; }
	const std::string	GetError (void) const 			{ return error; }

	void				NewSignature (void) 
							{ sigs.push_back(Sig()); DASSERT(!sigs.empty()); }
	void				NewArgument (const DeltaLibraryTypeInfo& type, const std::string& id);
	bool				CheckType (
							const DeltaLibraryTypeInfo& type, 
							const std::string&			id,
							const DeltaTypeTag*			excludedTags,
							util_ui32					n
						);
	void				SetReturnType(const DeltaLibraryTypeInfo& type);
	void				SetPostError (bool val) 
							{ DASSERT(sigs.size() || sigs.size()); postErrorOnTypeConflict = val; }

	static void			OnNewSig (void* closure)
							{ DPTR((DeltaLibraryFuncSignatures*) closure)->NewSignature(); }
	static void			OnNewArg (const DeltaLibraryTypeInfo& argType, const std::string& id, void* closure)
							{ DPTR((DeltaLibraryFuncSignatures*) closure)->NewArgument(argType, id); }
	static void			OnDoneSig (const DeltaLibraryTypeInfo& retType, void* closure)
							{ DPTR((DeltaLibraryFuncSignatures*) closure)->SetReturnType(retType); }
	static void			OnDoneSigs (bool postError, void* closure)
							{ DPTR((DeltaLibraryFuncSignatures*) closure)->SetPostError(postError); }
	
	///////////////////////////////////////////////////////////

	public:
	const Sigs&			GetSignatures (void) const 
							{ return sigs; }
	bool				ShouldPostErrorOnTypeConflict (void) const
							{ return postErrorOnTypeConflict; }
	void				AddAllPlausibleReturnTypes (TypeList* typeList) const;
	void				Append (const DeltaLibraryFuncSignatures& extraSigs)
							{ DASSERT(this != &extraSigs); ulistcopy(extraSigs.sigs, sigs); }
	static DeltaLibraryFuncSignatures*	
						New (const std::string&	sigsDef, std::string* error);

	~DeltaLibraryFuncSignatures(){}

	private:
	DeltaLibraryFuncSignatures (void) : postErrorOnTypeConflict(false), hasError(false){}
};

///////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.

