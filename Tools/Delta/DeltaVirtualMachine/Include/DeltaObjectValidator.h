// DeltaObjectValidator.h
// ScriptFighter Project.
// A. Savidis, December 2005.
//

#ifndef	DELTAOBJECTVALIDATOR_H
#define	DELTAOBJECTVALIDATOR_H

//-------------------------------------------------------------

#include "uvalidator.h"
#include "DDebug.h"
#include "DeltaStdDefs.h"

class DVM_CLASS DeltaObject;

class DVM_CLASS DeltaObjectValidator {
	
	private:
	static DeltaObjectValidator* singletonPtr;
	uvalidator validator;

	public:
	bool			Validate (DeltaObject* obj) 
						{ return !!validator.Validate(obj); }

	void			Register (DeltaObject* obj) 
						{ validator.Register(obj); }

	void			Cancel (DeltaObject* obj) 
						{ validator.Cancel(obj); }

	//***************************
	
	static void		SingletonCreate (void);					
	static void		SingletonDestroy (void);

	static DeltaObjectValidator*	
					GetPtr (void);

	DeltaObjectValidator(void){}
	~DeltaObjectValidator(){}
};

////////////////////////////////////////////////////////////////

inline DeltaObjectValidator* GetDeltaObjectValidator (void) 
	{ return DeltaObjectValidator::GetPtr(); }

inline bool DeltaObjectValidatorAlive (void) 
	{ return !!DeltaObjectValidator::GetPtr(); }

//-------------------------------------------------------------

#endif	// Do not add stuff beyond this point.