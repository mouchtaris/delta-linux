// DeltaValueValidator.cpp
// DeltaValue validator.
// Script Fighter Project.
// A. Savidis, October 1999.
//

#include "DDebug.h"
#include "DeltaValueValidator.h"

DeltaValueValidator* DeltaValueValidator::singletonPtr = (DeltaValueValidator*) 0;

void DeltaValueValidator::SingletonCreate (void) {
	if (!singletonPtr)
		singletonPtr = DNEW(DeltaValueValidator);
}

void DeltaValueValidator::SingletonDestroy (void) {
	if (singletonPtr) {
		DDELETE(singletonPtr);
		singletonPtr = (DeltaValueValidator*) 0;
	}
}

