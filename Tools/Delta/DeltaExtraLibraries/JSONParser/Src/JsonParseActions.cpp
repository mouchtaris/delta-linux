#include "JsonParseActions.h"
#include "JsonLoaderAPI.h"
#include "uptr.h"

DeltaObject * JsonParserLoaderActions::manage_objectEmpty()
	{ return DeltaObject::NativeCodeHelpers::NewObject(); }

DeltaObject * JsonParserLoaderActions::manage_membersPairs(DeltaObject * obj1, DeltaObject * obj2){
	DPTR(obj1)->Extend(DPTR(obj2));
	DeltaObject::NativeCodeHelpers::GiveUp(DPTR(obj2));
	
	return obj1;
}

DeltaObject * JsonParserLoaderActions::manage_pair(std::string * key, DeltaValue * val){

	 DeltaObject* obj = DeltaObject::NativeCodeHelpers::NewObject();

	 if (val) {
		 DPTR(obj)->Set(*DPTR(key), *DPTR(val));
		 DDELETE(val);
	 }
	 udelete(key);

	 return obj;
}

DeltaObject * JsonParserLoaderActions::manage_emptyArray()
	{ return DeltaObject::NativeCodeHelpers::NewObject(); }

DeltaObject * JsonParserLoaderActions::manage_elementsValue(DeltaValue * val){

	DeltaObject* obj = DeltaObject::NativeCodeHelpers::NewObject();

	if (val) {
		DPTR(obj)->Set((DeltaNumberValueType) 0, *DPTR(val));
		DDELETE(val);
	}

	return DPTR(obj);
}

DeltaObject * JsonParserLoaderActions::manage_elementsValues(DeltaValue * val, DeltaObject * vals){

	if (val) {
		vals->Set((DeltaNumberValueType)vals->Total(), *DPTR(val));
		DDELETE(val);
	}
		
	return vals;
}

DeltaValue * JsonParserLoaderActions::manage_valueString(std::string * str){
	
	DeltaValue* retValue = DNEWCLASS(DeltaValue, (*DPTR(str)));
	udelete(str);

	return DPTR(retValue);
}

DeltaValue * JsonParserLoaderActions::manage_valueObject(DeltaObject * obj)
	{ return DNEWCLASS(DeltaValue, (obj)); }

DeltaValue * JsonParserLoaderActions::manage_valueArray(DeltaObject * obj)
	{ return DNEWCLASS(DeltaValue, (obj)); }

DeltaValue * JsonParserLoaderActions::manage_valueTrue()
	{ return DNEWCLASS(DeltaValue, (true)); }

DeltaValue * JsonParserLoaderActions::manage_valueFalse()
	{ return DNEWCLASS(DeltaValue, (false)); }

DeltaValue * JsonParserLoaderActions::manage_valueNil(bool retainNull)
	{ return retainNull ? DNEWCLASS(DeltaValue, (JsonLoaderAPI::Null())) : (DeltaValue*) 0; }

DeltaValue * JsonParserLoaderActions::manage_numberInteger(double val)
	{ return DNEWCLASS(DeltaValue, ((DeltaNumberValueType)val)); }

DeltaValue * JsonParserLoaderActions::manage_numberFloat(double val)
	{ return DNEWCLASS(DeltaValue, ((DeltaNumberValueType)val)); }

DeltaValue * JsonParserLoaderActions::manage_numberExpInteger(std::string *val)
	{ return JsonParserLoaderActions::manage_valueString(val); }

DeltaValue * JsonParserLoaderActions::manage_numberExpFloat(std::string *val)
	{ return JsonParserLoaderActions::manage_valueString(val); }
