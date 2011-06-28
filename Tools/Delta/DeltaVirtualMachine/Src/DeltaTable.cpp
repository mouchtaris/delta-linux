// DeltaTable.cpp
// Implementation of DeltaTable class.
// Delta Virtual Machine.
// ScriptFighter Project.
// A. Savidis, October 1999.
// Many updates after original version.
// Added attributes (properties), January 2009.
// Extended to support assign and type casting overloading
// when setting object slots (fields or attributes).
// Major Refactoring, February 2011, included per case
// setters / getters and turned the basic functions to templates.
//
#include "DDebug.h"
#include "DeltaTable.h"
#include "DeltaString.h"
#include "DeltaValue.h"
#include "DeltaVirtualMachine.h"
#include "DeltaRuntimeError.h"
#include "DeltaTableFactory.h"
#include "uerrorclass.h"
#include "DeltaVirtualMachine.h"
#include "OperatorOverloading.h"
#include "DeltaExceptionHandling.h"

#ifdef _MSC_VER
#pragma warning (disable : 4102)
#endif

//---------------------------------------------------------------

DeltaTable::Field::Field(
		DeltaTable*			_owner, 
		const DeltaValue&	_index, 
		const DeltaValue&	_content
	) : Element(_owner), 
		index(_index), 
		content(_content) {
	index.SetResidencyType(DeltaValue::Contained,	(CollectableContainer*) owner);
	content.SetResidencyType(DeltaValue::Contained, (CollectableContainer*) owner);
}

//*********************

bool DeltaTable::Field::SetContent (const DeltaValue& val) 
	{ return content.AssignOverloadable(val); }

//*********************

DeltaTable::Element* DeltaTable::Field::Clone (DeltaTable* container) const 
	{ return DPTR(container)->SetNewElement(index, content); }

//*********************

void DeltaTable::Field::CheckChangeSelfForMethods (const DeltaTable* from, DeltaTable* to) {
	DPTR(from)->ChangeSelfIfMethodAndCallerIsSelf(index, to);
	DPTR(from)->ChangeSelfIfMethodAndCallerIsSelf(content, to);
}

///////////////////////////////////////////////////////

#define	ERROR_HANDLER(what, errclass)						\
	uerror::GetSingleton().post##errclass(					\
		"In attribute '%s': error %s!", id.c_str(), what	\
	); goto FAIL;

DeltaTable::Attribute::Attribute (
		DeltaTable*			_owner, 
		const std::string&	_id, 
		const DeltaValue&	_set,
		const DeltaValue&	_get
	) : Element(_owner), 
		id		(_id), 
		set		(_set), 
		setting	(false),
		get		(_get),
		getting	(false) {
	set.SetResidencyType(DeltaValue::Contained,		(CollectableContainer*) owner);
	get.SetResidencyType(DeltaValue::Contained,		(CollectableContainer*) owner);
	content.SetResidencyType(DeltaValue::Contained, (CollectableContainer*) owner);
}

//*********************

DeltaTable::Element* DeltaTable::Attribute::Clone (DeltaTable* container) const {
	Element* elem = DPTR(container)->NewAttribute(id, set, get);
	DeltaValue value(DeltaValue::Nil);
	UCHECK_DOMINO_ERROR(GetContent(&value), "internal clone failed (due to get)");
	UCHECK_DOMINO_ERROR(DPTR(elem)->SetContent(value), "internal clone failed (due to set)");
	return elem;
	FAIL: return (Element*) 0;
}

//*********************

void DeltaTable::Attribute::CheckChangeSelfForMethods (const DeltaTable* from, DeltaTable* to) {
	DPTR(from)->ChangeSelfIfMethodAndCallerIsSelf(set, to);
	DPTR(from)->ChangeSelfIfMethodAndCallerIsSelf(get, to);
	DPTR(from)->ChangeSelfIfMethodAndCallerIsSelf(content, to);
}

//*********************

bool DeltaTable::Attribute::SetContent (const DeltaValue& val) {
	UCHECK_PRIMARY_ERROR_EX(!setting, "cycle during 'set'", setting = false);
	setting = true;

	UCHECK_PRIMARY_ERROR(set.IsCallable(), "'set' slot not a callable");
	UCHECK_PRIMARY_ERROR(set(val), "'set' invocation failed");

	setting = false;
	return true;
	FAIL: return false;
}

bool DeltaTable::Attribute::SetContentDirectly (const DeltaValue& rv) 
	{ return content.AssignOverloadable(rv); }

//*********************

bool DeltaTable::Attribute::GetContent (DeltaValue* val) const {
	UCHECK_PRIMARY_ERROR_EX(!getting, "cycle during 'get'", getting = false);
	getting = true;

	UCHECK_PRIMARY_ERROR(get.IsCallable(), "'get' slot not a callable");
	UCHECK_PRIMARY_ERROR(get(val), "'get' invocation failed");

	getting = false;
	return true;
	FAIL: return false;
}

//---------------------------------------------------------------

#undef	ERROR_HANDLER
#define	ERROR_HANDLER	DELTATABLE_ERROR_HANDLER

DeltaTable::DeltaTable (void) {

	// Overloading-related initializations.
	//
	selfValue.SetResidencyType(DeltaValue::Contained, (CollectableContainer*) this);	// Self contained.
	selfValue.FromTable(this);
	delegatesValue.SetResidencyType(DeltaValue::Contained, (CollectableContainer*) this);
	delegatorsValue.SetResidencyType(DeltaValue::Contained, (CollectableContainer*) this);
	get.SetResidencyType(DeltaValue::Contained, (CollectableContainer*) this);
	set.SetResidencyType(DeltaValue::Contained, (CollectableContainer*) this);

	functorIndex.FromString(DELTA_OPERATOR_OBJECT_FUNCTIONCALL);

	overloadsGet		= 
	overloadsSet		= false;
	overloadingEnabled	= true;

	// Initialise index lists and hash-tables.
	//
	unullify(boolIndex[0]);
	unullify(boolIndex[1]);

	uzeromemory(&tableIndex);
	uzeromemory(&programFuncIndex);
	uzeromemory(&libraryFuncIndex);
	uzeromemory(&externIdIndex);
	uzeromemory(&methodFuncIndex);
	uzeromemory(&numberIndex);
	uzeromemory(&strIndex);

	// Initialise the iteration-specific lists of buckets
	// and hash table sizes.
	//
	util_ui16 i = 0;
	tables[i]		= &numberIndex[0];
	tableSizes[i++] = DELTA_TABLE_NUMBER_HASH_SIZE;

	tables[i]		= &strIndex[0];
	tableSizes[i++] = DELTA_TABLE_STRING_HASH_SIZE;

	tables[i]		= &tableIndex[0];
	tableSizes[i++] = DELTA_TABLE_COMMON_HASH_SIZE;

	tables[i]		= &programFuncIndex[0];
	tableSizes[i++]	= DELTA_TABLE_COMMON_HASH_SIZE;

	tables[i]		= &libraryFuncIndex[0];
	tableSizes[i++]	= DELTA_TABLE_COMMON_HASH_SIZE;

	tables[i]		= &boolIndex[0];
	tableSizes[i++] = 2;	// Just 'true' and 'false' buckets.

	tables[i]		= &externIdIndex[0];
	tableSizes[i++]	= DELTA_TABLE_COMMON_HASH_SIZE;

	tables[i]		= &methodFuncIndex[0];
	tableSizes[i++]	= DELTA_TABLE_COMMON_HASH_SIZE;

	DASSERT(i == DELTA_TABLE_TOTAL_HASHTABLES);

	total			= 0;

	// Initialise the function table for fast getbucket.
	//
	i = 0;
	getBucketFuncs[i++]	= GetBucket_Number;
	getBucketFuncs[i++]	= GetBucket_String;
	getBucketFuncs[i++]	= GetBucket_Table;
	getBucketFuncs[i++]	= GetBucket_ProgramFuncAddress;
	getBucketFuncs[i++]	= GetBucket_LibraryFuncName;
	getBucketFuncs[i++]	= GetBucket_Bool;
	getBucketFuncs[i++]	= GetBucket_ExternId;
	getBucketFuncs[i++]	= GetBucket_Error;				// Nil
	getBucketFuncs[i++]	= GetBucket_MethodFuncAddress;
	getBucketFuncs[i++]	= GetBucket_Error;				// Undefined

	DASSERT(i == TOTAL_GET_BUCKET_FUNCS);

	unullify(delegatesArray);
	unullify(delegatorsArray);
	isDelegatesUpToDate		=
	isDelegatorsUpToDate	=
	searchingInDelegates	=
	callingIsCallable		= 
	cycleCheck				= false;

	currBasicSetFunc						= &DeltaTable::BasicSetNoInternalDelegates;
	currSetFunc								= currBasicSetFunc;
	currBoundedSetFunc						= currBasicSetFunc;
	currSetNewFunc							= &DeltaTable::BasicSetNew;
	currAttrSetFunc							= &DeltaTable::SetAttributeNoInternalDelegates;
	currAttrGetFunc							= &DeltaTable::GetAttributeNoInternalDelegates;
	currAttrGetByUnoverloadedAssignFunc		= &DeltaTable::GetAttributeNoInternalDelegatesByUnoverloadedAssign;

	currChangeFunc							= &DeltaTable::BasicChange;
	currChangeBySkippingUserCodeFunc		= &DeltaTable::BasicChangeBySkippingUserCode;
	currFindFunc							= &DeltaTable::BasicFind;
	currFindBySkippingUserCodeFunc			= &DeltaTable::BasicFindBySkippingUserCode;

	currBasicGetFunc						= &DeltaTable::BasicGetNoInternalDelegates;
	currBasicGetBySkippingUserCodeFunc		= &DeltaTable::BasicGetNoInternalDelegatesBySkippingUserCode;
	currBasicGetByUnoverloadedAssignFunc	= &DeltaTable::BasicGetNoInternalDelegatesByUnoverloadedAssign;

	currBoundedGetFunc						= currBasicGetFunc;
	currBoundedGetByUnoverloadedAssignFunc	= currBasicGetByUnoverloadedAssignFunc;
	currGetFunc								= currBoundedGetFunc;
	currGetByUnoverloadedAssignFunc			= currBoundedGetByUnoverloadedAssignFunc;
	currGetBySkippingUserCodeFunc			= currBasicGetBySkippingUserCodeFunc;
}

