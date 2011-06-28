// DeltaClosureCreator.h
// Creator of function closures.
// Delta VirtualMachine implementation.
// ScriptFighter Project.
// A. Savidis, September 2010.
//

#ifndef	DELTACLOSURECREATOR_H
#define	DELTACLOSURECREATOR_H

#include "DeltaValueTypes.h"
#include "DeltaByteCodeTypes.h"
#include "DeltaClosure.h"
#include <list>
#include <map>

/////////////////////////////////////////////////////////////

class DVM_CLASS DeltaClosureCreator {

	private:
	typedef std::list<DeltaClosure*> ClosureList;
	typedef std::map<DeltaValue*, DeltaClosureVarProxy*> VarMap;
	
	ClosureList				closures;
	VarMap					usedVars;

	DeltaClosureVarProxy*	Get (DeltaValue* val);
	DeltaClosureVarProxy*	New (DeltaValue* val);

	bool					AreAllClosuresUsingMyCreatedVarsAsDynamicOnly (void) const;

	public:
	void					HandlePopStackVars (		// inclusively sp[0]...sp[n - 1]
								DeltaVirtualMachine*	vm,
								DeltaValue*				sp, 
								util_ui32				n
							);
	void					HandlePopAllStackVars (DeltaVirtualMachine* vm);	// Used in case of error or unwinding.
	DeltaClosure*			NewClosure (
								const DeltaStdFuncInfo::ClosureVarsInfo&	info,
								DeltaVirtualMachine*						vm
							);

	DeltaClosureCreator(void);
	~DeltaClosureCreator();
};

/////////////////////////////////////////////////////////////

#endif	// Do not ad stuff beyond this point.
