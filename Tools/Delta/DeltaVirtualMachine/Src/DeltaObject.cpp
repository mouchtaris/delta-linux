// DeltaObject.cpp
// The dynamic inheritance enabled vesion of a Delta
// table, following the semantics of the Delta language.
// ScriptFighter Project.
// A. Savidis, December 2005.
// Refactored March 2007, turned overloaded '.' consistently 
// interoperate with inheritance, dropped '..' (redundant).
// Refactored June 2009, changed the GC method for subobject trees.
// Refactoring, February 2011, turned the basic set / get functions 
// to templates.
//
#include "DeltaObject.h"
#include "DeltaObjectValidator.h"
#include "DDebug.h"
#include "DeltaVirtualMachine.h"
#include "DeltaRuntimeError.h"
#include "ContainerDelayedCollector.h"
#include "DeltaTableFactory.h"
#include "uerrorclass.h"
#include "ufunctors.h"

#define	ERROR_HANDLER		DELTATABLE_ERROR_HANDLER

///////////////////////////////////////////////////////////////

DeltaObject* DeltaObject::NativeCodeHelpers::NewObject (void) {
	DeltaObject* obj = DeltaObject::NewObject();
	obj->IncRefCounter((DeltaValue*) 0);
	return obj;
}

DeltaTable* DeltaObject::NativeCodeHelpers::NewTable (void) {
	DeltaTable* t = DeltaTableFactory::New();
	t->IncRefCounter((DeltaValue*) 0);
	return t;
}

void DeltaObject::NativeCodeHelpers::Delete (DeltaTable* t)
	{ DPTR(t)->DecRefCounter((DeltaValue*) 0); }

void DeltaObject::NativeCodeHelpers::GiveUp (DeltaTable* t)
	{ DPTR(t)->DecRefCounter((DeltaValue*) 0); }

void DeltaObject::NativeCodeHelpers::Refer (DeltaTable* t)
	{ DPTR(t)->IncRefCounter((DeltaValue*) 0); }

DeltaTable* DeltaObject::NativeCodeHelpers::Refer (const DeltaValue& obj) {
	DeltaTable* t = const_cast<DeltaValue&>(obj).ToTable();
	DPTR(t)->IncRefCounter((DeltaValue*) 0); 
	return t;
}

void DeltaObject::NativeCodeHelpers::Unrefer (DeltaTable* t)
	{ DPTR(t)->DecRefCounter((DeltaValue*) 0); }

///////////////////////////////////////////////////////////////

DeltaTable*	DeltaObject::New (void) const
	{ return DNEW(DeltaObject); }

DeltaObject* DeltaObject::NewObject (void)
	{ return DNEW(DELTA_OBJECT); }

DeltaObject::DeltaObject (void) {

	unullify(donatesTo);
	unullify(myTree);
	unullify(associatedExternId);
	mostDerived			= this;
	inVisit				= false;

	basesValue.SetResidencyType(DeltaValue::Contained, (CollectableContainer*) this);
	basesValue.FromTable(bases = DeltaTableFactory::New());
	DPTR(bases)->GetCreationInfo().SetCreatorInformation(
		DELTA_CREATION_FILE_HIDDEN,
		DELTA_CREATION_LINE_HIDDEN,
		"bases"
	);			
	isBasesUpToDate = true;

	// Although we register in the constructor, we do not
	// cancel in the destructor, but inside the instance clearing
	// functor.
	//
	GetDeltaObjectValidator()->Register(this);
}

///////////////////////////////////////////////////////////////

DeltaObject::~DeltaObject() {
	if (!IsCollected())
		GetDyingState().SetInProgress(true);
	ExplicitDestructor(); 
}

///////////////////////////////////////////////////////////////
// The instances are collected and sorted in a Breadth First Search fashion,
// left to right for those in the same level of the inheritrance hierarchy.
// Collects all hierarchy instances for the caller's instance
// hierarchy. It is based on an iterative algorithm that uses two swapped
// list pointers to collect the base instances of a particular list of child
// instances.
//
void DeltaObject::GatherAllHierarchyInstances (std::list<DeltaObject*>& putHere) {

	std::list<DeltaObject*>	la, lb;
	std::list<DeltaObject*>* curr = &la;
	std::list<DeltaObject*>* next = &lb;

	curr->push_back(mostDerived);

	while (true) {
		
		DASSERT((curr==&la && next==&lb) || (curr==&lb && next==&la));

		ulistcopy(*curr, putHere);

		std::for_each(
			curr->begin(), 
			curr->end(),
			std::bind2nd(std::mem_fun(&DeltaObject::AddAllBasesTo), next)
		);
		curr->clear();

		// Termination condition.
		if (next->empty())
			return;

		// Swap the lists.
		std::list<DeltaObject*>* temp	= curr;
		curr							= next;
		next							= temp;
	}
}