DeltaTable::~DeltaTable() {
	
	if (!IsCollected()) {
		DASSERT(DeltaTableFactory::GetSingleton().HasRecycled(this));
		DASSERT(DeltaTableFactory::GetSingleton().JustCallingDestructor(this));
		GetDyingState().SetInProgress(true);
	}

	ExplicitDestructor(); 
}

//---------------------------------------------------------------

bool DeltaTable::IsCallable (void) const {
	if (callingIsCallable)
		return callingIsCallable = false;
	else {
		callingIsCallable = true;
		DeltaValue f;
		bool result = const_cast<DeltaTable*>(this)->GetFunctor(&f) && f.IsCallable();
		callingIsCallable = false;
		return result;
	}
}

//---------------------------------------------------------------

void DeltaTable::ClearBucket (Element* bucket) {

	while (bucket) {
		Element* aux = bucket;
		bucket = DPTR(bucket)->GetNext();
		DDELETE(aux);
	}
}

/////////////////////////////////////////////////////////////////
// Only clear all hash tables.
//
void DeltaTable::Clear (void) {

	DASSERT(total == CountAll());
	usafecontainerbehavior::notify_cleared();

	if (total) {

		ClearBucket(boolIndex[0]);
		ClearBucket(boolIndex[1]);

		boolIndex[0] = boolIndex[1] = (Element*) 0;
		util_ui32 i = 0;
		for (i = 0; i<DELTA_TABLE_COMMON_HASH_SIZE; ++i) {

			ClearBucket(tableIndex[i]);
			ClearBucket(libraryFuncIndex[i]);
			ClearBucket(programFuncIndex[i]);
			ClearBucket(externIdIndex[i]);
			ClearBucket(methodFuncIndex[i]);

			tableIndex[i]		= 
			libraryFuncIndex[i]	=
			programFuncIndex[i]	=
			externIdIndex[i]	= 
			methodFuncIndex[i]	= (Element*) 0;
		}

		for (i = 0; i<DELTA_TABLE_NUMBER_HASH_SIZE; ++i) {
			ClearBucket(numberIndex[i]);
			unullify(numberIndex[i]);
		}

		for (i = 0; i<DELTA_TABLE_STRING_HASH_SIZE; ++i) {
			ClearBucket(strIndex[i]);
			unullify(strIndex[i]);
		}

		total = 0;
	}
}

//---------------------------------------------------------------

util_ui32 DeltaTable::CountBucket (Element* bucket) const {

	util_ui32 n = 0;
	for (n = 0; bucket; ++n, bucket = DPTR(bucket)->GetNext())
		;

	return n;
}

/////////////////////////////////////////////////////////////////

util_ui32 DeltaTable::CountAll (void) const {

	unsigned	n = CountBucket(boolIndex[0]) + CountBucket(boolIndex[1]);
	util_ui32	i;

	for (i = 0; i < DELTA_TABLE_COMMON_HASH_SIZE; ++i)

		n +=	CountBucket(tableIndex[i])		+
				CountBucket(libraryFuncIndex[i])	+
				CountBucket(programFuncIndex[i])	+
				CountBucket(externIdIndex[i])	+
				CountBucket(methodFuncIndex[i]);

	for (i = 0; i<DELTA_TABLE_NUMBER_HASH_SIZE; ++i)
		n += CountBucket(numberIndex[i]);

	for (i=0; i<DELTA_TABLE_STRING_HASH_SIZE; ++i)
		n += CountBucket(strIndex[i]);

	return n;
}

//---------------------------------------------------------------

void DeltaTable::ChangeSelfIfMethodAndCallerIsSelf (DeltaValue& obj, DeltaTable* newSelf) const {

	if (obj.Type() == DeltaValue_MethodFunc) {

		DeltaCodeAddress		f;
		DeltaTable*				oldSelf;
		DeltaVirtualMachine*	oldVM;
		obj.ToMethodFunc(&f, &oldSelf, &oldVM);

		if (oldSelf == this)
			obj.FromMethodFunc(f, newSelf, oldVM);
	}
}

/////////////////////////////////////////////////////////////////

DeltaTable* DeltaTable::NewTable (void)
	{ return DeltaTableFactory::New(); }

DeltaTable* DeltaTable::New (void) const
	{ return DeltaTableFactory::New(); }

/////////////////////////////////////////////////////////////////

bool DeltaTable::Copy (const DeltaTable* from, const Element& elem) {
	if (Element* copy = elem.Clone(this))
		{ DPTR(copy)->CheckChangeSelfForMethods(from, this); return true; }
	else
		return false;
}

/////////////////////////////////////////////////////////////////

DeltaTable*	DeltaTable::Copy (void) const {

	DeltaTable* copy = New();
	for (const_iterator i = begin(); i != end(); ++i)
		if (!DPTR(copy)->Copy(this, *i)) {
			UDOMINOERROR(
				uconstructstr(
					"%s 0x%x copy failed for element with index '%s'",
					GetExtClassString(),
					GetSerialNo(),
					i->GetIndex().ConvertToStringBySkippingUserCode().c_str()
				)
			);
			copy->IncRefCounter((DeltaValue*) 0);
			copy->DecRefCounter((DeltaValue*) 0);	// Will instantly collect here.
			return (DeltaTable*) 0;
		}

	return copy;
}

/////////////////////////////////////////////////////////////////

void  DeltaTable::Extend (const DeltaTable* from) {

	DeltaValue index, content;
	for (DeltaTable::const_iterator i = DPTR(from)->begin(); i != DPTR(from)->end(); ++i) {

		i->GetIndex(&index);
		BasicGet(index, &content);
		if (content.Type() == DeltaValue_Nil)
			Copy(from, *i);
	}
}

//---------------------------------------------------------------

struct NumberValueLessThan {
	bool operator()(const DeltaValue& x, const DeltaValue& y) const
		{ return x.ToNumber() < y.ToNumber(); }
};

DeltaTable* DeltaTable::GetIndices (void) const {

	DeltaTable* indices = DeltaTableFactory::New();
	util_ui32	order	= 0;
	DeltaValue index, content;

	std::vector<DeltaValue> numIndices;

	for (const_iterator i = begin(); i != end(); ++i) {		
		i->GetIndex(&index);
		if (index.Type() == DeltaValue_Number)	// Collect numeric indices together for sorting.
			numIndices.push_back(index);
		else {
			content.FromNumber((DeltaNumberValueType) order++);
			DPTR(indices)->SetNewElement(content, index);
		}
	}

	if (!numIndices.empty()) {
		std::sort(numIndices.begin(), numIndices.end(), NumberValueLessThan());
		for (std::vector<DeltaValue>::iterator i = numIndices.begin(); i != numIndices.end(); ++i) {
			content.FromNumber((DeltaNumberValueType) order++);
			DPTR(indices)->SetNewElement(content, *i);
		}				
	}

	return indices;
}

//---------------------------------------------------------------
// Hashing for strings and extern func names.
//
util_ui16 DeltaTable::Hash (const char* s, util_ui16 size) 
	{ return uhash(s, size); }

//*********************
// Hashing for tables, references, and extern ids.
//
util_ui16 DeltaTable::Hash (void* p, util_ui16 size) 
	{ return ((util_ui32) p) % size; }

//*********************
// Hashing for extern func address.
//
util_ui16 DeltaTable::Hash (DeltaCodeAddress addr, util_ui16 size) 
	{ return addr % size; }

//*********************

union DeltaNumberBytes {
	unsigned char bytes[sizeof(DeltaNumberValueType)];
	DeltaNumberValueType num;
};

//*********************

util_ui16 DeltaTable::Hash (DeltaNumberValueType num, util_ui16 size) {

	DeltaNumberBytes bytes;
	bytes.num = num;
	
	util_ui16 hashVal=0;
	for (util_ui16 i=0; i<sizeof(DeltaNumberBytes); ++i)
		hashVal += bytes.bytes[i];
	
	return hashVal % size;
}

//---------------------------------------------------------------

const DeltaTable::iterator DeltaTable::const_begin (void) const {
	iterator i;
	i.settable(const_cast<DeltaTable*>(this));
	if (!(i.currBucket = const_cast<DeltaTable*>(this)->SelectIterationBucket(i.currTableNo, i.currBucketNo)))
		i.fwd();	// To get the first valid position.
	return i;
}

/////////////////////////////////////////////////////////////////

const DeltaTable::iterator DeltaTable::const_end (void) const {
	iterator i;
	set_at_end(&i);
	return i;
}

//---------------------------------------------------------------
// Selects the approrpiate bucket list for a given index,
// and returns this bucket. Also, sets the '*table'
// to the corresponding table (it is needed in case the
// a new element has to be added in front of the bucket
// list, hence, the bucket list head ptr changes). 
//
DeltaTable::Element** DeltaTable::GetBucket (const DeltaValue&	index, Element*** table) 
	{ return (*getBucketFuncs[index.Type()])(this, index, table)? *table : (Element**) 0; }

bool DeltaTable::GetBucket_Number (GETBUCKET_ARGS) {
	*table = &p->numberIndex[p->Hash(index.val.numObj, DELTA_TABLE_NUMBER_HASH_SIZE)];
	return true;
}

bool DeltaTable::GetBucket_Bool (GETBUCKET_ARGS) {
	*table = &p->boolIndex[index.val.boolObj == true ? 1: 0];
	return true;
}

bool DeltaTable::GetBucket_String (GETBUCKET_ARGS) {
	*table = &p->strIndex[p->Hash(DPTR(index.val.strObj)->Content().c_str(), DELTA_TABLE_STRING_HASH_SIZE)];
	return true;
}

bool DeltaTable:: GetBucket_Table (GETBUCKET_ARGS) {
	*table = &p->tableIndex[p->Hash((void*) index.val.tableObj, DELTA_TABLE_COMMON_HASH_SIZE)];
	return true;
}

bool DeltaTable:: GetBucket_ProgramFuncAddress (GETBUCKET_ARGS) {
	*table = &p->programFuncIndex[p->Hash(index.val.programFuncObj.func->GetAddress(), DELTA_TABLE_COMMON_HASH_SIZE)];
	return true;
}


bool DeltaTable:: GetBucket_LibraryFuncName (GETBUCKET_ARGS) {
	*table = &p->libraryFuncIndex[p->Hash(ufunc2void(index.val.libraryFuncObj.f), DELTA_TABLE_COMMON_HASH_SIZE)];
	return true;
}

