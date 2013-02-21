// JsonParserActions.h
// Translation schemes for the json format.
// Giannhs Apostolidhs, january 2013.
//

#ifndef	JSONPARSEACTIONS_H
#define	JSONPARSEACTIONS_H

#include "DeltaObject.h"
#include "DeltaValue.h"

namespace JsonParserLoaderActions {

extern void Manage_Init(void);
extern void Manage_Clear(void);
extern void Manage_SetUndeletedString( char* str );

extern DeltaObject*		Manage_ObjectEmpty(void);
extern void				Manage_PushNewObject(void);
extern DeltaObject*		Manage_PopObject(void);
extern void				Manage_PairIndex(const char* key);
extern void				Manage_PairValue(DeltaValue* val);

extern DeltaObject*		Manage_EmptyArray();
extern void				Manage_PushNewArray();
extern DeltaObject*		Manage_PopArray();
extern void				Manage_ElementsValue(DeltaValue* val);

extern DeltaValue*		Manage_ValueString(const char* str);
extern DeltaValue*		Manage_ValueObject(DeltaObject* obj);
extern DeltaValue*		Manage_ValueArray(DeltaObject* obj);
extern DeltaValue*		Manage_ValueTrue(void);
extern DeltaValue*		Manage_ValueFalse(void);
extern DeltaValue*		Manage_ValueNil(bool retainNull);

extern DeltaValue*		Manage_NumberInteger(double val);
extern DeltaValue*		Manage_NumberFloat(double val);
extern DeltaValue*		Manage_NumberExpInteger(const char* val);
extern DeltaValue*		Manage_NumberExpFloat(const char* val);

}

#endif