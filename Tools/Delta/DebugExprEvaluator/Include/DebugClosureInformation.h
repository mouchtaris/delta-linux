// DebugClosureInformation.h
// Holds information about closures and is stored as 
// an extern id in values of the epression evaluator.
// ScriptFighter Project.
// A. Savidis, September 2010.
//

#ifndef	DEBUGCLOSUREINFORMATION_H
#define	DEBUGCLOSUREINFORMATION_H

#include "uvalidatable.h"
#include "utypes.h"
#include "usingleton.h"
#include "DeltaClosure.h"
#include "DeltaExternIdFieldGetter.h"
#include "DeltaByteCodeTypes.h"
#include <string>
#include <map>

#define	CLOSURE_INFO_TYPE_STR "<closure>"

///////////////////////////////////////////////////////////////

class DebugClosureInformation : public Validatable {
	
	private:
	static DeltaExternIdFieldGetter*	fieldGetter;
	static ubag<Validatable*>*			insts;

	typedef DeltaStdFuncInfo::ClosureVarsInfo	ClosureVarsInfo;
	typedef std::map<std::string, util_ui16>	OffsetMap;

	const DeltaClosure*				closure;
	const DeltaStdFuncInfo*			func;
	OffsetMap						offsetMap;
		
	USINGLETON_APISTYLE_DECLARE_PRIVATESTDMETHODS

	bool							IsValidOffset (util_ui16 offset) const
											{ return offset < (util_ui16) offsetMap.size(); }
	util_ui16						GetOffset (const std::string& name) const;

	static void						ToString (DeltaString* at, void* val);	// For extern ids.
	const char*						GetExtClassString (void) const		
										{ return CLOSURE_INFO_TYPE_STR; }
	const DeltaClosure*				GetClosure (void) const
										{ return closure; }
	const DeltaStdFuncInfo*			GetFunc (void) const
										{ return func; }

	DYNAMIC_CAST_NOBASE_INLINE_IMPL(CLOSURE_INFO_TYPE_STR)

	static bool						GetVar (void* info, const std::string& name, DeltaValue* val);
	static void						AddStringKeys (void* info, std::list<std::string>& l);

	DebugClosureInformation (
		const DeltaStdFuncInfo*		_func,
		const DeltaClosure*			_closure
	);
	virtual ~DebugClosureInformation();

	///////////////////////////////////////////////////////////////

	public:
	USINGLETON_APISTYLE_DECLARE_PUBLICSTDMETHODS
	static void						Initialise (void);	
	static void						CleanUp (void);		// Deletes all created instances.

	static void						ConvertToDelta (
										const DeltaStdFuncInfo*	func,
										const DeltaClosure*		closure, 
										DeltaValue*				at
									);
};

///////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.