bool DeltaTable:: GetBucket_ExternId (GETBUCKET_ARGS) {
	*table = &p->externIdIndex[p->Hash(DPTR(index.val.externIdObj), DELTA_TABLE_COMMON_HASH_SIZE)];
	return true;
}

bool DeltaTable:: GetBucket_MethodFuncAddress (GETBUCKET_ARGS) {
	*table = &p->methodFuncIndex[p->Hash(index.val.methodFuncObj.func->GetAddress(), DELTA_TABLE_COMMON_HASH_SIZE)];
	return true;
}

bool DeltaTable::GetBucket_Error (GETBUCKET_ARGS) {
	DASSERT(false);	// We should never come at this point!
	return false;
}

//--------------------------------------------------------------

bool DeltaTable::CheckLegalIndex (const DeltaValue& index) const {
	
	UCHECK_PRIMARY_ERROR(
		IsLegalIndex(index),
		uconstructstr("illegal index type '%s'", index.TypeStr())
	);
	
	return true;
	FAIL: return false;
}

//--------------------------------------------------------------

DeltaTable::Element* DeltaTable::GetEntry (
		const DeltaValue&	index,			
		Element*			bucket,	
		Element**			before
	) {	

	for (Element* p = (Element*) 0; bucket; p = bucket, bucket = DPTR(bucket)->GetNext())
		if (DPTR(bucket)->MatchesIndex(index)) 
			{ *before = p; return bucket; }
		
	return (Element*) 0;
}

/////////////////////////////////////////////////////////////////

DeltaTable::Element* DeltaTable::GetEntry (const DeltaValue& index, Element* bucket) {	

	for (; bucket; bucket = DPTR(bucket)->GetNext())
		if (DPTR(bucket)->MatchesIndex(index)) 
			return bucket;

	return (Element*) 0;
}

//////////////////////////////////////////////////////////////////

DeltaTable::Element* DeltaTable::GetElement (const std::string& id) {
	DeltaValue index(id);
	Element** table;
	GetBucket_String(this, index, &table);
	return GetEntry(index, *table);
}

//---------------------------------------------------------------

DeltaTable::Element* DeltaTable::Find (const DeltaValue& index) {
	FindContext context((Element*) 0, (Element*) 0, (Element**) 0);
	if (Find(index, &context)) {
		// Have either a 'before' element or we found the bucket's head element.
		DASSERT(context.third && (context.second || *context.third == context.first));
		return context.first;	// ENTRY
	}
	else 
		{ DASSERT(!context.first); return (Element*) 0; }
}

//////////////////////////////////////////////////////////

bool DeltaTable::Find (const DeltaValue& index, FindContext* context) {

	Element** bucket = GetBucket(index, &context->third);
	DASSERT(bucket);
	return !!(context->first = GetEntry(index, *bucket, &context->second));
}

//////////////////////////////////////////////////////////
// Fast set when we know we put a new entry in the table.

bool DeltaTable::BasicSetNew (const DeltaValue& index, const DeltaValue& content) 
	{ SetNewElement(index, content); return true; }

DeltaTable::Element* DeltaTable::SetNewElement (const DeltaValue& index, const DeltaValue& content) {
	
	Element** table = GetBucket(index, &table);
	Element* elem = DNEWCLASS(Field, (this, index, content));

	CheckOverloadedOperatorAdded(index, content);

	DPTR(elem)->SetNext(*table); 
	*table = elem;	
	++total;

	return elem;
}

//////////////////////////////////////////////////////////

void DeltaTable::Remove (FindContext& context) {

#define	ENTRY	context.first
#define	BEFORE	context.second
#define	TABLE	context.third

	DASSERT(total);

	if (!BEFORE)		// Bucket start removed.
		*TABLE = DPTR(ENTRY)->GetNext();
	else				// Intermediate item removed.
		DPTR(BEFORE)->SetNext(ENTRY->GetNext());

	--total;
	usafecontainerbehavior::notify_removed(ENTRY);
	DDELETE(ENTRY);

#undef	ENTRY
#undef	BEFORE	
#undef	TABLE	
}

//////////////////////////////////////////////////////////
// Local removal.

bool DeltaTable::Remove (const DeltaValue& index) {

	UCHECK_PRIMARY_ERROR(
		total,
		uconstructstr("removing with index '%s' when empty", index.ConvertToString().c_str())
	);

	{	// Intentional block.
	FindContext context((Element*) 0, (Element*) 0, (Element**) 0);

	UCHECK_PRIMARY_ERROR(
		Find(index, &context),
		uconstructstr("in remove, element with index '%s' not found", index.ConvertToString().c_str())
	);
	
	Remove(context);
	CheckOverloadedOperatorRemoved(index);
	}

	return true;
	FAIL: return false;
}

//---------------------------------------------------------------

void DeltaTable::ConvertToStringHandler (
		DeltaString*	at, 
		void			(DeltaValue::*tostring) (DeltaString* at) const,
		bool			(Element::*getcontent)(DeltaValue* at) const,
		void*			closure
	) {
	if (GetPrintState().IsInProgress())
		DPTR(at)->Add("<cycle>");
	else {

		DELTA_CONTAINER_TOSTRING_PROCESSING();

		DPTR(at)->Add(
			uconstructstr("%s " DELTA_OBJECT_SN_FORMAT "%s%s%s", 
				GetExtClassString(), 
				GetSerialNo(),
				closure ? ((const char*) closure) : "",
				myDelegates.size()	? uconstructstr("delegates(%d)",	myDelegates.size()).c_str()	 : "",
				myDelegators.size() ? uconstructstr("delegators(%d)",	myDelegators.size()).c_str() : ""
			)
		);

		at->Add("[");
		DeltaValue index, content;
		bool needComma = false;

		for (iterator i = begin(); i != end(); ++i) {

			if (needComma)
				at->Add(",");
			else
				needComma = true;

			i->GetIndex(&index);
			if (!(*i.*getcontent)(&content))
				{ at->Add("<error!>"); return; }

			(index.*tostring)(at);
			at->Add(":");
			(content.*tostring)(at);
		}
		at->Add("]");
	}
}

//////////////////////////////////////////////////////////////////

void DeltaTable::ConvertToString (DeltaString* at, void* closure) {
	ConvertToStringHandler(
		at, 
		&DeltaValue::ConvertToString, 
		&Element::GetContent,
		closure
	); 
}

void DeltaTable::ConvertToStringBySkippingUserCode (DeltaString* at, void* closure) {
	ConvertToStringHandler(
		at, 
		&DeltaValue::ConvertToStringBySkippingUserCode, 
		&Element::GetContentBySkippingUserCode,
		closure
	); 
}

//////////////////////////////////////////////////////////////////

void DeltaTable::DeleteMemory (void) {
	DASSERT(DeltaTableFactory::GetSingleton().HasProduced(this));
	DeltaTableFactory::Free(this);
	DASSERT(DeltaTableFactory::GetSingleton().HasRecycled(this));
}

//---------------------------------------------------------------
// SET FUNCTIONS.

#define	DELTATABLE_EXCEPTION(caller, err, errclass)			\
	if (true) {												\
	uerror::GetSingleton().post##errclass(					\
		"In '%s' " DELTA_OBJECT_SN_FORMAT ": error %s!",	\
		DPTR(caller)->GetExtClassString(),					\
		DPTR(caller)->GetSerialNo(),						\
		ucstringarg(err)									\
	); UERRORCLASS_POSTEXCEPTION;							\
	} else

///////////////////////////////////////////////////////////////////

struct DeltaTable::SetContentTraits {
	static void								SetContent (DeltaTable* t, DeltaTable::Element* elem, const DeltaValue& content) {
												if (!DPTR(elem)->SetContent(content))
													{ DELTATABLE_EXCEPTION(t, "in set (change element failed)", domino); }
											}

	static void								SetContentDirectly (DeltaTable* t, DeltaTable::Element* elem, const DeltaValue& content) 
												{ DPTR(elem)->SetContentDirectly(content); }

	static void								SkipSetContent (DeltaTable* t, DeltaTable::Element* elem, const DeltaValue& content) 
												{ DPTR(elem)->SetContentBySkippingUserCode(content); }

	static void								ChangeElementOverloadedOperatorCheck (
												DeltaTable*				t, 
												DeltaTable::Element*	elem, 
												const DeltaValue&		index,
												const DeltaValue&		content
											) {
												if (DPTR(elem)->GetType() == DeltaTable::FieldType)
													if (content.IsCallable())
														DPTR(t)->CheckOverloadedOperatorAdded(index, content);
													else
														DPTR(t)->CheckOverloadedOperatorRemoved(index);
												else {}
											}
				
};

///////////////////////////////////////////////////////////////////

template <void (*SetContentFunc)(DeltaTable*, DeltaTable::Element*, const DeltaValue&)>
struct SetImplTraits {
	static bool								BasicNormal (DeltaTable* t, const DeltaValue& index, const DeltaValue& content) {
												
												typedef DeltaTable::Element Element;
												DeltaTable::FindContext	context((Element*) 0, (Element*) 0, (Element**) 0);

												if (DPTR(t)->Find(index, &context))
													if (content.IsNil())
														{ DPTR(t)->Remove(context); return true; }
													else {
														Element* elem = context.first;
														(*SetContentFunc)(t, elem, content); 
														DeltaTable::SetContentTraits::ChangeElementOverloadedOperatorCheck(
															t, 
															elem, 
															index, 
															content
														);
														return true; 
													}
												else
													return false;
											}

	static bool								BasicOverloaded (DeltaTable* t, const DeltaValue& index, const DeltaValue& content) {
												if (DPTR(t)->OverloadsGet() && DPTR(t)->OverloadsSet()) {
													DeltaValue result(DeltaValue::Nil);
													if (!DPTR(t)->OverloadedBoundedGet(index, &result)) {	// Failed
														DELTATABLE_EXCEPTION(t, "in set (lookup via overloaded get failed)", domino);
														return false;
													}
													else
													if (!result.IsNil()) {	// Element found.
														if (!DPTR(t)->OverloadedSet(index, content)) {	// Failed
															DELTATABLE_EXCEPTION(t, "in set (overloaded) change element failed)", domino);
															return false;
														}
														else
															return true;
													}
													else
														return false;	// Not found.
												}
												else
													return BasicNormal(t, index, content);
											}

	static bool								BasicNew (DeltaTable* t, const DeltaValue& index, const DeltaValue& content) 
												{ return DPTR(t)->BasicSetNew(index, content); }