///////////////////////////////////////////////////////////////
// Sets the most derived to 'inst' for 'base' and every
// base instance of it.
//
void DeltaObject::SetMostDerived (DeltaObject* inst) {

	DASSERT(GetMostDerived() != inst);

	mostDerived = inst;

	std::for_each(
		inheritsFrom.begin(),
		inheritsFrom.end(),
		std::bind2nd(std::mem_fun(&DeltaObject::SetMostDerived), inst)
	);
}

///////////////////////////////////////////////////////////////

void DeltaObject::CreateInheritanceTree (void) {

	DASSERT(GetTree() && IsInvolvedInInheritance());

	GatherAllHierarchyInstances(*myTree);

	std::for_each(
		DPTR(myTree)->begin(), 
		DPTR(myTree)->end(),
		std::bind2nd(std::mem_fun(&DeltaObject::SetMyTree), myTree)
	);
}

///////////////////////////////////////////////////////////////

#define	INHERIT_ERROR_MESSAGE_STD_PREFIX "in inherit " DELTA_OBJECT_SN_FORMAT " base"

bool DeltaObject::Inherit (DeltaObject* from) {
	
	DASSERT(from);
	DASSERT(!HasLookupCycle());

	if (DPTR(from)->donatesTo) {

		if (DPTR(from)->donatesTo == this) {
			UCHECK_PRIMARY_ERROR(
				false,
				uconstructstr(
					INHERIT_ERROR_MESSAGE_STD_PREFIX "(base already donates to this object)!",
					DPTR(from)->GetSerialNo()
				)
			);
		}

		UCHECK_PRIMARY_ERROR(
			false,
			uconstructstr(
				INHERIT_ERROR_MESSAGE_STD_PREFIX  "(base already donates to another object)!",
				DPTR(from)->GetSerialNo()
			)
		);
	}
	else
	if (DPTR(from)->IsDerived(this)) {
		UCHECK_PRIMARY_ERROR(
			false,
			uconstructstr(
				INHERIT_ERROR_MESSAGE_STD_PREFIX "(base derived from inheritor - cyclic inheritance)!",
				DPTR(from)->GetSerialNo()
			)
		);
	}
	else {
	
		// The derived is put as a leftmost child (front)
		// meaning it is assumed as a more recent derivative.
		DPTR(from)->donatesTo = this;
		inheritsFrom.push_front(from);
		isBasesUpToDate = false;

		// Now, the base instance has as most-derived the
		// most-derived of its directly derived instance.
		DPTR(from)->SetMostDerived(mostDerived);

		// Create the inheritance tree.
		if (myTree) {
			udeleteunlessnull(DPTR(from)->myTree);
			DPTR(myTree)->clear();
		}
		else
		if (DPTR(from)->myTree) {
			myTree = DPTR(from)->myTree;
			DPTR(myTree)->clear();
		}
		else
			myTree = DNEW(std::list<DeltaObject*>);

		CreateInheritanceTree();

		if (HasLookupCycle()) {

			UnInherit(from);

			UCHECK_PRIMARY_ERROR(
				false,
				uconstructstr(
					INHERIT_ERROR_MESSAGE_STD_PREFIX "(causes a lookup cycle)!",
					DPTR(from)->GetSerialNo()
				)
			);
		}

		DASSERT(!HasLookupCycle());
	}

	return true;
	FAIL: return false;
}

///////////////////////////////////////////////////////////////

bool DeltaObject::InheritRedirect (DeltaObject* from) {

	if (DPTR(from)->donatesTo) {
		if (DPTR(from)->donatesTo != this) {
			bool result = DPTR(DPTR(from)->donatesTo)->UnInherit(from);
			DASSERT(result);
		}
		else
			return true;
	}

	return Inherit(from);
}

