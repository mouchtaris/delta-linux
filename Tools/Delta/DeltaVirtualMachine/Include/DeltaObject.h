// DeltaObject.h
// The dynamic inheritance enabled vesion of a Delta
// table, following the semantics of the Delta language.
// ScriptFighter Project.
// A. Savidis, December 2005.
//

#ifndef	DELTAPROTOTYPE_H
#define	DELTAPROTOTYPE_H

#include "DeltaTable.h"
#include "DeltaObjectValidator.h"
#include "ustlhelpers.h"
#include "ContainerDelayedCollector.h"

#include <list>
#include <functional>
#include <algorithm>

//-------------------------------------------------------------

class DeltaObjectAccessCache;

#define	DELTAOBJECT_VGET_INST(val)	\
			VGET_INST(DeltaObject, val, DELTA_OBJECT_CLASS_ID)

class DVM_CLASS DeltaObject : public DeltaTable {

	protected:

	///////////////////////////////////////////////////////////////
	// Subobject trees (inheritance) support.

	typedef std::list<DeltaObject*>	ObjectList;

	ObjectList					inheritsFrom;
	DeltaObject*				donatesTo;
	DeltaObject*				mostDerived;
	ObjectList*					myTree;							// My inheritance tree (all subobjects ordered).

	DeltaTable*					bases;							// Keeps all bases as an array.
	DeltaValue					basesValue;						// The value holding the arruy.
	bool						isBasesUpToDate;

	DeltaExternId*				associatedExternId;				// 'this' is the userdata of this externid.
	
	ObjectList*					GetTree (void) 
									{ return myTree; }
	const ObjectList*			GetTree (void) const
									{ return myTree; }
	void						NullifyTree (void)
									{ unullify(myTree); }

	void						GatherAllHierarchyInstances (std::list<DeltaObject*>& putHere);
	void						CreateInheritanceTree (void);

	void						SetMostDerived (DeltaObject* inst);
	void						SetMyTree (std::list<DeltaObject*>* _myTree) 
									{ myTree = _myTree; }

	void						AddAllBasesTo (std::list<DeltaObject*>* l)
									{ ulistcopy(inheritsFrom, *l); }
	void						ConvertToStringStandardPart (DeltaString* at) const; 

	///////////////////////////////////////////////////////////////
	// Garbage collection behaviors.
	
	virtual bool				IsAltReferencable (void) const;
	virtual void				AltPickAndSetAsDyingAllAliveReferrers (ContList& garbage);
	virtual void				ExplicitDestructor (void);
	virtual void				DeleteMemory (void) 
									{ DDELETE(this); }	// Non-recyclable.

	///////////////////////////////////////////////////////////////

	template <
		bool (DeltaTable::*ChangeFunc)(const DeltaValue&, const DeltaValue&),
		class ErrorTraits
	> bool ChangeWithSubobjectsTemplate (const DeltaValue& index, const DeltaValue& content) {

		DASSERT(IsInvolvedInInheritance());

		for (ObjectList::iterator i = DPTR(myTree)->begin(); i != DPTR(myTree)->end(); ++i)
			try 
				{ if ((DPTR(*i)->*ChangeFunc)(index, content)) return true; }
			catch (uerrorclass::udisengageexception) 
				{ ErrorTraits::SetFailed(*i, "subobjects"); return false; }

		if (content.IsNil())
			ErrorTraits::RemoveNonExisting(this);
		return false;
	}

	///////////////////////////////////////////////////////////////

	template <
		bool (DeltaTable::*FindFunc)(const DeltaValue&, DeltaValue*, bool* inDelegates),
		class ErrorTraits
	> bool FindWithSubobjectsTemplate (const DeltaValue& index, DeltaValue* content, bool* inDelegates) {

		DASSERT(IsInvolvedInInheritance());

		for (ObjectList::iterator i = DPTR(myTree)->begin(); i != DPTR(myTree)->end(); ++i)
			try 
				{ if ((DPTR(*i)->*FindFunc)(index, content, inDelegates)) return true; }
			catch (uerrorclass::udisengageexception) 
				{ ErrorTraits::GetFailed(*i, "subobjects"); return false; }

		return false;
	}

	virtual bool				ChangeWithSubobjects					(const DeltaValue& index, const DeltaValue& content);
	virtual bool				ChangeWithSubobjectsBySkippingUserCode	(const DeltaValue& index, const DeltaValue& content);
	virtual bool				FindWithSubobjects						(const DeltaValue& index, DeltaValue* content, bool* inDelegates);
	virtual bool				FindWithSubobjectsBySkippingUserCode	(const DeltaValue& index, DeltaValue* content, bool* inDelegates);