	static bool								OverloadedNew (DeltaTable* t, const DeltaValue& index, const DeltaValue& content) 
													{ return DPTR(t)->SetNew(index, content); }

	static bool								RecursiveNormal (DeltaTable* t, const DeltaValue& index, const DeltaValue& content)
												{ return DPTR(t)->ChangeWithSubobjects(index, content); }

	static bool								SkipRecursiveNormal (DeltaTable* t, const DeltaValue& index, const DeltaValue& content)
												{ return DPTR(t)->ChangeWithSubobjectsBySkippingUserCode(index, content); }

	static bool								NonRecursiveOverloaded (DeltaTable* t, const DeltaValue& index, const DeltaValue& content)
												{ return BasicOverloaded(t, index, content); }

	static bool								NonRecursiveBasic (DeltaTable* t, const DeltaValue& index, const DeltaValue& content)
												{ return BasicNormal(t, index, content); }
};

///////////////////////////////////////////////////////////////////

struct DeltaTable::InternalDelegatesAccessTraits {
	static bool								Has (DeltaTable* t) { return t->HasInternalDelegates(); }
	static const std::list<DeltaTable*>&	Get (DeltaTable* t) { return t->myInternalDelegates; }

};

struct DeltaTable::DelegatesAccessTraits {
	static bool								Has (DeltaTable* t) { return t->HasDelegates(); }
	static const std::list<DeltaTable*>&	Get (DeltaTable* t) { return t->myDelegates; }
};

///////////////////////////////////////////////////////////////////

void DeltaTable::LookupErrorTraits::SetFailed (DeltaTable* t, const std::string& context) 
	{ DELTATABLE_EXCEPTION(t, uconstructstr("in 'set' (while trying %s)", context.c_str()), domino); }

void DeltaTable::LookupErrorTraits::GetFailed (DeltaTable* t, const std::string& context) 
	{ DELTATABLE_EXCEPTION(t, uconstructstr("in 'get' (while trying %s)", context.c_str()), domino); }

void DeltaTable::LookupErrorTraits::RemoveNonExisting (DeltaTable* t) 
	{ DELTATABLE_EXCEPTION(t, "in 'set' (removing non-existing element)", primary); }

void DeltaTable::LookupErrorTraits::NewFailed (DeltaTable* t) 
	{ DELTATABLE_EXCEPTION(t, "in 'set' (while adding new element)", domino); }

///////////////////////////////////////////////////////////////////

void DeltaTable::SkipLookupErrorTraits::SetFailed (DeltaTable* t, const std::string& unused) 
	{ UERRORCLASS_POSTEXCEPTION; }

void DeltaTable::SkipLookupErrorTraits::GetFailed (DeltaTable* t, const std::string& unused) 
	{ UERRORCLASS_POSTEXCEPTION; }

void DeltaTable::SkipLookupErrorTraits::RemoveNonExisting (DeltaTable* t) 
	{ UERRORCLASS_POSTEXCEPTION; }

void DeltaTable::SkipLookupErrorTraits::NewFailed (DeltaTable* t)
	{ UERRORCLASS_POSTEXCEPTION; }

///////////////////////////////////////////////////////////////////

#define	INCLUDE_CALLER			true
#define	DONT_INCLUDE_CALLER		false
#define	ADD_WHEN_NOT_FOUND		true
#define	DONT_ADD_WHEN_NOT_FOUND	false

//////////////////////////////////////////////////////////////////

bool DeltaTable::BasicChange (const DeltaValue& index, const DeltaValue& content) {
	try {
		return	SetImplTraits<SetContentTraits::SetContent>::BasicNormal(
					this, 
					index, 
					content
				);
	}
	catch (uerrorclass::udisengageexception)
		{ return false; }
}

///////////////////////////////////////////////////////////////////

bool DeltaTable::BasicChangeBySkippingUserCode (const DeltaValue& index, const DeltaValue& content) {
	try {
		return	SetImplTraits<SetContentTraits::SkipSetContent>::BasicNormal(
					this, 
					index, 
					content
				);
	}
	catch (uerrorclass::udisengageexception)
		{ return false; }
}

///////////////////////////////////////////////////////////////////

bool DeltaTable::OverloadedChange (const DeltaValue& index, const DeltaValue& content) {
	try {
		return	SetImplTraits<SetContentTraits::SetContent>::BasicOverloaded(
					this, 
					index, 
					content
				);
	}
	catch (uerrorclass::udisengageexception)
		{ return false; }
}

///////////////////////////////////////////////////////////////////
// We separately check the caller since the recursion will get us back to 
// lookup inside subobjects.

bool DeltaTable::ChangeWithDelegates (const DeltaValue& index, const DeltaValue& content) {
try {
		return	SetImplTraits<SetContentTraits::SetContent>::BasicOverloaded(
					this, 
					index, 
					content
				) ||
				SetInDelegatesTemplate<
					SetImplTraits<SetContentTraits::SetContent>::RecursiveNormal,
					SetImplTraits<SetContentTraits::SetContent>::OverloadedNew,
					DelegatesAccessTraits,
					LookupErrorTraits,
					DONT_ADD_WHEN_NOT_FOUND,
					DONT_INCLUDE_CALLER
				>(index, content);
	}
	catch (uerrorclass::udisengageexception)
		{ return false; }
}

///////////////////////////////////////////////////////////////////

bool DeltaTable::ChangeWithDelegatesBySkippingUserCode (const DeltaValue& index, const DeltaValue& content) {
try {
		return	SetImplTraits<SetContentTraits::SkipSetContent>::BasicNormal(
					this, 
					index, 
					content
				) ||
				SetInDelegatesTemplate<
					SetImplTraits<SetContentTraits::SkipSetContent>::SkipRecursiveNormal,
					SetImplTraits<SetContentTraits::SkipSetContent>::BasicNew,
					DelegatesAccessTraits,
					SkipLookupErrorTraits,
					DONT_ADD_WHEN_NOT_FOUND,
					DONT_INCLUDE_CALLER
				>(index, content);
	}
	catch (uerrorclass::udisengageexception)
		{ return false; }
}

///////////////////////////////////////////////////////////////////

bool DeltaTable::SetAttributeNoInternalDelegates (const DeltaValue& id, const DeltaValue& content) {
	DASSERT(id.Type() == DeltaValue_String);
	if (content.IsNil())
		Remove(id);
	else
	if (Element* elem = GetElement(id.ToString()))
		DPTR(elem)->SetContentDirectly(content);
	else
		SetNewElement(id, content);
	return true;
}

//////////////////////////////////////////////////////////////////

bool DeltaTable::SetAttributeWithInternalDelegates (const DeltaValue& id, const DeltaValue& content) {
	DASSERT(id.Type() == DeltaValue_String);
	try {
		return	SetInDelegatesTemplate<
					SetImplTraits<SetContentTraits::SetContentDirectly>::NonRecursiveOverloaded,
					SetImplTraits<SetContentTraits::SetContentDirectly>::OverloadedNew,
					InternalDelegatesAccessTraits,
					LookupErrorTraits,
					ADD_WHEN_NOT_FOUND,
					INCLUDE_CALLER
				>(id, content);
	}
	catch (uerrorclass::udisengageexception)
		{ return false; }
}

///////////////////////////////////////////////////////////////////

bool DeltaTable::ChangeWithSubobjects (const DeltaValue& index, const DeltaValue& content) 
	{ return SetImplTraits<SetContentTraits::SetContent>::BasicOverloaded(this, index, content); }

bool DeltaTable::ChangeWithSubobjectsBySkippingUserCode (const DeltaValue& index, const DeltaValue& content) 
	{ return SetImplTraits<SetContentTraits::SkipSetContent>::BasicNormal(this, index, content); }

///////////////////////////////////////////////////////////////////

bool DeltaTable::SetWithDelegates (const DeltaValue& index, const DeltaValue& content) {
	
	UCHECK_DOMINO_ERROR(CheckLegalIndex(index), "in 'set' (illegal index)");

	try {
		return	SetInDelegatesTemplate<
					SetImplTraits<SetContentTraits::SetContent>::RecursiveNormal,
					SetImplTraits<SetContentTraits::SetContent>::OverloadedNew,
					DelegatesAccessTraits,
					LookupErrorTraits,
					ADD_WHEN_NOT_FOUND,
					INCLUDE_CALLER
				>(index, content);
	}
	catch (uerrorclass::udisengageexception)
		{ return false; }

	FAIL: return false;
}

//////////////////////////////////////////////////////////////////

bool DeltaTable::SetBySkippingUserCode (const DeltaValue& index, const DeltaValue& content) {

	if (!IsLegalIndex(index))
		return false;
	else
	try {
		return	SetInDelegatesTemplate<
					SetImplTraits<SetContentTraits::SkipSetContent>::SkipRecursiveNormal,
					SetImplTraits<SetContentTraits::SkipSetContent>::BasicNew,
					DelegatesAccessTraits,
					SkipLookupErrorTraits,
					ADD_WHEN_NOT_FOUND,
					INCLUDE_CALLER
				>(index, content);
	}
	catch (uerrorclass::udisengageexception)
		{ return false; }
}

//////////////////////////////////////////////////////////////////

bool DeltaTable::BasicSetWithInternalDelegates (const DeltaValue& index, const DeltaValue& content) {

	UCHECK_DOMINO_ERROR(CheckLegalIndex(index), "in 'set' (illegal index)");

	try {
		return	SetInDelegatesTemplate<
					SetImplTraits<SetContentTraits::SetContent>::NonRecursiveBasic,
					SetImplTraits<SetContentTraits::SetContent>::BasicNew,
					InternalDelegatesAccessTraits,
					LookupErrorTraits,
					ADD_WHEN_NOT_FOUND,
					INCLUDE_CALLER
				>(index, content);
	}
	catch (uerrorclass::udisengageexception)
		{ return false; }

	return true;
	FAIL: return false;
}

//////////////////////////////////////////////////////////////////

bool DeltaTable::BasicSetNoInternalDelegates (const DeltaValue& index, const DeltaValue& content) {

	UCHECK_DOMINO_ERROR(CheckLegalIndex(index), "in 'set' (illegal index)");

	if (content.IsNil())
		return Remove(index);
	else
	if (Element* elem = Find(index)) {
		UCHECK_DOMINO_ERROR(DPTR(elem)->SetContent(content), "in 'set' (change element failed)");							
		if (DPTR(elem)->GetType() == FieldType)
			SetContentTraits::ChangeElementOverloadedOperatorCheck(this, elem, index, content);
	}
	else
		SetNewElement(index, content);

	return true;
	FAIL: return false;
}

