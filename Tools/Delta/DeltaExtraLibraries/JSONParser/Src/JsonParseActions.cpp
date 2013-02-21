// JsonParserActions.cpp
// Translation schemes for the json format.
// Giannhs Apostolidhs, january 2013.
//

#include "JsonParseActions.h"
#include "JsonLoaderAPI.h"
#include "uptr.h"
#include <list>
#include <algorithm>

static char* 							undeletedString = (char*) 0;
static DeltaValue*						undeletedValue = (DeltaValue*) 0;
static std::list<std::string>			indexHolder;
static std::list<DeltaObject*>			objectHolder;
static std::list<DeltaObject*>			arrayHolder;

namespace JsonObjectDeleter{
	struct ObjectDeleter{
		void operator()(DeltaObject* deltaObj){
			DeltaObject::NativeCodeHelpers::Delete( deltaObj );
		}
	};
}

void JsonParserLoaderActions::Manage_Init(void){
	DASSERT(	indexHolder.empty()		&&
				objectHolder.empty()	&& 
				arrayHolder.empty()		&&
				!undeletedValue			&& 
				!undeletedString		
			);
}

void JsonParserLoaderActions::Manage_Clear(void){
	udeleteunlessnull( undeletedValue );
	udeleteasarrayunlessnull( undeletedString );

	std::for_each(
		arrayHolder.begin(), 
		arrayHolder.end(), 
		JsonObjectDeleter::ObjectDeleter()
	);
	arrayHolder.clear();

	std::for_each(
		objectHolder.begin(), 
		objectHolder.end(), 
		JsonObjectDeleter::ObjectDeleter()
	);
	objectHolder.clear();
	indexHolder.clear();
}

void JsonParserLoaderActions::Manage_SetUndeletedString( char* str ){
	DASSERT(!undeletedString);
	undeletedString = str;
}

////////////////////////////////////object

DeltaObject* JsonParserLoaderActions::Manage_ObjectEmpty()
	{ return DeltaObject::NativeCodeHelpers::NewObject(); }


void JsonParserLoaderActions::Manage_PushNewObject(void)
	{ objectHolder.push_front( DeltaObject::NativeCodeHelpers::NewObject() ); }

DeltaObject* JsonParserLoaderActions::Manage_PopObject(void){
	DASSERT( objectHolder.size()>0 );
	DeltaObject* topObj = objectHolder.front();
	objectHolder.pop_front();

	return topObj;
}

void JsonParserLoaderActions::Manage_PairIndex(const char* key){
	DASSERT( objectHolder.size()>0 );
	DASSERT( key );
	indexHolder.push_front(key);
	DASSERT(undeletedString == key);
	udeleteasarray(undeletedString);
}

void JsonParserLoaderActions::Manage_PairValue(DeltaValue* val){
	DASSERT( objectHolder.size()>0 );
	DASSERT( indexHolder.size()>0 );
	if (val) {
		DPTR(objectHolder.front())->Set( indexHolder.front(), *DPTR( val ) );
		DASSERT(undeletedValue == val);
		udelete(undeletedValue);
	}
	indexHolder.pop_front();
}

/////////////////////////////////array

DeltaObject* JsonParserLoaderActions::Manage_EmptyArray(){ 
	DASSERT( objectHolder.size()>0 ); 
	return DeltaObject::NativeCodeHelpers::NewObject(); 
}

void JsonParserLoaderActions::Manage_PushNewArray(){
	DASSERT( objectHolder.size()>0 ); 
	arrayHolder.push_front( DeltaObject::NativeCodeHelpers::NewObject() );
}

DeltaObject* JsonParserLoaderActions::Manage_PopArray(){
	DASSERT( arrayHolder.size()>0 );
	DASSERT( objectHolder.size()>0 );
	DeltaObject* topObj = arrayHolder.front();
	arrayHolder.pop_front();

	return topObj;
}

void JsonParserLoaderActions::Manage_ElementsValue(DeltaValue* val){
	DASSERT( arrayHolder.size()>0 );
	DASSERT( objectHolder.size()>0 );

	if (val) {
		DeltaObject* topArr = arrayHolder.front();
		DPTR(topArr)->Set((DeltaNumberValueType)DPTR(topArr)->Total(), *DPTR(val));
		DASSERT(undeletedValue == val);
		udelete(undeletedValue);
	}
}

////////////////////////////////////// values

DeltaValue* JsonParserLoaderActions::Manage_ValueString(const char* str){
	DASSERT(str && !undeletedValue);
	DeltaValue* retValue = DNEWCLASS(DeltaValue, (DPTR(str)));

	DASSERT(undeletedString == str);
	udeleteasarray(undeletedString);

	return undeletedValue = DPTR(retValue);
}

static DeltaValue* MakeNewValueFromObject(DeltaObject* obj){
	DASSERT( obj && !undeletedValue );
	undeletedValue = DNEWCLASS( DeltaValue, (obj) );
	DeltaObject::NativeCodeHelpers::Delete( DPTR(obj) );
	return undeletedValue;
}

DeltaValue* JsonParserLoaderActions::Manage_ValueObject(DeltaObject* obj)
	{ return MakeNewValueFromObject(obj); }

DeltaValue* JsonParserLoaderActions::Manage_ValueArray(DeltaObject* obj)
	{ return MakeNewValueFromObject(obj); }

DeltaValue* JsonParserLoaderActions::Manage_ValueTrue(){ 
	DASSERT(!undeletedValue); 
	return undeletedValue = DNEWCLASS(DeltaValue, (true)); 
}

DeltaValue* JsonParserLoaderActions::Manage_ValueFalse(){ 
	DASSERT(!undeletedValue); 
	return undeletedValue = DNEWCLASS(DeltaValue, (false)); 
}

DeltaValue* JsonParserLoaderActions::Manage_ValueNil(bool retainNull){ 
	DASSERT(!undeletedValue); 
	return retainNull ?
		undeletedValue = DNEWCLASS(DeltaValue, (JsonLoaderAPI::Null())) :
		(DeltaValue*) 0; 
}

DeltaValue* JsonParserLoaderActions::Manage_NumberInteger(double val){ 
	DASSERT(!undeletedValue); 
	return undeletedValue = DNEWCLASS(DeltaValue, ((DeltaNumberValueType)val)); 
}

DeltaValue* JsonParserLoaderActions::Manage_NumberFloat(double val){ 
	DASSERT(!undeletedValue); 
	return undeletedValue = DNEWCLASS(DeltaValue, ((DeltaNumberValueType)val)); 
}

DeltaValue* JsonParserLoaderActions::Manage_NumberExpInteger(const char* val){ 
	DASSERT( val && !undeletedValue ); 
	return undeletedValue = JsonParserLoaderActions::Manage_ValueString(val); 
}

DeltaValue* JsonParserLoaderActions::Manage_NumberExpFloat(const char* val){ 
	DASSERT( val && !undeletedValue ); 
	return undeletedValue = JsonParserLoaderActions::Manage_ValueString(val); 
}
