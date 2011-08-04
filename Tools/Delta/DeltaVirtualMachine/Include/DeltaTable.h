// DeltaTable.h
// Base class for objects in Delta.
// Delta Virtual Machine.
// ScriptFighter Project.
// A. Savidis, October 1999 (original version).
// Revised January 2004 to incorporate new garbage collection
// method shifted in a base class.
//

#ifndef	DELTATABLE_H
#define	DELTATABLE_H

//-------------------------------------------------------------

#include "DeltaByteCodeTypes.h"
#include "DeltaTableP.h"
#include "DeltaCollectableContainer.h"
#include "DeltaValue.h"
#include "DeltaString.h"
#include "DeltaTableFactory.h"
#include "DeltaCreationInfo.h"
#include "uhash.h"
#include "utypes.h"
#include "ufunctors.h"
#include "uvalidatable.h"
#include "uderivablecheck.h"
#include "ucallbacks.h"
#include "usafeiterators.h"
#include <list>

#define	DELTATABLE_VGET_INST(val)	VGET_INST(DeltaTable, val, DELTA_TABLE_CLASS_ID)
#define	DELTA_OBJECT_SN_FORMAT		"sn(0x%x)"

#define	DELTATABLE_ERROR_HANDLER(what, errclass)			\
	uerror::GetSingleton().post##errclass(					\
		"In '%s' " DELTA_OBJECT_SN_FORMAT ": error %s!",	\
		GetExtClassString(),								\
		GetSerialNo(),										\
		what												\
	); goto FAIL;


//--------------------------------------------------------------