//---------------------------------------------------------------
// ATTRIBUTES.
//
DeltaTable::Attribute* DeltaTable::NewAttribute (
		const std::string&	id,
		const DeltaValue&	set,
		const DeltaValue&	get
	) {

	DeltaValue index(id);
	Element** table;
	GetBucket_String(this, index, &table);

	UCHECK_PRIMARY_ERROR(
		!GetEntry(index, *table),
		uconstructstr("new attribute failed since slot '%s' already exists", id.c_str())
	);

	{
		Attribute* entry = DNEWCLASS(Attribute, (this, id, set, get));
		DPTR(entry)->SetNext(*table);
		*table = entry;
		++total;
		return entry;
	}

	FAIL: return (Attribute*) 0;
}

//////////////////////////////////////////////////////////////////

DeltaTable::Attribute* DeltaTable::GetAttribute (const std::string& id) {
	if (Element* elem = GetElement(id))
		if (elem->GetType() == AttributeType)
			return (Attribute*) elem;
	return (Attribute*) 0;
}

const DeltaTable::Attribute* DeltaTable::GetAttribute (const std::string& id) const 
	{ return const_cast<DeltaTable*>(this)->GetAttribute(id); }

//////////////////////////////////////////////////////////////////

bool DeltaTable::HasAttribute (const std::string& id) const
	{ return !!const_cast<DeltaTable*>(this)->GetElement(id); }

//---------------------------------------------------------------
// GET.

struct DeltaTable::GetContentTraits {
	static void								GetContent (DeltaTable* t, DeltaTable::Element* elem, DeltaValue* content) {
												if (!DPTR(elem)->GetContent(content))
													{ DELTATABLE_EXCEPTION(t, "in get (extracting content failed)", domino); }
											}

	static void								SkipGetContent(DeltaTable* t, DeltaTable::Element* elem, DeltaValue* content) 
												{ DPTR(elem)->GetContentBySkippingUserCode(content); }

	static void								GetContentDirectly (DeltaTable* t, DeltaTable::Element* elem, DeltaValue* content) 
												{ DPTR(elem)->GetContentDirectly(content); }
};

///////////////////////////////////////////////////////////////////

template <void (*GetContentFunc)(DeltaTable*, DeltaTable::Element*, DeltaValue*)>
struct GetImplTraits {
	static bool								BasicNormal (DeltaTable* t, const DeltaValue& index, DeltaValue* content) {
												typedef DeltaTable::Element Element;
												if (Element* elem = DPTR(t)->Find(index)) 
													{ (*GetContentFunc)(t, elem, content); return true; }
												else
													return false;
											}

	static bool								BasicOverloaded (DeltaTable* t, const DeltaValue& index, DeltaValue* content) {
												if (DPTR(t)->OverloadsGet()) {
													if (!DPTR(t)->OverloadedBoundedGet(index, content)) {	// Failed
														DELTATABLE_EXCEPTION(t, "in get (overloaded) failed)", domino);
														return false;
													}
													else
														return !content->IsNil();
												}
												else
													return BasicNormal(t, index, content);
											}

	static bool								RecursiveNormal (DeltaTable* t, const DeltaValue& index, DeltaValue* content, bool* inDelegates)
												{ return DPTR(t)->FindWithSubobjects(index, content, inDelegates); }

	static bool								SkipRecursiveNormal (DeltaTable* t, const DeltaValue& index, DeltaValue* content, bool* inDelegates)
												{ return DPTR(t)->FindWithSubobjectsBySkippingUserCode(index, content, inDelegates); }

	static bool								NonRecursiveOverloaded (DeltaTable* t, const DeltaValue& index, DeltaValue* content, bool* inDelegates) 
												{ return BasicOverloaded(t, index, content) ? *inDelegates = true : false; }

	static bool								NonRecursiveBasic (DeltaTable* t, const DeltaValue& index, DeltaValue* content, bool* inDelegates) 
												{ return BasicNormal(t, index, content) ? *inDelegates = true : false; }
};

//////////////////////////////////////////////////////////////////

#define	HANDLE_FIND_IN_DELEGATES()							\
	if (found) {											\
			DASSERT(inDelegates);							\
			if (ChangeSelfWhenDelegateMethod(&result))		\
				content->Assign(result);					\
			else											\
				return content->AssignOverloadable(result);	\
		}													\
		else												\
			content->FromNil()


//////////////////////////////////////////////////////////////////

bool DeltaTable::GetAttributeNoInternalDelegates (const DeltaValue& id, DeltaValue* content) {
	DASSERT(id.IsString());
	if (Element* elem = GetElement(id.ToString())) {
		DeltaValue result;
		DPTR(elem)->GetContentDirectly(&result);
		return content->AssignOverloadable(result);
	}
	else
		{ content->FromNil(); return true; }
}

//////////////////////////////////////////////////////////////////

bool DeltaTable::GetAttributeNoInternalDelegatesByUnoverloadedAssign (const DeltaValue& id, DeltaValue* content) {
	DASSERT(id.IsString());
	content->FromNil(); 
	if (Element* elem = GetElement(id.ToString()))
		DPTR(elem)->GetContentDirectly(content);
	return true;
}

//////////////////////////////////////////////////////////////////

bool DeltaTable::GetAttributeWithInternalDelegates (const DeltaValue& id, DeltaValue* content) {

	DASSERT(id.IsString());

	try {
		bool inDelegates = false;
		DeltaValue result(DeltaValue::Nil);

		bool found =	GetInDelegatesTemplate<
							GetImplTraits<GetContentTraits::GetContentDirectly>::NonRecursiveBasic,
							InternalDelegatesAccessTraits,
							LookupErrorTraits,
							INCLUDE_CALLER
						>(id, &result, &inDelegates);

		HANDLE_FIND_IN_DELEGATES();
		return true;
	}
	catch (uerrorclass::udisengageexception)
		{ return false; }
}

//////////////////////////////////////////////////////////////////

bool DeltaTable::GetAttributeWithInternalDelegatesByUnoverloadedAssign (const DeltaValue& id, DeltaValue* content) {

	DASSERT(id.IsString());

	try {
		bool inDelegates = false;
		content->FromNil();

		bool found =	GetInDelegatesTemplate<
							GetImplTraits<GetContentTraits::GetContentDirectly>::NonRecursiveBasic,
							InternalDelegatesAccessTraits,
							LookupErrorTraits,
							INCLUDE_CALLER
						>(id, content, &inDelegates);
		if (found) {
			DASSERT(inDelegates);
			ChangeSelfWhenDelegateMethod(content);
		}
		return true;
	}
	catch (uerrorclass::udisengageexception)
		{ return false; }
}

//////////////////////////////////////////////////////////////////

bool DeltaTable::BasicGetWithInternalDelegates (const DeltaValue& index, DeltaValue* content) {

	UCHECK_DOMINO_ERROR(CheckLegalIndex(index), "in 'get' (illegal index)");

	try {
		bool		inDelegates = false;
		DeltaValue	result(DeltaValue::Nil);

		bool found =	GetInDelegatesTemplate<
							GetImplTraits<GetContentTraits::GetContent>::NonRecursiveBasic,
							InternalDelegatesAccessTraits,
							LookupErrorTraits,
							INCLUDE_CALLER
						>(index, &result, &inDelegates);

		HANDLE_FIND_IN_DELEGATES();
		return true;
	}
	catch (uerrorclass::udisengageexception)
		{ return false; }

	FAIL: return false;
}

//****************************

bool DeltaTable::BasicGetNoInternalDelegates (const DeltaValue& index, DeltaValue* content) {

	UCHECK_DOMINO_ERROR(CheckLegalIndex(index), "in 'get' (illegal index)");
	try {
		DeltaValue result(DeltaValue::Nil);
		if (GetImplTraits<GetContentTraits::GetContent>::BasicNormal(this, index, &result))
			return content->AssignOverloadable(result);
		else
			{ content->FromNil(); return true; }
	}
	catch (uerrorclass::udisengageexception)
		{ return false; }

	FAIL: return false;
}

//****************************

bool DeltaTable::BasicGetWithInternalDelegatesBySkippingUserCode (const DeltaValue& index, DeltaValue* content) {

	if (!IsLegalIndex(index))
		return false;
	else
	try {
		bool inDelegates = false;
		content->FromNil();
		bool found =	GetInDelegatesTemplate<
							GetImplTraits<GetContentTraits::SkipGetContent>::NonRecursiveBasic,
							InternalDelegatesAccessTraits,
							SkipLookupErrorTraits,
							INCLUDE_CALLER
						>(index, content, &inDelegates);

		if (found) {
			DASSERT(inDelegates);
			ChangeSelfWhenDelegateMethod(content);
		}		
		return true;
	}
	catch (uerrorclass::udisengageexception)
		{ return false; }
}

//****************************

bool DeltaTable::BasicGetNoInternalDelegatesBySkippingUserCode (const DeltaValue& index, DeltaValue* content) {
	if (!IsLegalIndex(index))
		return false;
	else 
	try {
		content->FromNil();
		GetImplTraits<GetContentTraits::SkipGetContent>::BasicNormal(this, index, content);
		return true;
	}
	catch (uerrorclass::udisengageexception)
		{ return false; }
}

//****************************

bool DeltaTable::BasicGetWithInternalDelegatesByUnoverloadedAssign (const DeltaValue& index, DeltaValue* content) {

	UCHECK_DOMINO_ERROR(CheckLegalIndex(index), "in 'get' (illegal index)");

	try {
		bool inDelegates = false;
		content->FromNil();
		bool found =	GetInDelegatesTemplate<
							GetImplTraits<GetContentTraits::GetContent>::NonRecursiveBasic,
							InternalDelegatesAccessTraits,
							LookupErrorTraits,
							INCLUDE_CALLER
						>(index, content, &inDelegates);

		if (found) {
			DASSERT(inDelegates);
			ChangeSelfWhenDelegateMethod(content);
		}
		return true;
	}
	catch (uerrorclass::udisengageexception)
		{ return false; }

	FAIL: return false;
}

//****************************

