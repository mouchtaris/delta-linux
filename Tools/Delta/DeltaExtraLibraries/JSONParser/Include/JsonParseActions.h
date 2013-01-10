#ifndef	JSONPARSEACTIONS_H
#define	JSONPARSEACTIONS_H

#include "DeltaObject.h"
#include "DeltaValue.h"

namespace JsonParserLoaderActions {

extern DeltaObject *	manage_objectEmpty();

extern DeltaObject *	manage_membersPairs(DeltaObject * obj1, DeltaObject * obj2);
extern DeltaObject *	manage_pair(std::string * key, DeltaValue * val);

extern DeltaObject *	manage_emptyArray();

extern DeltaObject *	manage_elementsValue(DeltaValue * val);
extern DeltaObject *	manage_elementsValues(DeltaValue * val, DeltaObject * vals);

extern DeltaValue *		manage_valueString(std::string * str);
extern DeltaValue *		manage_valueObject(DeltaObject * obj);
extern DeltaValue *		manage_valueArray(DeltaObject * obj);
extern DeltaValue *		manage_valueTrue();
extern DeltaValue *		manage_valueFalse();

extern DeltaValue *		manage_valueNil(bool retainNull);

extern DeltaValue *		manage_numberInteger(double val);
extern DeltaValue *		manage_numberFloat(double val);
extern DeltaValue *		manage_numberExpInteger(std::string *val);
extern DeltaValue *		manage_numberExpFloat(std::string *val);

}

#endif