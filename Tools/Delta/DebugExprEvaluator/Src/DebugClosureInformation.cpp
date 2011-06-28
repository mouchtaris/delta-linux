// DebugClosureInformation.h
// Holds information about closures and is stored as 
// an extern id in values of the epression evaluator.
// ScriptFighter Project.
// A. Savidis, September 2010.
//

#include "DebugClosureInformation.h"
#include "DeltaByteCodeTypes.h"
#include "DeltaValue.h"
#include "DDebug.h"
#include "ufunctors.h"
#include <algorithm>

#define	INFO_VAL(val)	\
			VGET_INST(DebugClosureInformation, val, CLOSURE_INFO_TYPE_STR)
#define	NOT_A_CLOSURE_VAR	"<not a closure var>"

///////////////////////////////////////////////////////////////

ubag<Validatable*>*			DebugClosureInformation::insts			= (ubag<Validatable*>*) 0;
DeltaExternIdFieldGetter*	DebugClosureInformation::fieldGetter	= (DeltaExternIdFieldGetter*) 0;

bool DebugClosureInformation::IsSingletonCreated(void)
	{ return insts && fieldGetter; }

void DebugClosureInformation::SingletonCreate (void) {
	DASSERT(!IsSingletonCreated()); 
	unew(insts); 
	unew(fieldGetter);
	DPTR(fieldGetter)->SetIncludeReservedKeys(false);	// Hide standard keys.
	DPTR(fieldGetter)->SetGetByStringDynamic(&GetVar);
	DPTR(fieldGetter)->SetAddStringKeysDynamic(&AddStringKeys);
}

void DebugClosureInformation::SingletonDestroy (void) {
	DASSERT(IsSingletonCreated());
	DASSERT(DPTR(insts)->empty());
	udelete(insts); 
	udelete(fieldGetter);
}

void DebugClosureInformation::CleanUp (void) {
	DASSERT(IsSingletonCreated());
	if (!DPTR(insts)->empty()) {
		ubag<Validatable*> copied(*DPTR(insts)); // Necessary, entries auto-remove from 'insts' on destruction.
		std::for_each(
			copied.begin(),
			copied.end(),
			udestroy_first< std::pair<Validatable*, bool> >()
		);
		DASSERT(DPTR(insts)->empty());	// Must have been auto-removed.
	}
}

///////////////////////////////////////////////////////////////

DebugClosureInformation::DebugClosureInformation (
		const DeltaStdFuncInfo*		_func,
		const DeltaClosure*			_closure
	) :
	closure	(_closure),
	func	(_func) {
	DASSERT(IsSingletonCreated());
	DASSERT(DPTR(insts)->find(this) == DPTR(insts)->end());
	DPTR(insts)->insert(this);

	// Produce the offset map so that we can get quickly from a name
	// ot its respective offset.

	const ClosureVarsInfo&	info = func->GetClosureVarsInfo();
	util_ui16 offset = 0;
	for (ClosureVarsInfo::const_iterator i = info.begin(); i != info.end(); ++i)
		offsetMap[i->GetName()] = offset++;
}

///////////////////////////////////////////////////////////////

DebugClosureInformation::~DebugClosureInformation() {
	DASSERT(IsSingletonCreated());
	ubag<Validatable*>::iterator i = DPTR(insts)->find(this);
	DASSERT( i != DPTR(insts)->end());
	DPTR(insts)->remove(i);
}

///////////////////////////////////////////////////////////////

util_ui16 DebugClosureInformation::GetOffset (const std::string& name) const {
	OffsetMap::const_iterator i = offsetMap.find(name);
	return i != offsetMap.end() ? i->second : (util_ui16) offsetMap.size();
}

///////////////////////////////////////////////////////////////

void DebugClosureInformation::ToString (DeltaString* at, void* val) {

	DASSERT(IsSingletonCreated());
	
	std::list<std::string> keys;
	AddStringKeys(val, keys);

	std::string vars;
	for (std::list<std::string>::iterator i = keys.begin(); i != keys.end(); ++i)
		vars += std::string(i != keys.begin() ? "," : "") + *i;

	DPTR(at)->Add(std::string("closure(") + vars + ")");
}

///////////////////////////////////////////////////////////////

bool DebugClosureInformation::GetVar (void* nativeVal, const std::string& name, DeltaValue* val) {

	DebugClosureInformation* info = INFO_VAL(nativeVal);
	DASSERT(info);

	util_ui16 offset = DPTR(info)->GetOffset(name);
	if (!DPTR(info)->IsValidOffset(offset))
		val->FromString(NOT_A_CLOSURE_VAR);
	else
		val->Assign(DPTR(info)->closure->GetVar(offset));
	return true;
}

///////////////////////////////////////////////////////////////

void DebugClosureInformation::AddStringKeys (void* nativeVal, std::list<std::string>& l) {

	DebugClosureInformation* info = INFO_VAL(nativeVal);
	DASSERT(info && DPTR(insts)->find(info) != DPTR(insts)->end());

	const DeltaStdFuncInfo* func = DPTR(info)->func;
	const ClosureVarsInfo&	vars = func->GetClosureVarsInfo();

	for (ClosureVarsInfo::const_iterator i = vars.begin(); i != vars.end(); ++i)
		l.push_back(i->GetName());
}

///////////////////////////////////////////////////////////////

void DebugClosureInformation::ConvertToDelta (
		const DeltaStdFuncInfo* func, 
		const DeltaClosure*		closure, 
		DeltaValue*				at
	){
	DASSERT(IsSingletonCreated());
	DebugClosureInformation* info = DNEWCLASS(DebugClosureInformation, (func, closure));
	at->FromExternId(
		(void*) DPTR(info)->GetSerialNo(),
		DeltaExternId_NonCollectable,
		ToString,
		CLOSURE_INFO_TYPE_STR,
		fieldGetter
	); 
}

///////////////////////////////////////////////////////////////