bool DeltaTable::BasicGetNoInternalDelegatesByUnoverloadedAssign (const DeltaValue& index, DeltaValue* content) {

	UCHECK_DOMINO_ERROR(CheckLegalIndex(index), "in 'get' (illegal index)");

	try {
		content->FromNil();
		GetImplTraits<GetContentTraits::GetContent>::BasicNormal(this, index, content);
		return true;
	}
	catch (uerrorclass::udisengageexception)
		{ return false; }

	FAIL: return false;
}

//////////////////////////////////////////////////////////////////

bool DeltaTable::FindWithSubobjects (const DeltaValue& index, DeltaValue* content, bool* inDelegates)
	{ return GetImplTraits<GetContentTraits::GetContent>::BasicOverloaded(this, index, content); }

bool DeltaTable::FindWithSubobjectsBySkippingUserCode (const DeltaValue& index, DeltaValue* content, bool* inDelegates)
	{ return GetImplTraits<GetContentTraits::GetContentDirectly>::BasicNormal(this, index, content); }

//////////////////////////////////////////////////////////////////

bool DeltaTable::BasicFind (const DeltaValue& index, DeltaValue* content, bool* inDelegates) {
	try {
		return	GetImplTraits<GetContentTraits::GetContent>::BasicNormal(
					this, 
					index, 
					content
				);
	}
	catch (uerrorclass::udisengageexception)
		{ return false; }
}

//////////////////////////////////////////////////////////////////

bool DeltaTable::BasicFindBySkippingUserCode (const DeltaValue& index, DeltaValue* content, bool* inDelegates) {
	try {
		return	GetImplTraits<GetContentTraits::SkipGetContent>::BasicNormal(
					this, 
					index, 
					content
				);
	}
	catch (uerrorclass::udisengageexception)
		{ return false; }
}

//////////////////////////////////////////////////////////////////

bool DeltaTable::OverloadedFind (const DeltaValue& index, DeltaValue* content, bool* inDelegates) {
	try {
		return	GetImplTraits<GetContentTraits::GetContent>::BasicOverloaded(
					this, 
					index, 
					content
				);
	}
	catch (uerrorclass::udisengageexception)
		{ return false; }
}

//////////////////////////////////////////////////////////////////

bool DeltaTable::FindWithDelegates (const DeltaValue& index, DeltaValue* content, bool* inDelegates) {
	try {
		return	(*inDelegates = GetImplTraits<GetContentTraits::GetContent>::BasicOverloaded(
									this, 
									index, 
									content
								)) 
				||
				GetInDelegatesTemplate<
					GetImplTraits<GetContentTraits::GetContent>::RecursiveNormal,
					DelegatesAccessTraits,
					LookupErrorTraits,
					DONT_INCLUDE_CALLER	// Necessary to avoid endless recursion.
				>(index, content, inDelegates);
	}
	catch (uerrorclass::udisengageexception)
		{ return false; }
}

//////////////////////////////////////////////////////////////////

bool DeltaTable::FindWithDelegatesBySkippingUserCode (const DeltaValue& index, DeltaValue* content, bool* inDelegates) {
	try {
		return	(*inDelegates = GetImplTraits<GetContentTraits::SkipGetContent>::BasicNormal(
									this, 
									index, 
									content
								)) 
				||
				GetInDelegatesTemplate<
					GetImplTraits<GetContentTraits::SkipGetContent>::SkipRecursiveNormal,
					DelegatesAccessTraits,
					LookupErrorTraits,
					DONT_INCLUDE_CALLER	// Necessary to avoid endless recursion.
				>(index, content, inDelegates);
	}
	catch (uerrorclass::udisengageexception)
		{ return false; }
}

//////////////////////////////////////////////////////////////////

bool DeltaTable::GetWithDelegates (const DeltaValue& index, DeltaValue* content) {

	UCHECK_DOMINO_ERROR(CheckLegalIndex(index), "in 'get' (illegal index)");

	try {
		DeltaValue	result(DeltaValue::Nil);
		bool		inDelegates = false;
		bool		found =	GetInDelegatesTemplate<
								GetImplTraits<GetContentTraits::GetContent>::RecursiveNormal,
								DelegatesAccessTraits,
								LookupErrorTraits,
								INCLUDE_CALLER
							>(index, &result, &inDelegates);
		HANDLE_FIND_IN_DELEGATES();
		return true;
	}
	catch (uerrorclass::udisengageexception)
		{ return false; }

	FAIL: return false;
}

//////////////////////////////////////////////////////////////////

bool DeltaTable::GetWithDelegatesBySkippingUserCode (const DeltaValue& index, DeltaValue* content) {

	if (!IsLegalIndex(index))
		return false;
	else
	try {
		content->FromNil();
		bool		inDelegates = false;
		bool		found =	GetInDelegatesTemplate<
								GetImplTraits<GetContentTraits::SkipGetContent>::SkipRecursiveNormal,
								DelegatesAccessTraits,
								SkipLookupErrorTraits,
								INCLUDE_CALLER
							>(index, content, &inDelegates);
		if (found) {
			DASSERT(inDelegates);
			ChangeSelfWhenDelegateMethod(content);
		}			
		return true;
	}
	catch (uerrorclass::udisengageexception)
		{ return false; }
}

//////////////////////////////////////////////////////////////////

bool DeltaTable::GetWithDelegatesByUnoverloadedAssign (const DeltaValue& index, DeltaValue* content) {

	UCHECK_DOMINO_ERROR(CheckLegalIndex(index), "in 'get' (illegal index)");

	try {
		content->FromNil();
		bool		inDelegates = false;
		bool		found =	GetInDelegatesTemplate<
								GetImplTraits<GetContentTraits::GetContent>::RecursiveNormal,
								DelegatesAccessTraits,
								LookupErrorTraits,
								INCLUDE_CALLER
							>(index, content, &inDelegates);
		if (found) {
			DASSERT(inDelegates);
			ChangeSelfWhenDelegateMethod(content);
		}			
		return true;
	}
	catch (uerrorclass::udisengageexception)
		{ return false; }

	FAIL: return false;
}

//---------------------------------------------------------------
// OVERLOADING.
//

bool DeltaTable::OverloadedSet (const DeltaValue& index, const DeltaValue& content) {

	DASSERT(overloadsSet);

	const DeltaValue* args[] = { &content, &index, &selfValue };
	UCHECK_WHATEVER_ERROR(set(args, 3, (DeltaValue*) 0), "overloaded bounded set failed");

	return true;
	FAIL: return false;
}

//////////////////////////////////////////////////////////////////

bool DeltaTable::OverloadedBoundedGetByUnoverloadedAssign (const DeltaValue& index, DeltaValue* content) {

	DASSERT(overloadsGet);
	content->FromNil();

	const DeltaValue* args[] = { &index, &selfValue };
	UCHECK_WHATEVER_ERROR(get(args, 2, content), "overloaded bounded get failed");

	return true;
	FAIL: return false;
}

//////////////////////////////////////////////////////////////////

bool DeltaTable::OverloadedBoundedGet (const DeltaValue& index, DeltaValue* content) {

	DASSERT(overloadsGet);
	content->FromNil();

	const DeltaValue* args[] = { &index, &selfValue };
	DeltaValue result(DeltaValue::Nil);
	UCHECK_WHATEVER_ERROR(get(args, 2, &result), "overloaded bounded get failed");

	return content->AssignOverloadable(result);
	FAIL: return false;
}

//////////////////////////////////////////////////////////////////

bool DeltaTable::OverloadedGet (const DeltaValue& index, DeltaValue* content, bool* inDelegate) {

	DASSERT(overloadsGet);

	if (inDelegate)
		*inDelegate = false;
	const DeltaValue* args[] = { &index, &selfValue };

	DeltaValue result(DeltaValue::Nil);
	UCHECK_WHATEVER_ERROR(get(args, 2, &result), "overloaded bounded get failed");

	return content->AssignOverloadable(result);
	FAIL: return false;
}

//////////////////////////////////////////////////////////////////

void DeltaTable::InstallOverloadedBoundedGetFunctions (void) {
	
	DASSERT(currBoundedGetFunc						== currBasicGetFunc);
	DASSERT(currBoundedGetByUnoverloadedAssignFunc	== currBasicGetByUnoverloadedAssignFunc);

	currBoundedGetFunc						= &DeltaTable::OverloadedBoundedGet;
	currBoundedGetByUnoverloadedAssignFunc	= &DeltaTable::OverloadedBoundedGetByUnoverloadedAssign;

	if (!HasDelegates()) {
		currGetFunc								= currBoundedGetFunc;
		currGetByUnoverloadedAssignFunc			= currBoundedGetByUnoverloadedAssignFunc;
		currGetBySkippingUserCodeFunc			= currBasicGetBySkippingUserCodeFunc;
		currFindFunc							= &DeltaTable::OverloadedFind;
		if (OverloadsSet())
			currChangeFunc = &DeltaTable::OverloadedChange;
	}
}

//////////////////////////////////////////////////////////////////

void DeltaTable::InstallOverloadedBoundedSetFunctions (void) {
	
	DASSERT(currBoundedSetFunc == currBasicSetFunc);

	currBoundedSetFunc	= &DeltaTable::OverloadedSet;
	currSetNewFunc		= &DeltaTable::OverloadedSet;

	if (!HasDelegates()) {
		currSetFunc		= currBoundedSetFunc;
		if (OverloadsGet())
			currChangeFunc = &DeltaTable::OverloadedChange;
	}
}

//////////////////////////////////////////////////////////////////

void DeltaTable::UninstallOverloadedBoundedGetFunctions (void) {
	
	DASSERT(currBoundedGetFunc						== &DeltaTable::OverloadedBoundedGet);
	DASSERT(currBoundedGetByUnoverloadedAssignFunc	== &DeltaTable::OverloadedBoundedGetByUnoverloadedAssign);

	currBoundedGetFunc						= currBasicGetFunc;
	currBoundedGetByUnoverloadedAssignFunc	= currBasicGetByUnoverloadedAssignFunc;

	if (!HasDelegates()) {
		currGetFunc								= currBoundedGetFunc;
		currGetByUnoverloadedAssignFunc			= currBoundedGetByUnoverloadedAssignFunc;
		currGetBySkippingUserCodeFunc			= currBasicGetBySkippingUserCodeFunc;
		currChangeFunc							= &DeltaTable::BasicChange;
		currFindFunc							= &DeltaTable::BasicFind;

	}
	else {
		currChangeFunc	= &DeltaTable::ChangeWithDelegates;
		currFindFunc	= &DeltaTable::FindWithDelegates;
	}
}