class DVM_CLASS DeltaTable :	public CollectableContainer, 
								public usafecontainerbehavior {
	
	public:
	class DVM_CLASS Element;

	protected:
	template <class T, class C>  friend class basic_iterator;
	template <void (*SetContentFunc)(DeltaTable*, Element*, const DeltaValue&)>			friend struct SetImplTraits;
	template <void (*GetContentFunc)(DeltaTable*, DeltaTable::Element*, DeltaValue*)>	friend struct GetImplTraits;

	struct GetContentTraits;
	struct SetContentTraits;
	struct InternalDelegatesAccessTraits;
	struct DelegatesAccessTraits;
	struct LookupErrorTraits;
	struct SkipLookupErrorTraits;

	friend class DeltaObject;	
	friend struct DeltaTable::GetContentTraits;
	friend struct DeltaTable::SetContentTraits;
	friend struct DeltaTable::InternalDelegatesAccessTraits;
	friend struct DeltaTable::DelegatesAccessTraits;
	friend struct DeltaTable::LookupErrorTraits;
	friend struct DeltaTable::SkipLookupErrorTraits;

	///////////////////////////////////////////////////////

	public:

	enum ElementType { FieldType = 0, AttributeType = 1 };

	class DVM_CLASS Element {

		protected:
		DeltaTable*		owner; 
		Element*		next;

		public:
		virtual ElementType		GetType (void) const = 0;
		
		virtual bool			SetContent (const DeltaValue& val) = 0;					// via attribute methods (if any)
		virtual bool			SetContentDirectly (const DeltaValue& val) = 0;			// straight content, overloaded assign
		virtual bool			SetContentBySkippingUserCode (const DeltaValue& val) = 0;	// straight content, simple assign

		virtual bool			GetContent (DeltaValue* at) const = 0;						// attribute methods (if any), no overloaded assign
		virtual bool			GetContentDirectly  (DeltaValue* at) const = 0;			// straight content, no overloaded assign
		virtual bool			GetContentBySkippingUserCode  (DeltaValue* at) const = 0;	// straight content, no overloaded assign

		virtual DeltaValue*	GetContentPtr (void) = 0;
		virtual const DeltaValue*	
								GetContentPtr (void) const = 0;
		virtual bool			GetIndex  (DeltaValue* at) const = 0;
		virtual const DeltaValue
								GetIndex (void) const = 0;
		virtual bool			MatchesIndex (const DeltaValue& val) const = 0;
		DeltaTable*				GetOwner (void) const 
									{ return owner; }
		Element*				GetNext (void) const 
									{ return next; }
		void					SetNext (Element* _next) 
									{ next = _next; }
		virtual Element*		Clone (DeltaTable* container) const = 0;
		virtual void			CheckChangeSelfForMethods (const DeltaTable* from, DeltaTable* to) = 0;

		Element (DeltaTable* _owner) : owner(_owner), next ((Element*) 0){}
		virtual ~Element(){}
	};

	//--------------------------------------------------------------

	protected:

	class DVM_CLASS Field : public Element {

		friend class DeltaTable;
		private:
		DeltaValue		index;
		DeltaValue		content;

		public:
		virtual ElementType		GetType (void) const
									{ return FieldType; }

		virtual bool			SetContent (const DeltaValue& val);
		virtual bool			SetContentDirectly (const DeltaValue& val)
									{ SetContent(val);  return true; }
		virtual bool			SetContentBySkippingUserCode (const DeltaValue& val) 
									{ content.Assign(val);  return true; }

								// All three version intentionally equivalent.
		virtual bool			GetContent (DeltaValue* at) const
									{ DASSERT(at); at->Assign(content);  return true; }
		virtual bool			GetContentDirectly  (DeltaValue* at) const
									{ DASSERT(at); at->Assign(content);  return true; }
		virtual bool			GetContentBySkippingUserCode  (DeltaValue* at) const
									{ DASSERT(at); at->Assign(content);  return true; }

		virtual DeltaValue*	GetContentPtr (void) 
									{ return &content; }
		virtual const DeltaValue*	
								GetContentPtr (void) const
									{ return &content; }
		virtual bool			GetIndex  (DeltaValue* at) const
									{ DASSERT(at); at->AssignOverloadable(index);  return true; }
		virtual const DeltaValue
								GetIndex (void) const
									{ return index; }
		virtual bool			MatchesIndex (const DeltaValue& val) const
									{ return index.Equal(val); }
		virtual Element*		Clone (DeltaTable* container) const;
		virtual void			CheckChangeSelfForMethods (const DeltaTable* from, DeltaTable* to);

		Field (
			DeltaTable*			_owner, 
			const DeltaValue&	_index, 
			const DeltaValue&	_content
		);
	};

	///////////////////////////////////////////////////////

	public:

	class DVM_CLASS Attribute  : public Element {
		friend class DeltaTable;
		private:
		std::string			id;
		DeltaValue			content;
		DeltaValue			set;
		bool				setting;
		mutable DeltaValue	get;
		mutable bool		getting;
		
		///////////////////////////////////////////////////////

		public:
		virtual ElementType		GetType (void) const
									{ return AttributeType; }
		const std::string&		GetId (void) const
									{ return id; }
		virtual bool			SetContent (const DeltaValue& val);
		virtual bool			SetContentDirectly (const DeltaValue& val);
		virtual bool			SetContentBySkippingUserCode (const DeltaValue& val) 
									{ content.Assign(val);  return true; }

		virtual DeltaValue*	GetContentPtr (void) 
									{ return (DeltaValue*) 0; }
		virtual const DeltaValue*	
								GetContentPtr (void) const
									{ return (DeltaValue*) 0; }
		virtual bool			GetContent (DeltaValue* at) const;
		virtual bool			GetContentDirectly  (DeltaValue* at) const
									{ DASSERT(at); at->AssignOverloadable(content);  return true; }
		virtual bool			GetContentBySkippingUserCode  (DeltaValue* at) const
									{ DASSERT(at); at->Assign(content);  return true; }

		virtual bool			GetIndex  (DeltaValue* at) const
									{ DASSERT(at); at->FromString(id);  return true; }
		virtual const DeltaValue
								GetIndex (void) const
									{ return id; }
		virtual bool			MatchesIndex (const DeltaValue& val) const
									{ return val.Type() == DeltaValue_String && val.ToString() == id; }

		virtual Element*		Clone (DeltaTable* container) const;
		virtual void			CheckChangeSelfForMethods (const DeltaTable* from, DeltaTable* to);

		const DeltaValue&		GetGetter (void) const 
									{ return get; }
		const DeltaValue&		GetSetter (void) const 
									{ return set; }

		private:
		Attribute (
			DeltaTable*			_owner, 
			const std::string&	_id, 
			const DeltaValue&	_set,
			const DeltaValue&	_get
		);
	};

	///////////////////////////////////////////////////////
	// We maintain separate lists and hash-tables, for each different
	// index object type, to speed-up run-time table editing.
	//

	protected:

	Element*					numberIndex			[DELTA_TABLE_NUMBER_HASH_SIZE];
	Element*					boolIndex			[2];
	Element*					strIndex			[DELTA_TABLE_STRING_HASH_SIZE];
	Element*					tableIndex			[DELTA_TABLE_COMMON_HASH_SIZE];
	Element*					libraryFuncIndex	[DELTA_TABLE_COMMON_HASH_SIZE];
	Element*					programFuncIndex	[DELTA_TABLE_COMMON_HASH_SIZE];
	Element*					externIdIndex		[DELTA_TABLE_COMMON_HASH_SIZE];
	Element*					methodFuncIndex		[DELTA_TABLE_COMMON_HASH_SIZE];
	
	util_ui32					total;
	Element**					tables[DELTA_TABLE_TOTAL_HASHTABLES];
	util_ui32					tableSizes[DELTA_TABLE_TOTAL_HASHTABLES];
	DeltaCreationInfo			creationInfo;

	///////////////////////////////////////////////////////
	// Delegation.
	std::list<DeltaTable*>		myDelegates;				// those i have delegated
	ubag<DeltaTable*>			myDelegators;				// those that delegated me
	std::list<DeltaTable*>		myInternalDelegates;		// those i have delegated marking them as internal
	DeltaTable*					delegatesArray;				// keeps all direct delegates
	DeltaTable*					delegatorsArray;			// keeps all delegators
	bool						isDelegatesUpToDate;
	bool						isDelegatorsUpToDate;
	mutable bool				searchingInDelegates;		// to avoid endless recursion
	mutable bool				callingIsCallable;			// to avoid endless recursion
	
	void						AddDelegator (DeltaTable* delegator);
	void						AddDelegate (DeltaTable* delegate);
	void						RemoveDelegator (DeltaTable* delegator);
	void						RemoveDelegate (DeltaTable* delegate);

	///////////////////////////////////////////////////////
	// We need four basic hashing schemes.
	//
	util_ui16 					Hash (const char* s, util_ui16 size);
	util_ui16 					Hash (void* p, util_ui16 size);
	util_ui16 					Hash (DeltaNumberValueType num, util_ui16 size);
	util_ui16 					Hash (DeltaCodeAddress addr, util_ui16 size);
	
	Element**					GetBucket (
									const DeltaValue&	index,	// index object
									Element***			table	// in which table found
								);

	Element*					GetEntryStringIndex (
									const std::string&	index,		// at which index
									Element*			bucket		// from which bucket
								);


	Element*					GetEntry (
									const DeltaValue&	index,		// at which index
									Element*			bucket		// from which bucket
								);

	Element*					GetEntry (
									const DeltaValue&	index,		// at which index
									Element*			bucket,		// from which bucket
									Element**			before		// if  before element needed
								);

	void						ClearBucket (Element* bucket);	

	Element*					SelectIterationBucket (util_ui32 tableNo, util_ui32 bucketNo) 
									{ return tables[tableNo][bucketNo]; }

	const Element*				SelectIterationBucket (util_ui32 tableNo, util_ui32 bucketNo) const
									{ return tables[tableNo][bucketNo]; }

	static bool 				GetBucket_Number			(GETBUCKET_ARGS);
	static bool 				GetBucket_String			(GETBUCKET_ARGS);
	static bool 				GetBucket_Bool				(GETBUCKET_ARGS);
	static bool 				GetBucket_ProgramFuncAddress	(GETBUCKET_ARGS);
	static bool 				GetBucket_LibraryFuncName	(GETBUCKET_ARGS);
	static bool 				GetBucket_Table				(GETBUCKET_ARGS);
	static bool 				GetBucket_ExternId			(GETBUCKET_ARGS);
	static bool 				GetBucket_Error				(GETBUCKET_ARGS);
	static bool 				GetBucket_MethodFuncAddress (GETBUCKET_ARGS);

	typedef bool				(*GetBucket_Func) (
									DeltaTable*			p,
									const DeltaValue&	index, 
									Element***			table
								);
	GetBucket_Func				getBucketFuncs[TOTAL_GET_BUCKET_FUNCS];

	static bool					IsLegalIndex (const DeltaValue& index)
									{ return index.Type() != DeltaValue_Nil && index.Type() != DeltaValue_Undefined; }
	bool						CheckLegalIndex (const DeltaValue& index) const;
	void						ChangeSelfIfMethodAndCallerIsSelf (DeltaValue& obj, DeltaTable* newSelf) const;

	///////////////////////////////////////////////////////

	typedef utriple<
				Element*,	// ENTRY
				Element*,	// BEFORE
				Element**	// TABLE
			> FindContext;

	Element*					Find (const DeltaValue& index);
	bool						Find (
									const DeltaValue&	index,
									FindContext*		context
								);

	util_ui32					CountBucket (Element* bucket) const;	
	util_ui32					CountAll (void) const;

	bool						Copy (const DeltaTable* from, const Element& elem);	// Copies (new element) 'elem' of 'from'.
	Element*					GetElement (const std::string& id);
	Element*					SetNewElement (const DeltaValue& index, const DeltaValue& content);

	
	///////////////////////////////////////////////////////
	// Optimised setters per case. Return false in case of error.

	bool						BasicSetNew												(const DeltaValue& index, const DeltaValue& content);
	bool						BasicSetWithInternalDelegates							(const DeltaValue& index, const DeltaValue& content);
	bool						BasicSetNoInternalDelegates								(const DeltaValue& index, const DeltaValue& content);
	bool						SetWithDelegates										(const DeltaValue& index, const DeltaValue& content);
	bool						SetAttributeNoInternalDelegates							(const DeltaValue& id, const DeltaValue& val);
	bool						SetAttributeWithInternalDelegates						(const DeltaValue& id, const DeltaValue& val);

	bool						(DeltaTable::*currAttrSetFunc)							(const DeltaValue& index, const DeltaValue& content);
	bool						(DeltaTable::*currSetFunc)								(const DeltaValue& index, const DeltaValue& content);
	bool						(DeltaTable::*currSetNewFunc)							(const DeltaValue& index, const DeltaValue& content);
	bool						(DeltaTable::*currBasicSetFunc)							(const DeltaValue& index, const DeltaValue& content);
	bool						(DeltaTable::*currBoundedSetFunc)						(const DeltaValue& index, const DeltaValue& content);

	bool						SetNew (const DeltaValue& index, const DeltaValue& content) 
									{ return (this->*currSetNewFunc)(index, content); }
	void						Remove (FindContext& context);

	///////////////////////////////////////////////////////
	// Optimised getters per case. Return false in case of error.

	bool						BasicGetWithInternalDelegates							(const DeltaValue& index, DeltaValue* content);
	bool						BasicGetNoInternalDelegates								(const DeltaValue& index, DeltaValue* content);
	bool						BasicGetWithInternalDelegatesBySkippingUserCode			(const DeltaValue& index, DeltaValue* content);
	bool						BasicGetNoInternalDelegatesBySkippingUserCode			(const DeltaValue& index, DeltaValue* content);
	bool						BasicGetWithInternalDelegatesByUnoverloadedAssign		(const DeltaValue& index, DeltaValue* content);
	bool						BasicGetNoInternalDelegatesByUnoverloadedAssign			(const DeltaValue& index, DeltaValue* content);
	bool						GetWithDelegates										(const DeltaValue& index, DeltaValue* content);
	bool						GetWithDelegatesBySkippingUserCode						(const DeltaValue& index, DeltaValue* content);
	bool						GetWithDelegatesByUnoverloadedAssign					(const DeltaValue& index, DeltaValue* content);

	bool						GetAttributeNoInternalDelegates							(const DeltaValue& id, DeltaValue* val);
	bool						GetAttributeNoInternalDelegatesByUnoverloadedAssign		(const DeltaValue& id, DeltaValue* val);
	bool						GetAttributeWithInternalDelegates						(const DeltaValue& id, DeltaValue* val);
	bool						GetAttributeWithInternalDelegatesByUnoverloadedAssign	(const DeltaValue& id, DeltaValue* val);

	bool						(DeltaTable::*currGetFunc)								(const DeltaValue& index, DeltaValue* content);
	bool						(DeltaTable::*currGetByUnoverloadedAssignFunc)			(const DeltaValue& index, DeltaValue* content);
	bool						(DeltaTable::*currGetBySkippingUserCodeFunc)			(const DeltaValue& index, DeltaValue* content);
	bool						(DeltaTable::*currBoundedGetFunc)						(const DeltaValue& index, DeltaValue* content);
	bool						(DeltaTable::*currBoundedGetByUnoverloadedAssignFunc)	(const DeltaValue& index, DeltaValue* content);
	bool						(DeltaTable::*currBasicGetFunc)							(const DeltaValue& index, DeltaValue* content);
	bool						(DeltaTable::*currBasicGetBySkippingUserCodeFunc)		(const DeltaValue& index, DeltaValue* content);
	bool						(DeltaTable::*currBasicGetByUnoverloadedAssignFunc)		(const DeltaValue& index, DeltaValue* content);
	bool						(DeltaTable::*currAttrGetFunc)							(const DeltaValue& index, DeltaValue* content);
	bool						(DeltaTable::*currAttrGetByUnoverloadedAssignFunc)		(const DeltaValue& index, DeltaValue* content);

	///////////////////////////////////////////////////////
	// Change functions return true when element found and changed. 
	// In case of error the exception of uerrorclass is thrown.

	bool						BasicChange												(const DeltaValue& index, const DeltaValue& content);
	bool						BasicChangeBySkippingUserCode							(const DeltaValue& index, const DeltaValue& content);
	bool						OverloadedChange										(const DeltaValue& index, const DeltaValue& content);
	bool						ChangeWithDelegates										(const DeltaValue& index, const DeltaValue& content);
	bool						ChangeWithDelegatesBySkippingUserCode					(const DeltaValue& index, const DeltaValue& content);
	virtual bool				ChangeWithSubobjects									(const DeltaValue& index, const DeltaValue& content);
	virtual bool				ChangeWithSubobjectsBySkippingUserCode					(const DeltaValue& index, const DeltaValue& content);

	bool						(DeltaTable::*currChangeFunc)							(const DeltaValue& index, const DeltaValue& content);
	bool						(DeltaTable::*currChangeBySkippingUserCodeFunc)			(const DeltaValue& index, const DeltaValue& content);

	///////////////////////////////////////////////////////
	// Find functions return true when element found and tken. 
	// In case of error the exception of uerrorclass is thrown.

	bool						BasicFind												(const DeltaValue& index, DeltaValue* content, bool* inDelegates);
	bool						BasicFindBySkippingUserCode								(const DeltaValue& index, DeltaValue* content, bool* inDelegates);
	bool						OverloadedFind											(const DeltaValue& index, DeltaValue* content, bool* inDelegates);
	bool						FindWithDelegates										(const DeltaValue& index, DeltaValue* content, bool* inDelegates);
	bool						FindWithDelegatesBySkippingUserCode						(const DeltaValue& index, DeltaValue* content, bool* inDelegates);
	virtual bool				FindWithSubobjects										(const DeltaValue& index, DeltaValue* content, bool* inDelegates);
	virtual bool				FindWithSubobjectsBySkippingUserCode					(const DeltaValue& index, DeltaValue* content, bool* inDelegates);

	bool						(DeltaTable::*currFindFunc)								(const DeltaValue& index, DeltaValue* content, bool* inDelegates);
	bool						(DeltaTable::*currFindBySkippingUserCodeFunc)			(const DeltaValue& index, DeltaValue* content, bool* inDelegates);

	///////////////////////////////////////////////////////

	struct LookupErrorTraits {
		static void				GetFailed (DeltaTable* t, const std::string& context);
		static void				SetFailed (DeltaTable* t, const std::string& context);
		static void				RemoveNonExisting (DeltaTable* t);
		static void				NewFailed (DeltaTable* t);
	};

	struct SkipLookupErrorTraits {
		static void				GetFailed (DeltaTable* t, const std::string& context);
		static void				SetFailed (DeltaTable* t, const std::string& context);
		static void				RemoveNonExisting (DeltaTable* t);
		static void				NewFailed (DeltaTable* t);
	};

	///////////////////////////////////////////////////////

	template <
		bool (*SetRecursiveFunc)(DeltaTable*, const DeltaValue&, const DeltaValue&),
		bool (*SetNewFunc)(DeltaTable*, const DeltaValue&, const DeltaValue&),
		class DelegatesAccessTraits,
		class ErrorTraits,
		const bool AddWhenNotFound,
		const bool IncludeCaller
	>
	 bool SetInDelegatesTemplate (const DeltaValue&	index, const DeltaValue& content) {
		
		std::list<DeltaTable*> currList, nextList;
		ulistcopy(DelegatesAccessTraits::Get(this), currList);
		if (IncludeCaller)
			currList.push_front(this);	// Push caller in front to avoid separate checks.

		while (true) {	// BFS traversal.

			DASSERT(currList.size());

			for (std::list<DeltaTable*>::iterator i = currList.begin(); i != currList.end(); ++i) {

				try 
					{ if ((*SetRecursiveFunc)(*i, index, content)) return true; }
				catch (uerrorclass::udisengageexception) 
					{ ErrorTraits::SetFailed(*i, "delegates"); }
				
				if (IncludeCaller && this == *i)
					continue;
				else {
					DASSERT(this != *i);
					if (DelegatesAccessTraits::Has(DPTR(*i)))
						ulistcopy(DelegatesAccessTraits::Get(DPTR(*i)), nextList);
				}
			}

			if (nextList.empty())
				break;

			currList.clear(); 
			ulistcopy(nextList, currList); 
			nextList.clear();
		}
		
		// Fallback means found nowhere thus we may add a new element.
		// If content is nil can't remove since not found.

		if (content.IsNil())
			ErrorTraits::RemoveNonExisting(this);
		else
		if (AddWhenNotFound)
			if (!(*SetNewFunc)(this, index, content))
				ErrorTraits::NewFailed(this);
			else
				return true;
		return false;
	}

	///////////////////////////////////////////////////////

	template <
		bool (*GetRecursive)(DeltaTable*, const DeltaValue&, DeltaValue*, bool*),
		class DelegatesAccessTraits,
		class ErrorTraits,
		const bool IncludeCaller
	>
	 bool GetInDelegatesTemplate (const DeltaValue&	index, DeltaValue* content, bool* inDelegates) {
		
		std::list<DeltaTable*> currList, nextList;
		ulistcopy(DelegatesAccessTraits::Get(this), currList);
		if (IncludeCaller)
			currList.push_front(this);	// Push caller in front to avoid separate checks.

		while (true) {	// BFS traversal.

			DASSERT(currList.size());

			for (std::list<DeltaTable*>::iterator i = currList.begin(); i != currList.end(); ++i) {

				try {
					if ((*GetRecursive)(*i, index, content, inDelegates))
						return *inDelegates = true;
				}
				catch (uerrorclass::udisengageexception) 
					{ ErrorTraits::GetFailed(*i, "delegates"); }
				
				if (IncludeCaller && *i == this)
					continue;
				else {
					DASSERT(this != *i);
					if (DelegatesAccessTraits::Has(DPTR(*i)))
						ulistcopy(DelegatesAccessTraits::Get(DPTR(*i)), nextList);
				}
			}

			if (nextList.empty())
				break;

			currList.clear(); 
			ulistcopy(nextList, currList); 
			nextList.clear();
		}
		
		// Fallback means not found.
		return false;
	}

	///////////////////////////////////////////////////////
	// Overloading.
	//

	DeltaValue					selfValue;				// The table as an object.
	DeltaValue					delegatesValue;			// Delegates table value.
	DeltaValue					delegatorsValue;		// Delegators table value.

	bool						overloadingEnabled;
	bool						overloadsGet;
	bool						overloadsSet;
	DeltaValue					get;
	DeltaValue					set;
	DeltaValue					functorIndex;			// Cached index.

	void						CheckOverloadedOperatorAdded (const DeltaValue& index, const DeltaValue& content);
	void						CheckOverloadedOperatorRemoved (const DeltaValue& index);
	bool						ChangeSelfWhenDelegateMethod (DeltaValue* content);

	bool						OverloadedSet (const DeltaValue& index, const DeltaValue& content);
	bool						OverloadedGet (
									const DeltaValue&	index, 
									DeltaValue*			content, 
									bool*				inDelegate = (bool*) 0
								);
	bool						OverloadedBoundedGetByUnoverloadedAssign(const DeltaValue&	index, DeltaValue* content);
	bool						OverloadedBoundedGet(const DeltaValue&	index, DeltaValue* content);
	void						InstallOverloadedBoundedGetFunctions (void);
	void						UninstallOverloadedBoundedGetFunctions (void);

	void						InstallOverloadedBoundedSetFunctions (void);
	void						UninstallOverloadedBoundedSetFunctions (void);

	mutable bool				cycleCheck;
	virtual bool				HasLookupCycle (void) const;

	//************************
	// Garbage collection behaviors.

	virtual bool				IsAltReferencable (void) const;
	virtual void				AltPickAndSetAsDyingAllAliveReferrers (ContList& garbage);
	virtual void				ExplicitDestructor (void);
	virtual void				DeleteMemory (void);

	//************************
	// To string conversion handler.

	void						ConvertToStringHandler (
									DeltaString* at, 
									void (DeltaValue::*tostring) (DeltaString* at) const,
									bool (Element::*getcontent)(DeltaValue* at) const,
									void* closure
								);

	//--------------------------------------------------------------

	public:

	///////////////////////////////////////////////////////
	// For iteration through elements. If element addition / removal takes place
	// while iterating, the iteration gets invalidated. Iterations
	// cannot be nested.
	//
	template <class C> 
	class basic_iterator :	public	std::iterator<
										std::input_iterator_tag, 
										typename C::elem_type
									>,
							public	usafeiteratorbehavior {

		friend class DeltaTable;

		protected:
		util_ui32							currTableNo;
		typename C::return_elem_type_ptr	currBucket;
		util_ui32							currBucketNo;
		typename C::table_ptr				table;
		util_ui32							tableSerialNo;
		

		virtual usafecontainerbehavior*	
								get_container (void)
									{ return table; }
		virtual const void*		get_value_address (void)
									{ return currBucket; }
		virtual void			set_at_end (void)
									{ DPTR(table)->set_at_end(this); }
		virtual bool			is_at_end (void) const
									{ DASSERT(table); return DPTR(table)->is_at_end(this); }
		bool					isvalid (void) const 
									{ return tableSerialNo && ValidatableHandler::Get(tableSerialNo); }

		const DeltaTable*		gettable (void) const { return table; }

		void					settable (DeltaTable* t) {
									DASSERT(t);
									if (table != t) {
										DASSERT(!table);	// Only initially set via this call.
										table = t;
										tableSerialNo = DPTR(table)->GetSerialNo();
										on_after_set_container();
									}
								}

		virtual void fwd (void) {

			DASSERT(currTableNo < DELTA_TABLE_TOTAL_HASHTABLES);

			if (should_ignore_next_fwd())	// Already moved forward.
				reset_ignore_next_fwd();	// We can ingore only one fwd call.
			else {

				// If we finished doing all buckets of a hash table,
				// we proceed with next hash table. If all tables are done,
				// then iteration is completed (reached end).

				if (currBucket)
					currBucket = DPTR(currBucket)->GetNext();

				while (!currBucket) {
					if (currBucketNo == DPTR(table)->tableSizes[currTableNo] - 1)
						if (++currTableNo == DELTA_TABLE_TOTAL_HASHTABLES) {
							unullify(currBucket);
							currBucketNo = 0;
							return;
						}
						else
							currBucketNo = 0;
					else
						++currBucketNo;
					currBucket =	DPTR(table)->SelectIterationBucket(
										currTableNo, 
										currBucketNo
									);
				}
			}
		}


		////////////////////////////////////////////////////////////////

		public:
		bool								operator==(const basic_iterator& i) const 
												{ return table == i.table && currBucket == i.currBucket; }
		bool								operator!=(const basic_iterator& i) const 
												{ return !operator==(i); }
		typename C::return_elem_type_ref	operator*(void) const
												{ DASSERT(isvalid() && currBucket); return *DPTR(currBucket); }
		typename C::return_elem_type_ptr	operator->(void) const 
												{DASSERT(isvalid() && currBucket);  return currBucket; }
		const basic_iterator				operator++(int) 
												{DASSERT(isvalid()); basic_iterator i(*this); fwd(); return i; }
		const basic_iterator				operator++(void) 
												{ DASSERT(isvalid()); fwd(); return *this; }
		const basic_iterator				operator--(int) // Unsupported.
												{ DASSERT(isvalid()); return *this; }
		const basic_iterator				operator--(void) // Unsupported.
												{ DASSERT(isvalid()); return *this; }
		const basic_iterator&				operator=(const basic_iterator& i) 
												{ clear(); return *new(this) basic_iterator(i); }

		basic_iterator (const basic_iterator& i):
			usafeiteratorbehavior(i),
			currTableNo(i.currTableNo),
			currBucket(i.currBucket),
			currBucketNo(i.currBucketNo),
			table(i.table),
			tableSerialNo(i.tableSerialNo)
				{ if (table) on_after_set_container(); }

		basic_iterator (
			util_ui32							tableNo,
			typename C::return_elem_type_ptr	bucket,
			util_ui32							bucketNo,
			typename C::table_ptr				t
		) :	currTableNo(tableNo),
			currBucket(bucket),
			currBucketNo(bucketNo),
			table(t),
			tableSerialNo(t->GetSerialNo())
				{ if (table) on_after_set_container(); }

		basic_iterator (void):
			currTableNo(0),
			currBucket((Element*) 0),
			currBucketNo(0),
			table((DeltaTable*) 0),
			tableSerialNo(0)
				{}

		virtual ~basic_iterator() 
				{ clear(); }
	};

	///////////////////////////////////////////////////////////////

	struct iterator_types {
		typedef DeltaTable::Element*		elem_type;
		typedef DeltaTable::Element&		return_elem_type_ref;
		typedef const DeltaTable::Element*	return_elem_const_type_ptr;
		typedef DeltaTable::Element*		return_elem_type_ptr;
		typedef DeltaTable*					table_ptr;
	};
	
	class iterator : public basic_iterator<iterator_types> {
		public:
		iterator (const iterator& i): basic_iterator<iterator_types>(i){}
		iterator (void){}
	};

	///////////////////////////////////////////////////////////////

	struct const_iterator_types {
		typedef const DeltaTable::Element*		elem_type;
		typedef const DeltaTable::Element&		return_elem_type_ref;
		typedef const DeltaTable::Element*		return_elem_type_ptr;
		typedef return_elem_type_ptr			return_elem_const_type_ptr;
		typedef	DeltaTable*						table_ptr;
	};

	class const_iterator : public basic_iterator<const_iterator_types> {
		public:
		UOVERLOADED_ASSIGN_VIA_CONVERTER_CONSTRUCTOR(const_iterator, DeltaTable::iterator)
		bool operator==(const  DeltaTable::iterator& i) const
			{ return table == i.table && currBucket == i.currBucket; }
		bool operator!=(const  DeltaTable::iterator& i) const
			{ return table != i.table || currBucket != i.currBucket; }
		const_iterator (const DeltaTable::iterator& i) :
			basic_iterator<const_iterator_types>(i.currTableNo, i.currBucket, i.currBucketNo, i.table){}
		const_iterator (const const_iterator& i) : basic_iterator<const_iterator_types>(i){}
		const_iterator (void){}
	};

	///////////////////////////////////////////////////////////////

	const iterator				const_begin (void) const;
	const iterator				const_end (void) const;
	const iterator				begin (void)		{ return const_begin(); }
	const iterator				end (void)			{ return const_end(); }
	const iterator				begin (void) const	{ return const_begin(); }
	const iterator				end (void) const	{ return const_end(); }

	template <class iterator_traits>
	void						set_at_end (basic_iterator<iterator_traits>* i) const {
									i->settable(const_cast<DeltaTable*>(this));
									i->currTableNo	= DELTA_TABLE_TOTAL_HASHTABLES;	
									unullify(i->currBucket);
								}

	template <class iterator_traits>
	bool						is_at_end (const basic_iterator<iterator_traits>* i) const {
									return	i->gettable() == this && 
											i->currTableNo == DELTA_TABLE_TOTAL_HASHTABLES; 
								}

	///////////////////////////////////////////////////////////////
	// Copy() makes an exact one-level copy (i.e. not recursive for table elements)
	// of the caller, not changing the owner table of member functions. Extend()
	// adds only elements not found in the caller, while setting the caller as the
	// owner of every method found in 'from'.
	//
	DeltaTable*					Copy (void) const;					// Template method, uses virtual New().
	void						Extend (const DeltaTable* from);	// Template method, uses virtual New().
	static DeltaTable*			NewTable (void);					// Table construction explicitly.
	virtual DeltaTable*			New (void) const;					// Virtual factory method (can be refined, to return subclass instance).
	virtual void				Clear (void);						// Removes all elements.
	util_ui32					Total(void) const
									{ return total; }
	DeltaTable*					GetIndices (void) const;			// Produces a new table with every call.

	//*****************************
	// Sets 'to' as an array of successive indices for all elements found in 'from'.

	template <class T>
	static void					MakeArray (
									DeltaTable*&	to, 
									const T&		from, 
									DeltaTable*		(*f)(typename T::const_iterator&)
								) {
									DASSERT(to);
									DPTR(to)->Clear();
									DeltaValue index, content;
									util_ui32	j = 0;

									for (typename T::const_iterator i = from.begin(); i != from.end(); ++i, ++j) {
										index.FromNumber(j);
										content.FromTable((*f)(i));
										DPTR(to)->Set(index, content);
									}
								}

	//*****************************
	// Sets or creates 'to' via MakeArray using 'from'.

	template <class T>
	static DeltaTable*			GetArray (
									bool&			flag, 
									DeltaTable*&	to, 
									T&				from, 
									DeltaTable*		(*f)(typename T::const_iterator&)
								) {
									if (!flag) {
										DPTR(to)->Clear();
										MakeArray(to, from, f);
										flag = true;
									}
									DASSERT(to);
									return to;							
								}

	//************************

	bool						IsOverloadingEnabled (void) const
									{ return overloadingEnabled; }
	void						EnableOverloading (void);
	void						DisableOverloading (void);

	bool						OverloadsGet (void) const 
									{ return overloadingEnabled && overloadsGet; }
	bool						OverloadsSet (void) const 
									{ return overloadingEnabled && overloadsSet; }

	//************************
	// Primary lookup (set / get) functions.

	virtual	bool				Get (const DeltaValue& index, DeltaValue* content)
									{ return (this->*currGetFunc)(index, content); }
	virtual bool				GetBySkippingUserCode (const DeltaValue& index, DeltaValue* content)
									{ return (this->*currGetBySkippingUserCodeFunc)(index, content); }
	virtual bool				GetByUnoverloadedAssign (const DeltaValue& index, DeltaValue* content)
									{ return (this->*currGetByUnoverloadedAssignFunc)(index, content); }

	bool						BasicGet (const DeltaValue& index, DeltaValue* content)
									{ return (this->*currBasicGetFunc)(index, content); }
	bool						BasicGetBySkippingUserCode (const DeltaValue& index, DeltaValue* content)
									{ return (this->*currBasicGetBySkippingUserCodeFunc)(index, content); }
	bool						BasicGetByUnoverloadedAssign (const DeltaValue& index, DeltaValue* content)
									{ return (this->*currBasicGetByUnoverloadedAssignFunc)(index, content); }
	bool						BoundedGet (const DeltaValue& index, DeltaValue* content)
									{ return (this->*currBoundedGetFunc)(index, content); }
	bool						BoundedGetByUnoverloadedAssign (const DeltaValue& index, DeltaValue* content)
									{ return (this->*currBoundedGetByUnoverloadedAssignFunc)(index, content); }

	virtual bool				Set (const DeltaValue& index, const DeltaValue& content)
									{ return (this->*currSetFunc)(index, content); }
	virtual bool				SetBySkippingUserCode (const DeltaValue& index, const DeltaValue& content);
	bool						BasicSet (const DeltaValue& index, const DeltaValue& content)
									{ return (this->*currBasicSetFunc)(index, content); }
	bool						BoundedSet (const DeltaValue& index, const DeltaValue& content)
									{ return (this->*currBoundedSetFunc)(index, content); }

	bool						Finder (const DeltaValue& index, DeltaValue* content, bool* inDelegates)
									{ return (this->*currFindFunc)(index, content, inDelegates); }
	bool						FinderBySkippingUserCode (const DeltaValue& index, DeltaValue* content, bool* inDelegates)
									{ return (this->*currFindBySkippingUserCodeFunc)(index, content, inDelegates); }

	bool						Change (const DeltaValue& index, const DeltaValue& content)
									{ return (this->*currChangeFunc)(index, content); }
	bool						ChangeBySkippingUserCode (const DeltaValue& index, const DeltaValue& content)
									{ return (this->*currChangeBySkippingUserCodeFunc)(index, content); }

	bool						Remove (const DeltaValue& index);	// Equivalent to Set(index, Nil content), but locally.

	//************************
	// Attributes. Set / Get versions *always* local (no inheritance / delegation apply).
	// The reason is that in inheritance / delegation we may have attributes for
	// the inquired key in a tree / graph of objects, however, none of those may be 
	// returned by the lookup algorithm. Overall, it is meaningless to qualify that an
	// attribute is needed, since the lookup semantics do not involve the slot type.

	Attribute*					NewAttribute (
									const std::string&	id,
									const DeltaValue&	setter,
									const DeltaValue&	getter
								);

	Attribute*					GetAttribute (const std::string& id);	// Returns only attributes else (field nor no entry) null.
	const Attribute*			GetAttribute (const std::string& id) const;

	bool						GetAttribute (const std::string& id, DeltaValue* content)
									{ return (this->*currAttrGetFunc)(DeltaValue(id), content); }

	bool						GetAttributeByUnoverloadedAssign (const std::string& id, DeltaValue* content)
									{ return (this->*currAttrGetByUnoverloadedAssignFunc)(DeltaValue(id), content); }

	bool						HasAttribute (const std::string& id) const;	// Returns true if either an existing attribute or a field.

	bool						SetAttribute (const std::string& id, const DeltaValue& content)	// If not an existing attribute it makes a new field.
									{ return (this->*currAttrSetFunc)(DeltaValue(id), content); }

	//************************
	// Delegation.

	bool						DelegateInternal (DeltaTable* obj);
	bool						UndelegateInternal (DeltaTable* obj);
	bool						IsInternalDelegate (const DeltaTable* obj) const;
	bool						HasInternalDelegates (void) const
									{ return !myInternalDelegates.empty(); }

	virtual bool				Delegate (DeltaTable* obj);
	virtual bool				Undelegate (DeltaTable* obj);
	bool						IsDelegate (DeltaTable* obj) const;
	bool						IsDirectDelegate (DeltaTable* obj) const;
	bool						IsDelegator (DeltaTable* obj) const;

	bool						HasDelegates (void) const
									{ return !myDelegates.empty(); }
	bool						HasDelegators (void) const
									{ return !myDelegators.empty(); }

	const std::list<DeltaTable*>&
								GetDelegates (void) const { return myDelegates; }
	const ubag<DeltaTable*>&	GetDelegators (void) const { return myDelegators; }

	DeltaTable*					GetDelegatesAsTable (void);
	DeltaTable*					GetDelegatorsAsTable (void);
	
	//************************
	// Dynamic function-call overloading.

	bool						GetFunctor (DeltaValue* at)
									{ return Get(functorIndex, at) && at->Type() != DeltaValue_Nil; }
	bool						IsCallable (void) const;

	virtual void				ConvertToStringBySkippingUserCode (DeltaString* at, void* closure = (void*) 0);
	virtual void				ConvertToString (DeltaString* at, void* closure = (void*) 0);
	virtual void				ToValue (DeltaValue* at) 
									{ at->FromTable(this); }
	static const std::pair<const char**, util_ui32>
								GetOverloabableOperators (void);
	static bool					IsOverloadableOperator (const std::string& op);

	const DeltaCreationInfo&	GetCreationInfo (void) const 
									{ return creationInfo; }
	DeltaCreationInfo&			GetCreationInfo (void) 
									{ return creationInfo; }

	//**************************
	// Virtual functions required for Validatable base class.
	//
	bool						IsOfClass (const std::string& classId ) const
									{ return classId == GetExtClassString(); }

	virtual const char*			GetExtClassString (void) const 
									{ return DELTA_TABLE_CLASS_ID; }

	DYNAMIC_CAST_SINGLEBASE_INLINE_IMPL(CollectableContainer, DELTA_TABLE_CLASS_ID)

	// We never create instances directly, but always through the factory. 

	DeltaTable (void);
	virtual ~DeltaTable();
};

//-------------------------------------------------------------

#endif	// Do not add stuff beyond this point.