	virtual bool				HasLookupCycle (void) const;

	//*****************************
	// Visiting with the lookup order.

	bool						inVisit;
	bool						VisitInheritance (bool (*pred)(DeltaObject*, void*), void* closure);
	bool						VisitDelegation (bool (*pred)(DeltaObject*, void*), void* closure);

	///////////////////////////////////////////////////////////////

	public:

	//*****************************
	// Virtual functions (of DeltaTable) refined for lookup and string
	// conversion.

	virtual void				ConvertToString (DeltaString* at, void* closure = (void*) 0);
	virtual void				ConvertToStringBySkippingUserCode (DeltaString* at, void* closure = (void*) 0);

	virtual bool				Set						(const DeltaValue& index, const DeltaValue& content);
	virtual bool				SetBySkippingUserCode	(const DeltaValue& index, const DeltaValue& content);

	virtual bool				Get						(const DeltaValue& index, DeltaValue* content);
	virtual bool				GetBySkippingUserCode	(const DeltaValue& index, DeltaValue* content);
	virtual bool				GetByUnoverloadedAssign (const DeltaValue& index, DeltaValue* content);

	virtual DeltaTable*			New (void) const;
	static DeltaObject*			NewObject (void);

	//***********************
	// Inheritance methods (subobject trees).

	bool						Inherit (DeltaObject* from);
	bool						InheritRedirect (DeltaObject* from);
	bool						UnInherit (DeltaObject* from);
	bool						IsDerived (DeltaObject* base) const;

	bool						IsInvolvedInInheritance (void) const
									{ return donatesTo || inheritsFrom.size(); }

	DeltaObject*				GetMostDerived (void) 
									{ DASSERT(mostDerived); return mostDerived; }
	DeltaObject*				GetDerived (void) 
									{ return donatesTo; }
	const std::list<DeltaObject*>&
								GetBases (void) const 
									{ return inheritsFrom; }
	DeltaObject*				GetBase (util_ui32 i) const {
									return	i < inheritsFrom.size() ? 
												ulistgetbyindex(inheritsFrom, i) : 
												(DeltaObject*) 0; 
								}
	DeltaTable*					GetBasesAsTable (void);

	//***********************
	// Linkage to extern ids when being user data.
	// Allows to treat userdata objects as externids
	// within library methods or functions.

	bool						IsUserDataOfExternId (void) const {
									if (associatedExternId)
										{ DASSERT(associatedExternId->GetUserData() == this); return true; }
									return false;
								}
	void						ResetIsUserDataOfExternId (DeltaExternId* externId) {	// When externid is to be destroyed.
									DASSERT(GetExternIdWhenUserData() == externId);
									unullify(associatedExternId);
								}
	void						SetIsUserDataOfExternId (DeltaExternId* externId) {
									DASSERT(!associatedExternId && externId->GetUserData() == this); 
									associatedExternId = externId; 
								}
	DeltaExternId*				GetExternIdWhenUserData (void)
									{ DASSERT(IsUserDataOfExternId()); return associatedExternId; }

	//***********************
	// Visitor respecting the inheritance and delegation.

	typedef bool (*VisitorPred)(DeltaObject* curr, void* closure);
	bool						Visit (VisitorPred pred, void* closure);	// False in case of error.

	//***********************
	// Native code helpers to avoid mess up with GC details.
	// Never use ref counting with these methods (they encapsulate all
	// the necessary GC invocations).
	//
	struct DVM_CLASS NativeCodeHelpers {
		static DeltaObject*		NewObject (void);					// New dynamic object to be at least stored in native code.
		static DeltaTable*		NewTable (void);					// New dynamic table to be at least stored in native code.
		static void				Delete (DeltaTable* t);				// Delete a dynamic native object / table, expecting none refers to it at this point.
		static void				GiveUp (DeltaTable* t);				// Forget a dynamic native object / table, expecting Delta objects or native code refers to it.
		static void				Refer (DeltaTable* t);				// Refer to an existing object/ table from native code.
		static DeltaTable*		Refer (const DeltaValue& obj);		// Refer to a object / table of a Delta object from native code.
		static void				Unrefer (DeltaTable* t);			// Unrefer to a object / table we referred explicitly earlier.
	};

	//**************************
	// Virtual functions required for Validatable base class.

	virtual const char*			GetExtClassString (void) const 
									{ return DELTA_OBJECT_CLASS_ID; }
	
	DYNAMIC_CAST_SINGLEBASE_INLINE_IMPL(
		DeltaTable, 
		DELTA_OBJECT_CLASS_ID
	)

	DeltaObject (void);
	virtual ~DeltaObject();
};

//-------------------------------------------------------------

#endif	// Do not add stuff beyond this point.