//////////////////////////////////////////////////////////////////

void DeltaTable::UninstallOverloadedBoundedSetFunctions (void) {

	DASSERT(currBoundedSetFunc	== &DeltaTable::OverloadedSet);
	currBoundedSetFunc	= currBasicSetFunc;
	currSetNewFunc		= &DeltaTable::BasicSetNew;

	if (!HasDelegates()) {
		currSetFunc		= currBoundedSetFunc;
		currChangeFunc	= &DeltaTable::BasicChange;
	}
	else
		currChangeFunc	= &DeltaTable::ChangeWithDelegates;
}

//////////////////////////////////////////////////////////////////

void DeltaTable::CheckOverloadedOperatorAdded (const DeltaValue& index, const DeltaValue& content) {
	if (index.Type() == DeltaValue_String) {
		std::string s(index.ToString());
		if (s == DELTA_OPERATOR_OBJECT_SET) {
			set.Assign(content); 
			if (IsOverloadingEnabled() && !overloadsSet)
				InstallOverloadedBoundedSetFunctions();
			overloadsSet = true;
		}
		else
		if (s  == DELTA_OPERATOR_OBJECT_GET) {
			get.Assign(content); 
			if (IsOverloadingEnabled() && !overloadsGet)
				InstallOverloadedBoundedGetFunctions();
			overloadsGet = true;
		}
	}
}

//////////////////////////////////////////////////////////////////

void DeltaTable::CheckOverloadedOperatorRemoved (const DeltaValue& index) {
	if (index.Type() == DeltaValue_String) {
		std::string s(index.ToString());
		if (s == DELTA_OPERATOR_OBJECT_SET) {
			if (overloadsSet) {
				set.FromNil(); 
				overloadsSet = false;
				if (IsOverloadingEnabled()) 
					UninstallOverloadedBoundedSetFunctions();
			}
		}
		else
		if (s  == DELTA_OPERATOR_OBJECT_GET) {
			if (overloadsGet) {
				get.FromNil(); 
				overloadsGet = false;
				if (IsOverloadingEnabled()) 
					UninstallOverloadedBoundedGetFunctions();
			}
		}
	}
}

//////////////////////////////////////////////////////////////////

bool DeltaTable::ChangeSelfWhenDelegateMethod (DeltaValue* content) {
	if (content->IsMethodFunc() && IsDelegate(content->GetMethodFuncSelf()))
		{ content->ChangeMethodFuncSelf(this); return true; }
	else
		return false;
}

//////////////////////////////////////////////////////////////////

void DeltaTable::EnableOverloading (void) {
	if (!overloadingEnabled) {
		overloadingEnabled = true; 
		if (overloadsGet)
			InstallOverloadedBoundedGetFunctions();
	}
}

//////////////////////////////////////////////////////////////////

void DeltaTable::DisableOverloading (void) {
	if (overloadingEnabled) {
		overloadingEnabled = false; 
		if (overloadsGet) 
			UninstallOverloadedBoundedGetFunctions();
	}
}

//////////////////////////////////////////////////////////////////

bool DeltaTable::HasLookupCycle (void) const {
	if (!HasDelegates())
		return false;
	else
	if (cycleCheck)	// Already checked.
		return true;
	else {
		cycleCheck = true;
		bool cycleFound = false;
		for (std::list<DeltaTable*>::const_iterator i = myDelegates.begin(); i != myDelegates.end(); ++i)
			if (DPTR(*i)->HasLookupCycle())
				{ cycleFound = true; break; }
		cycleCheck = false;
		return cycleFound;
	}
}

//////////////////////////////////////////////////////////////////

static const char* overloadableOperators[] = {

	"+",		"-",		"*",		"/",			"%",	
	"+_",		"-_",		"*_",		"/_",			"%_",

	"!=",		"==",		">=",		"<=",			">",	"<",
	"!=_",		"==_",		">=_",		"<=_",			">_",	 "<_",

	"=",		"()",		"=()",		"tostring()",	".",	".="
};								

const std::pair<const char**, util_ui32> 
DeltaTable::GetOverloabableOperators (void) {
	return	std::pair<const char**, util_ui32>(
				overloadableOperators, 
				uarraysize(overloadableOperators)
			);
}

bool DeltaTable::IsOverloadableOperator (const std::string& op) {
	return	ustrpos(
				overloadableOperators, 
				uarraysize(overloadableOperators), 
				op.c_str()
			) != uarraysize(overloadableOperators);
}

//---------------------------------------------------------------
// DELEGATION.

bool DeltaTable::DelegateInternal (DeltaTable* obj) {
	if (!Delegate(obj))
		return false;
	else  {
		if (myInternalDelegates.empty()) {

			currBasicGetFunc						= &DeltaTable::BasicGetWithInternalDelegates;
			currBasicGetBySkippingUserCodeFunc		= &DeltaTable::BasicGetWithInternalDelegatesBySkippingUserCode;
			currBasicGetByUnoverloadedAssignFunc	= &DeltaTable::BasicGetWithInternalDelegatesByUnoverloadedAssign;
			currBasicSetFunc						= &DeltaTable::BasicSetWithInternalDelegates;
			currAttrGetFunc							= &DeltaTable::GetAttributeWithInternalDelegates;
			currAttrGetByUnoverloadedAssignFunc		= &DeltaTable::GetAttributeWithInternalDelegatesByUnoverloadedAssign;
			currAttrSetFunc							= &DeltaTable::SetAttributeWithInternalDelegates;

			if (!OverloadsGet()) {	// Only then we change it.
				currBoundedGetFunc						= currBasicGetFunc;
				currBoundedGetByUnoverloadedAssignFunc	= currBasicGetByUnoverloadedAssignFunc;
			}

			if (!OverloadsSet())	// Only then we change it.
				currBoundedSetFunc						= currBasicSetFunc;
		}
		else {
			DASSERT(currBasicGetFunc						== &DeltaTable::BasicGetWithInternalDelegates);
			DASSERT(currBasicGetBySkippingUserCodeFunc		== &DeltaTable::BasicGetWithInternalDelegatesBySkippingUserCode);
			DASSERT(currBasicGetByUnoverloadedAssignFunc	== &DeltaTable::BasicGetWithInternalDelegatesByUnoverloadedAssign);
			DASSERT(currBasicSetFunc						== &DeltaTable::BasicSetWithInternalDelegates);
			DASSERT(currAttrGetFunc							== &DeltaTable::GetAttributeWithInternalDelegates);
			DASSERT(currAttrGetByUnoverloadedAssignFunc		== &DeltaTable::GetAttributeWithInternalDelegatesByUnoverloadedAssign);
			DASSERT(currAttrSetFunc							== &DeltaTable::SetAttributeWithInternalDelegates);
		}

		myInternalDelegates.push_back(obj); 
		return true; 
	}
}

//////////////////////////////////////////////////////////////////

bool DeltaTable::UndelegateInternal (DeltaTable* obj) {
	
	UCHECK_PRIMARY_ERROR(
		IsInternalDelegate(obj),
		uconstructstr(
			"undelegate internal to " DELTA_OBJECT_SN_FORMAT " failed (not an internal delegate)",
			DPTR(obj)->GetSerialNo()
		)
	);
	
	DASSERT(currBasicGetFunc						== &DeltaTable::BasicGetWithInternalDelegates);
	DASSERT(currBasicGetBySkippingUserCodeFunc		== &DeltaTable::BasicGetWithInternalDelegatesBySkippingUserCode);
	DASSERT(currBasicGetByUnoverloadedAssignFunc	== &DeltaTable::BasicGetWithInternalDelegatesByUnoverloadedAssign);
	DASSERT(currBasicSetFunc						== &DeltaTable::BasicSetWithInternalDelegates);
	DASSERT(currAttrGetFunc							== &DeltaTable::GetAttributeWithInternalDelegates);
	DASSERT(currAttrGetByUnoverloadedAssignFunc		== &DeltaTable::GetAttributeWithInternalDelegatesByUnoverloadedAssign);
	DASSERT(currAttrSetFunc							== &DeltaTable::SetAttributeWithInternalDelegates);

	myInternalDelegates.remove(obj);
	DASSERT(IsDelegate(obj));

	if (myInternalDelegates.empty()) {

		currBasicGetFunc						= &DeltaTable::BasicGetNoInternalDelegates;
		currBasicGetBySkippingUserCodeFunc		= &DeltaTable::BasicGetNoInternalDelegatesBySkippingUserCode;
		currBasicGetByUnoverloadedAssignFunc	= &DeltaTable::BasicGetNoInternalDelegatesByUnoverloadedAssign;
		currBasicSetFunc						= &DeltaTable::BasicSetNoInternalDelegates;
		currAttrGetFunc							= &DeltaTable::GetAttributeNoInternalDelegates;
		currAttrGetByUnoverloadedAssignFunc		= &DeltaTable::GetAttributeNoInternalDelegatesByUnoverloadedAssign;
		currAttrSetFunc							= &DeltaTable::SetAttributeNoInternalDelegates;

		if (!OverloadsGet()) {	// Only then we change it.
			currBoundedGetFunc						= currBasicGetFunc;
			currBoundedGetByUnoverloadedAssignFunc	= currBasicGetByUnoverloadedAssignFunc;
		}

		if (!OverloadsSet())	// Only then we change it.
			currBoundedSetFunc						= currBasicSetFunc;
	}

	{ bool result = Undelegate(obj); DASSERT(result); }

	return true;
	FAIL: return false;
}

//////////////////////////////////////////////////////////////////

bool DeltaTable::IsInternalDelegate (const DeltaTable* obj) const 
	{ return std::find(myInternalDelegates.begin(), myInternalDelegates.end(), obj) !=  myInternalDelegates.end(); }

//////////////////////////////////////////////////////////////////