///////////////////////////////////////////////////////////////

bool DeltaObject::IsDerived (DeltaObject* base) const{
	if (inheritsFrom.empty())
		return false;
	else	// ('base' in inheritsFrom) or (for x in inheritsFrom, x->IsDerived(base) )
		return	std::find_if(
					inheritsFrom.begin(),
					inheritsFrom.end(),
					ubinary_or(
						std::bind2nd(std::equal_to<DeltaObject*>(), base),
						std::bind2nd(std::mem_fun(&DeltaObject::IsDerived), base)
					)
				) != inheritsFrom.end();
}

///////////////////////////////////////////////////////////////

#define	UNINHERIT_ERROR_MESSAGE_STD_PREFIX "in uninherit " DELTA_OBJECT_SN_FORMAT " base"

bool DeltaObject::UnInherit (DeltaObject* from) {

	if (DPTR(from)->donatesTo != this) {	// 'from' does not inherit to caller.

		DASSERT(			// We should not find 'from' in the direct bases.
			std::find(
					inheritsFrom.begin(),
					inheritsFrom.end(),
					from
			) == inheritsFrom.end()
		);

		UCHECK_PRIMARY_ERROR(
			false,
			uconstructstr(
				UNINHERIT_ERROR_MESSAGE_STD_PREFIX " (base does not donate to this object)!",
				DPTR(from)->GetSerialNo()
			)
		);
	}
	else {

		std::list<DeltaObject*>::iterator pos;

		pos =	std::find(	// We should find 'from' in the direct bases.
					inheritsFrom.begin(),
					inheritsFrom.end(),
					from
				);

		// Now remove from the list of base instances.
		//
		DASSERT(pos != inheritsFrom.end());
		inheritsFrom.erase(pos);
		isBasesUpToDate = false;

		// Cut the hierarchy starting from base instace 'from', setting
		// the most derived to be 'from'.
		//
		unullify(DPTR(from)->donatesTo);
		DPTR(from)->SetMostDerived(from);
		
		// Now split the inheritance trees. We keep the tree for the derived instance.
		// We create a new for the base instance. We check if instances are left alone.
		//
		if (IsInvolvedInInheritance()) {
			unullify(DPTR(from)->myTree);	// Current object steels tree from its old ('from') base.
			DPTR(myTree)->clear();
			CreateInheritanceTree();
		}
		else  
			unullify(myTree);

		if (DPTR(from)->IsInvolvedInInheritance())	// Now reconstruct the base subtree.
			if (!DPTR(from)->myTree) {
				DPTR(from)->myTree = DNEW(std::list<DeltaObject*>);
				DPTR(from)->CreateInheritanceTree();
			}
			else									// Remove derived instance from the old tree.
				DPTR(from)->myTree->remove(this);
		else
			udeleteunlessnull(DPTR(from)->myTree);

		// Verify that both the base and the child instances
		// are referencable (since they are called explicitly as 
		// actual arguments they will be on the stack).

		DASSERT(IsReferencable() && DPTR(from)->IsReferencable());
		DASSERT(!HasLookupCycle());
	}

	return true;
	FAIL: return false;
}

///////////////////////////////////////////////////////////////

void DeltaObject::ConvertToStringStandardPart (DeltaString* at) const {

	std::string basePart, derivedPart;

	if (inheritsFrom.size())
		basePart = uconstructstr("bases(%u)", (util_ui32) inheritsFrom.size());

	if (donatesTo)
		derivedPart = uconstructstr("derived " DELTA_OBJECT_SN_FORMAT, donatesTo->GetSerialNo());

	if (basePart.size() || derivedPart.size())
		at->Add(
			std::string("<")		+
			basePart				+
			derivedPart				+
			std::string(">:")
		);
}

///////////////////////////////////////////////////////////////

void DeltaObject::ConvertToStringBySkippingUserCode (DeltaString* at, void* closure) {
	ConvertToStringStandardPart(at);
	DeltaTable::ConvertToStringBySkippingUserCode(at, closure);
}

void DeltaObject::ConvertToString (DeltaString* at, void* closure) {
	ConvertToStringStandardPart(at);
	DeltaTable::ConvertToString(at, closure);
}

//-------------------------------------------------------------

