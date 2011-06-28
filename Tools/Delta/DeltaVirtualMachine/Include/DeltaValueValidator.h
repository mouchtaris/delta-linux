// DeltaValueValidator.h
// DeltaValue validator.
// ScriptFighter Project.
// A. Savidis, October 1999.
//

#ifndef	DELTAVALUEVALIDATOR_H
#define	DELTAVALUEVALIDATOR_H

#include "uvalidator.h"
#include "DeltaStdDefs.h"
#include "DeltaValue.h"

class DVM_CLASS DeltaValueValidator {
	
	private:
	static DeltaValueValidator* singletonPtr;
	uvalidator validator;

	public:
	const DeltaValue*	Validate (const DeltaValue* obj) 
							{ return (DeltaValue*) validator.Validate(obj); }

	void				Register (const DeltaValue* obj) 
							{ validator.Register(obj); }

	void				Cancel (const DeltaValue* obj) 
							{ validator.Cancel(obj); }

	static void			SingletonCreate (void);
	static void			SingletonDestroy (void);

	static DeltaValueValidator*	
						GetPtr (void) { DASSERT(singletonPtr); return singletonPtr; }

	DeltaValueValidator(void){}
	~DeltaValueValidator(){}
};

// Use the following pair of functions to get
// the validator, only if it is alive.
//
inline DeltaValueValidator* GetDeltaValueValidator (void) 
	{ return DeltaValueValidator::GetPtr(); }

inline bool DeltaValueValidatorAlive (void) 
	{ return !!DeltaValueValidator::GetPtr(); }

#endif	// Do not add stuff beyond this point.