bool DeltaTable::Delegate (DeltaTable* obj) {

	DASSERT(!IsDelegate(obj));
	DASSERT(!HasLookupCycle());

	UCHECK_PRIMARY_ERROR(
		!DPTR(obj)->IsDelegate(this),
		uconstructstr(
			"delegate to " DELTA_OBJECT_SN_FORMAT " causes a delegation cycle",
			DPTR(obj)->GetSerialNo()
		)
	);
	
	if (myDelegates.empty()) {
		currGetFunc								= &DeltaTable::GetWithDelegates;
		currGetBySkippingUserCodeFunc			= &DeltaTable::GetWithDelegatesBySkippingUserCode;
		currGetByUnoverloadedAssignFunc			= &DeltaTable::GetWithDelegatesByUnoverloadedAssign;
		currSetFunc								= &DeltaTable::SetWithDelegates;
		currChangeFunc							= &DeltaTable::ChangeWithDelegates;
		currChangeBySkippingUserCodeFunc		= &DeltaTable::ChangeWithDelegatesBySkippingUserCode;
		currFindFunc							= &DeltaTable::FindWithDelegates;
		currFindBySkippingUserCodeFunc			= &DeltaTable::FindWithDelegatesBySkippingUserCode;
	}
	else {
		DASSERT(currGetFunc						== &DeltaTable::GetWithDelegates);
		DASSERT(currGetBySkippingUserCodeFunc	== &DeltaTable::GetWithDelegatesBySkippingUserCode);
		DASSERT(currGetByUnoverloadedAssignFunc	== &DeltaTable::GetWithDelegatesByUnoverloadedAssign);
		DASSERT(currSetFunc						== &DeltaTable::SetWithDelegates);
		DASSERT(currChangeFunc						== &DeltaTable::ChangeWithDelegates);
		DASSERT(currChangeBySkippingUserCodeFunc	== &DeltaTable::ChangeWithDelegatesBySkippingUserCode);
		DASSERT(currFindFunc					== &DeltaTable::FindWithDelegates);
		DASSERT(currFindBySkippingUserCodeFunc	== &DeltaTable::FindWithDelegatesBySkippingUserCode);
	}

	AddDelegate(obj);
	DPTR(obj)->AddDelegator(this);
	
	if (HasLookupCycle()) {

		RemoveDelegate(obj);
		DPTR(obj)->RemoveDelegator(this);
	
		UCHECK_PRIMARY_ERROR(
			false,
			uconstructstr(
				"delegate to " DELTA_OBJECT_SN_FORMAT " causes a lookup cycle",
				DPTR(obj)->GetSerialNo()
			)
		);
	}

	return true;
	FAIL: return false;
}

//////////////////////////////////////////////////////////////////

bool DeltaTable::Undelegate (DeltaTable* obj) {

	UCHECK_PRIMARY_ERROR(
		IsDirectDelegate(obj),
		uconstructstr(
			"undelegate to " DELTA_OBJECT_SN_FORMAT " failed (not a delegate)",
			DPTR(obj)->GetSerialNo()
		)
	);

	UCHECK_PRIMARY_ERROR(
		!IsInternalDelegate(obj),
		uconstructstr(
			"undelegate to " DELTA_OBJECT_SN_FORMAT " failed (it is an internal delegate)",
			DPTR(obj)->GetSerialNo()
		)
	);

	RemoveDelegate(obj);
	DPTR(obj)->RemoveDelegator(this);

	DASSERT(!HasLookupCycle());

	DASSERT(currGetFunc						== &DeltaTable::GetWithDelegates);
	DASSERT(currGetBySkippingUserCodeFunc	== &DeltaTable::GetWithDelegatesBySkippingUserCode);
	DASSERT(currGetByUnoverloadedAssignFunc	== &DeltaTable::GetWithDelegatesByUnoverloadedAssign);
	DASSERT(currSetFunc						== &DeltaTable::SetWithDelegates);
	DASSERT(currChangeFunc						== &DeltaTable::ChangeWithDelegates);
	DASSERT(currChangeBySkippingUserCodeFunc	== &DeltaTable::ChangeWithDelegatesBySkippingUserCode);
	DASSERT(currFindFunc					== &DeltaTable::FindWithDelegates);
	DASSERT(currFindBySkippingUserCodeFunc	== &DeltaTable::FindWithDelegatesBySkippingUserCode);

	if (!HasDelegates()) {
		currGetFunc							= currBoundedGetFunc;
		currGetByUnoverloadedAssignFunc		= currBoundedGetByUnoverloadedAssignFunc;
		currGetBySkippingUserCodeFunc		= currBasicGetBySkippingUserCodeFunc;
		currSetFunc							= currBoundedSetFunc;
		currChangeFunc						= OverloadsGet() && OverloadsSet() ? &DeltaTable::OverloadedChange : &DeltaTable::BasicChange;
		currChangeBySkippingUserCodeFunc	= &DeltaTable::BasicChangeBySkippingUserCode;
		currFindFunc						= OverloadsGet() ? &DeltaTable::OverloadedFind : &DeltaTable::BasicFind;
		currFindBySkippingUserCodeFunc		= &DeltaTable::BasicFindBySkippingUserCode;
	}

	// Verify that both the base and the child instances
	// are referencable (since they are called explicitly as 
	// parameters).
	DASSERT(IsReferencable() && DPTR(obj)->IsReferencable());

	return true;
	FAIL: return false;
}

//////////////////////////////////////////////////////////////////

bool DeltaTable::IsDirectDelegate (DeltaTable* obj) const 
	{ return std::find(myDelegates.begin(), myDelegates.end(), obj) != myDelegates.end(); }

//////////////////////////////////////////////////////////////////

bool DeltaTable::IsDelegate (DeltaTable* obj) const {
	
	DASSERT(!searchingInDelegates);
	searchingInDelegates = true;
	
	bool found = false;
	for (	std::list<DeltaTable*>::const_iterator i = myDelegates.begin(); 
			!found && i != myDelegates.end(); 
			++i	) 
			if (*i == obj || DPTR(*i)->IsDelegate(obj))
				found = true;
			
	searchingInDelegates = false;
	return found;
}

//////////////////////////////////////////////////////////////////

bool DeltaTable::IsDelegator (DeltaTable* obj) const 
	{ return myDelegators.find(obj) != myDelegators.end(); }

//////////////////////////////////////////////////////////////////

static DeltaTable* GetFromBag (ubag<DeltaTable*>::const_iterator& i)		
	{ return i->first; }

static DeltaTable* GetFromList (std::list<DeltaTable*>::const_iterator& i)		
	{ return *i; }

#define	GET_DELEGATES_OR_DELEGATORS_IMPL(suffix, cont, typeStr)											\
DeltaTable* DeltaTable::GetDelegat##suffix##AsTable (void) {											\
	if (!delegat##suffix##Array) {																		\
		delegat##suffix##Value.FromTable(delegat##suffix##Array = DeltaTableFactory::New());			\
		DPTR(delegat##suffix##Array)->GetCreationInfo().SetCreatorInformation(							\
			DELTA_CREATION_FILE_HIDDEN,																	\
			DELTA_CREATION_LINE_HIDDEN,																	\
			typeStr																						\
		);																								\
	}																									\
	GetArray(isDelegat##suffix##UpToDate, delegat##suffix##Array, myDelegat##suffix, &GetFrom##cont);	\
	DASSERT(delegat##suffix##Value.ToTable() == delegat##suffix##Array);								\
	return delegat##suffix##Array;																		\
}

GET_DELEGATES_OR_DELEGATORS_IMPL(es, List, "delegators")	// DeltaTable::GetDelegatesAsTable
GET_DELEGATES_OR_DELEGATORS_IMPL(ors, Bag, "delegates")		// DeltaTable::GetDelegatorsAsTable

//////////////////////////////////////////////////////////////////

void DeltaTable::RemoveDelegator (DeltaTable* delegator) {
	DASSERT(myDelegators.find(delegator) != myDelegators.end());
	myDelegators.remove(delegator);
	isDelegatorsUpToDate = false;
}

void DeltaTable::RemoveDelegate (DeltaTable* delegate) {
	std::list<DeltaTable*>::iterator i = std::find(myDelegates.begin(), myDelegates.end(), delegate);
	DASSERT(i != myDelegates.end());
	myDelegates.erase(i);
	isDelegatesUpToDate = false;
}

void DeltaTable::AddDelegator (DeltaTable* delegator) {
	DASSERT(myDelegators.find(delegator) == myDelegators.end());
	myDelegators.insert(delegator);
	isDelegatorsUpToDate = false;
}

void DeltaTable::AddDelegate (DeltaTable* delegate) {
	DASSERT(std::find(myDelegates.begin(), myDelegates.end(), delegate) == myDelegates.end());
	myDelegates.push_back(delegate);	// Assumed more recent delegate.
	isDelegatesUpToDate = false;
}

//---------------------------------------------------------------
// GC

// When a table is collected, all delegates and delegators are also
// under collection (recursively).

void DeltaTable::ExplicitDestructor (void) {

	if (GetDyingState().IsCompleted())
		return;

	selfValue.Undefine();
	
	if (delegatesArray)
		DASSERT(delegatesValue.ToTable() == delegatesArray);
	delegatesValue.Undefine();

	if (delegatorsArray)
		DASSERT(delegatorsValue.ToTable() == delegatorsArray);
	delegatorsValue.Undefine();

	get.Undefine();
	set.Undefine();

	if (HasDelegates()) {
		for (std::list<DeltaTable*>::iterator i = myDelegates.begin(); i != myDelegates.end(); ++i) 
			DPTR(*i)->RemoveDelegator(this);
		myDelegates.clear();
	}

	if (HasDelegators()) {
		for (ubag<DeltaTable*>::iterator i = myDelegators.begin(); i != myDelegators.end(); ++i) {
			DASSERT(DPTR(i->first)->IsCollected());
			DPTR(i->first)->RemoveDelegate(this);
		}
		myDelegators.clear();
	}

	Clear();	// All contents.

	CollectableContainer::ExplicitDestructor();
	GetDyingState().SetCompleted();
}

//////////////////////////////////////////////////////////////////

bool DeltaTable::IsAltReferencable (void) const {
	if (HasDelegators())
		for (ubag<DeltaTable*>::const_iterator i = myDelegators.begin(); i != myDelegators.end(); ++i)
			if (DPTR(i->first)->IsDirectlyReferencable() ||  DPTR(i->first)->IsReferencableRecursion())
				return true;
	return false;	
}

//////////////////////////////////////////////////////////////////
// Upon collection, all delegator objects are collected since
// there are assumed to be referres.

void DeltaTable::AltPickAndSetAsDyingAllAliveReferrers (ContList& garbage) {
	if (HasDelegators())
		for (ubag<DeltaTable*>::iterator i = myDelegators.begin(); i != myDelegators.end(); ++i)
			PickAndSetAsDyingAliveReferrer(i->first, garbage);
}

//---------------------------------------------------------------