bool DeltaObject::Set (const DeltaValue& index, const DeltaValue& content) {

	UCHECK_DOMINO_ERROR(CheckLegalIndex(index), "in 'set' (illegal index)");

	if (!IsInvolvedInInheritance())	
		return (this->*DeltaTable::currSetFunc)(index, content);
	else 
	try {
		if (!ChangeWithSubobjectsTemplate<&DeltaTable::Change, DeltaTable::LookupErrorTraits>(index, content))
			SetNew(index, content);
	}
	catch (uerrorclass::udisengageexception)
		{ return false; }

	return true;
	FAIL: return false;
}

///////////////////////////////////////////////////////////////

bool DeltaObject::SetBySkippingUserCode (const DeltaValue& index, const DeltaValue& content) {
	if (!IsLegalIndex(index))
		return false;
	else
	if (!IsInvolvedInInheritance())
		return DeltaTable::SetBySkippingUserCode(index, content);
	else
	try {
		if (!ChangeWithSubobjectsTemplate<&DeltaTable::ChangeBySkippingUserCode, DeltaTable::SkipLookupErrorTraits>(index, content))
			BasicSetNew(index, content);
		return true;
	}
	catch (uerrorclass::udisengageexception)
		{ return false; }
}

///////////////////////////////////////////////////////////////

bool DeltaObject::ChangeWithSubobjects (const DeltaValue& index, const DeltaValue& content) {
	if (!IsInvolvedInInheritance())
		return DeltaTable::ChangeWithSubobjects(index, content);
	else 
		return ChangeWithSubobjectsTemplate<&DeltaTable::Change, DeltaTable::LookupErrorTraits>(index, content);
}

///////////////////////////////////////////////////////////////

bool DeltaObject::ChangeWithSubobjectsBySkippingUserCode (const DeltaValue& index, const DeltaValue& content) {
	if (!IsInvolvedInInheritance())
		return DeltaTable::ChangeWithSubobjectsBySkippingUserCode(index, content);
	else
		return ChangeWithSubobjectsTemplate<&DeltaTable::ChangeBySkippingUserCode, DeltaTable::SkipLookupErrorTraits>(index, content);
}

//-------------------------------------------------------------
// GET

bool DeltaObject::FindWithSubobjects (const DeltaValue& index, DeltaValue* content, bool* inDelegates) {
	if (!IsInvolvedInInheritance())
		return DeltaTable::FindWithSubobjects(index, content, inDelegates);
	else 
		return FindWithSubobjectsTemplate<&DeltaTable::Finder, DeltaTable::LookupErrorTraits>(index, content, inDelegates);
}

///////////////////////////////////////////////////////////////

bool DeltaObject::FindWithSubobjectsBySkippingUserCode (const DeltaValue& index, DeltaValue* content, bool* inDelegates){
	if (!IsInvolvedInInheritance())
		return DeltaTable::FindWithSubobjectsBySkippingUserCode(index, content, inDelegates);
	else
		return FindWithSubobjectsTemplate<&DeltaTable::FinderBySkippingUserCode, DeltaTable::SkipLookupErrorTraits>(index, content, inDelegates);
}

///////////////////////////////////////////////////////////////

bool DeltaObject::Get (const DeltaValue& index, DeltaValue* content) {

	UCHECK_DOMINO_ERROR(CheckLegalIndex(index), "in 'get' (illegal index)");

	if (!IsInvolvedInInheritance())
		return (this->*DeltaTable::currGetFunc)(index, content);
	else 
	try {
		bool		inDelegates = false;
		DeltaValue result(DeltaValue::Nil);

		if (FindWithSubobjectsTemplate<&DeltaTable::Finder, DeltaTable::LookupErrorTraits>(index, &result, &inDelegates))
			if (inDelegates && ChangeSelfWhenDelegateMethod(&result))
				content->Assign(result);
			else
				return content->AssignOverloadable(result);
		else
			content->FromNil();
		return true;
	}
	catch (uerrorclass::udisengageexception)
		{ return false; }

	FAIL: return false;
}

///////////////////////////////////////////////////////////////

