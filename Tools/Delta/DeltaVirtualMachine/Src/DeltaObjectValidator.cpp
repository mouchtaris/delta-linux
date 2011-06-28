// DeltaObjectValidator.h
// Script Fighter Project.
// A. Savidis, December 2005.
//

#include "DDebug.h"
#include "DeltaObjectValidator.h"

DeltaObjectValidator* DeltaObjectValidator::singletonPtr = (DeltaObjectValidator*) 0;

void DeltaObjectValidator::SingletonCreate (void) 
	{ unew(singletonPtr); }

void DeltaObjectValidator::SingletonDestroy (void)
	{ udelete(singletonPtr); }

DeltaObjectValidator* DeltaObjectValidator::GetPtr (void) 
	{ return DPTR(singletonPtr); }