bool DeltaObject::GetByUnoverloadedAssign (const DeltaValue& index, DeltaValue* content) {

	UCHECK_DOMINO_ERROR(CheckLegalIndex(index), "in 'get' (illegal index)");

	if (!IsInvolvedInInheritance())
		return (this->*DeltaTable::currGetByUnoverloadedAssignFunc)(index, content);
	else 
	try {
		bool inDelegates = false;
		content->FromNil();
		if (FindWithSubobjectsTemplate<&DeltaTable::Finder, DeltaTable::LookupErrorTraits>(index, content, &inDelegates))
			if (inDelegates)
				ChangeSelfWhenDelegateMethod(content);
		return true;
	}
	catch (uerrorclass::udisengageexception)
		{ return false; }

	FAIL: return false;
}

///////////////////////////////////////////////////////////////

bool DeltaObject::GetBySkippingUserCode (const DeltaValue& index, DeltaValue* content) {
	if (!IsLegalIndex(index))
		return false;
	else
	if (!IsInvolvedInInheritance())
		return (this->*DeltaTable::currGetBySkippingUserCodeFunc)(index, content);
	else 
	try {
		bool inDelegates = false;
		content->FromNil();
		if (FindWithSubobjectsTemplate<&DeltaTable::FinderBySkippingUserCode, DeltaTable::SkipLookupErrorTraits>(index, content, &inDelegates))
			if (inDelegates)
				ChangeSelfWhenDelegateMethod(content);
		return true;
	}
	catch (uerrorclass::udisengageexception)
		{ return false; }
}

//-------------------------------------------------------------

static DeltaTable* GetFromList (std::list<DeltaObject*>::const_iterator& i)	
	{ return *i; }

DeltaTable* DeltaObject::GetBasesAsTable (void)
	{ return GetArray(isBasesUpToDate, bases, inheritsFrom, &GetFromList); }

//-------------------------------------------------------------
// VISITOR. Never alters the visItited structure (no overloading called).

#define	VISIT_REENTRANCY_CHECK(func)											\
	UCHECK_PRIMARY_ERROR(														\
		!inVisit,																\
		uconstructstr(															\
			"in " DELTA_OBJECT_SN_FORMAT " cycle caused while visiting (%s)",	\
			GetSerialNo(), #func												\
		)																		\
	)

#define	IF_SATISFIES_PREDICATE(who)								\
		if ((*pred)(who, closure))								\
			return true

#define	ENTER_VISIT(who)										\
			(who)->inVisit = true

#define	EXIT_VISIT(who)											\
			(who)->inVisit = false

#define	RETURN_AND_CONTINUE_VISIT()								\
	if (true) { EXIT_VISIT(this); return false; } else

#define	ERROR_RETURN_STOP_VISIT()								\
	if (true) { EXIT_VISIT(this); return false; } else

#define	RETURN_AND_FINISH_VISIT()								\
	if (true) { EXIT_VISIT(this); return true; } else

///////////////////////////////////////////////////////////////

bool DeltaObject::VisitInheritance (VisitorPred pred, void* closure) {

	VISIT_REENTRANCY_CHECK(inheritance)
	DASSERT(IsInvolvedInInheritance());

	for (std::list<DeltaObject*>::iterator i = DPTR(myTree)->begin(); i != DPTR(myTree)->end(); ++i)
		if (DPTR(*i)->HasDelegates()) {
			if (DPTR(*i)->VisitDelegation(pred, closure))
				RETURN_AND_FINISH_VISIT();
			else
			if (UERROR_ISRAISED())
				{ ERROR_RETURN_STOP_VISIT(); }
		}
		else
		IF_SATISFIES_PREDICATE(*i);

	FAIL: return false;	// Fallback case too.
}

///////////////////////////////////////////////////////////////

bool DeltaObject::VisitDelegation (VisitorPred pred, void* closure) {
	
	VISIT_REENTRANCY_CHECK(delegation)
	DASSERT(HasDelegates());

	IF_SATISFIES_PREDICATE(this);
	else {

		ENTER_VISIT(this);
		std::list<DeltaTable*> currList, nextList;
		ulistcopy(GetDelegates(), currList);

		while (true) {																
			DASSERT(currList.size());
			for (std::list<DeltaTable*>::iterator i = currList.begin(); i != currList.end(); ++i)
				if ((*i)->IsOfClass(DELTA_OBJECT_CLASS_ID)) {	// We only visit prototypes.
					DeltaObject* proto = (DeltaObject*) *i;
					DASSERT(GetDeltaObjectValidator()->Validate(proto));
					if (DPTR(proto)->IsInvolvedInInheritance()) {
						if (DPTR(proto)->VisitInheritance(pred, closure))
							RETURN_AND_FINISH_VISIT();
						else
						if (UERROR_ISRAISED())
							{ ERROR_RETURN_STOP_VISIT(); }
					}
					else
					IF_SATISFIES_PREDICATE(proto);
					
					if (DPTR(*i)->HasDelegates())
						ulistcopy(DPTR(*i)->GetDelegates(), nextList);
				}

			if (nextList.empty())
				RETURN_AND_CONTINUE_VISIT();
			else {
				currList.clear(); 
				ulistcopy(nextList, currList); 
				nextList.clear();
			}		
		}		
	}

	FAIL: return false;
}

///////////////////////////////////////////////////////////////

bool DeltaObject::Visit (VisitorPred pred, void* closure) {
	if (IsInvolvedInInheritance())
		VisitInheritance(pred, closure); 
	else
	if (HasDelegates())
		VisitDelegation(pred, closure);
	else
		IF_SATISFIES_PREDICATE(this);

	UCHECK_DOMINO_ERROR(
		!UERROR_ISRAISED(), 
		uconstructstr("in " DELTA_OBJECT_SN_FORMAT "while visiting", GetSerialNo())
	);
	return true;
	FAIL: return false;
}

///////////////////////////////////////////////////////////////

bool DeltaObject::HasLookupCycle (void) const {
	if (!IsInvolvedInInheritance())
		return DeltaTable::HasLookupCycle();
	else {
		for (std::list<DeltaObject*>::const_iterator i = DPTR(myTree)->begin(); i != DPTR(myTree)->end(); ++i)
			if (DPTR(*i)->DeltaTable::HasLookupCycle())
				return true;
		return false;
	}
}

//-------------------------------------------------------------
// GC

void DeltaObject::ExplicitDestructor (void) {

	if (GetDyingState().IsCompleted())	// We are done with destruction.
		return;

	DASSERT(GetDyingState().IsInProgress());
	DASSERT(DPTR(GetDeltaObjectValidator())->Validate(this));

	basesValue.Undefine();

	// Only the first subobject (invoked) in  a subobject tree will actually destroy
	// myTree, while it will call all subobjects to nullify it. Additionally,
	// it will call the most derived subobject to destroy the cache.

	if (GetTree()) {

		for (ObjectList::iterator i = DPTR(myTree)->begin(); i != DPTR(myTree)->end(); ++i)
			if (*i != this)
				{ DASSERT(DPTR(*i)->IsCollected()); DPTR(*i)->NullifyTree(); }
		udelete(myTree);
	}

	DeltaTable::ExplicitDestructor();

	DPTR(GetDeltaObjectValidator())->Cancel(this);
	GetDyingState().SetCompleted();
}

///////////////////////////////////////////////////////////////
// If there is any other object in the subobject tree that
// is referencable, then every object is referencable.

bool DeltaObject::IsAltReferencable (void) const {

	if (DeltaTable::IsAltReferencable())	// Check via delegation
		return true;
	else
	if (GetTree()) {			// Check via inheritance
		DASSERT(IsInvolvedInInheritance());
		for (ObjectList::const_iterator i = DPTR(myTree)->begin(); i != DPTR(myTree)->end(); ++i)
			if (DPTR(*i)->IsDirectlyReferencable() || DPTR(*i)->IsReferencableRecursion())
				return true;
		return false;
	}
	else
		{ DASSERT(!IsInvolvedInInheritance()); return false; }
}

//////////////////////////////////////////////////////////////////
// Upon collection, all objects in a subobject tree are collected since
// there are assumed to be referrers.

void DeltaObject::AltPickAndSetAsDyingAllAliveReferrers (ContList& garbage) {

	DeltaTable::AltPickAndSetAsDyingAllAliveReferrers(garbage);

	if (GetTree()) {
		DASSERT(IsInvolvedInInheritance());
		for (ObjectList::iterator i = DPTR(myTree)->begin(); i != DPTR(myTree)->end(); ++i)
			PickAndSetAsDyingAliveReferrer(*i, garbage);
	}
	else
		DASSERT(!IsInvolvedInInheritance());
}

//-------------------------------------------------------------
