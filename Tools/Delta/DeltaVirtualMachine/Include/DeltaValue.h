// DeltaValue.h
// Virtual Machine data types.
// Delta Virtual Machine.
// ScriptFighter Project.
// A. Savidis, Original version: October 1999.
// Last update to include residency information: January 2005.
//

#ifndef	DELTAVALUE_H
#define	DELTAVALUE_H

#include "DeltaByteCodeTypes.h"
#include "DeltaCollectableContainer.h"
#include "DeltaExternId.h"
#include "ucrosscast.h"
#include "DeltaRuntimeError.h"
#include "DeltaString.h"
#include "DeltaValueTypes.h"

////////////////////////////////////////////////////////////////////

enum DeltaValueType {
	DeltaValue_Number			= 0,
	DeltaValue_String			= 1,
	DeltaValue_Table			= 2,
	DeltaValue_ProgramFunc		= 3,
	DeltaValue_LibraryFunc		= 4,
	DeltaValue_Bool				= 5,
	DeltaValue_ExternId			= 6,
	DeltaValue_Nil				= 7,
	DeltaValue_MethodFunc		= 8,
	DeltaValue_Undefined		= 9
};

#define	TOTAL_DELTAVALUE_TYPES	10

////////////////////////////////////////////////////////////////////
// The basic data type of an object in the Delta memory
// stack at run-time. Emphasis on simplicity and compactness.
//
class DVM_CLASS DeltaValue {
	private:
	#include "DeltaValueP.h"

	public:
	enum _Nil { Nil = 0 };

	////////////////////////////////////////////////////////////////////
	// A virtual base class for classes whose values should convert 
	// to delta objects as extern ids and be validated.

	class Convertible {
		public:
		virtual void ConvertToDelta (DeltaValue* at) const = 0;
	};

	#define	ISA_DELTACONVERTIBLE									\
				public virtual Validatable,							\
				public virtual DeltaValue::Convertible

	#define	DELTACONVERTIBLE_PUBLIC_CLASSDEFS_NOCASTING(_typestr)	\
		typedef void	(*DeltaConverter)(							\
							DeltaValue*			at,					\
							const std::string&	classId,			\
							util_ui32			serialNo			\
						);											\
		virtual void	ConvertToDelta (DeltaValue* at) const {		\
							DASSERT(deltaConverter);				\
							(*deltaConverter)(						\
								at,									\
								GetExtClassString(),				\
								GetSerialNo()						\
							);										\
						}											\
		virtual const char*	GetExtClassString (void) const			\
							{ return _typestr; }					\
		static void		SetDeltaConverter (DeltaConverter f)		\
							{ deltaConverter = f; }					\

	#define	DELTACONVERTIBLE_PUBLIC_CLASSDEFS(_typestr)				\
		DELTACONVERTIBLE_PUBLIC_CLASSDEFS_NOCASTING(_typestr)		\
		DYNAMIC_CAST_NOBASE_INLINE_IMPL(_typestr)

	#define	DELTACONVERTIBLE_PRIVATEPROTECTED_CLASSDEFS				\
		static DeltaConverter deltaConverter;

	#define	DELTACONVERTIBLE_IMPLDEFS(_base)						\
		_base::DeltaConverter										\
		_base::deltaConverter = (DeltaConverter) 0;

	#define	DELTACONVERTIBLE_SETCONVERTER(_base, _f)				\
		_base::SetDeltaConverter(_f)

	////////////////////////////////////////////////////////////////////
	// Where a delta object actually inhabits. 
	// Needed by the garbage collection algorithm.
	
	enum ResidencyType {
		Internal	=	0,		// In C++ code.
		Stacked		=	1,		// In VM stack.
		Contained	=	2		// In a collectable container subclass.
	};

	//-------------------------------------------------------------

	private:
	DeltaValueType		type;
	ResidencyType		residencyType;		// Default is 'Internal'.
	void*				residencyContext;	// Type cast to context instance.
	std::string			typeTag;			// User defined tag.
	static util_ui32	toStringMaxLen;		// Only when user code is skipped.

	union {
		DeltaNumberValueType	numObj;
		DeltaString*			strObj;
		bool					boolObj;
		DeltaTable*				tableObj;
		DeltaExternId*			externIdObj;

		struct {
			DeltaLibraryFuncArgsBinder*	binder;
			DeltaLibraryFunc			f;
		} libraryFuncObj;

		struct {
			const DeltaStdFuncInfo*	func;		// Funciton info.
			DeltaVirtualMachine*	vm;			// The owner virtual machine.
			DeltaClosure*			closure;	// Optional closure.
		} programFuncObj;

		struct {
			const DeltaStdFuncInfo*	func;		// Funciton info.
			DeltaTable*				self;		// The self object.
			DeltaVirtualMachine*	vm;			// The owner virtual machine.
			DeltaClosure*			closure;	// Optional closure.
		} methodFuncObj;

	} val;

	DeltaTable*				GetTable (void);			// Requires table / method.
	CollectableContainer*	GetContainer (void);		// Requires table / method / collectable container externid.

	///////////////////////////////////////////////////////////////
	// Assignment optimization functions.
	//
	static void AssignFrom_Number		(DeltaValue* dest, const DeltaValue* src);				
	static void AssignFrom_String		(DeltaValue* dest, const DeltaValue* src);				
	static void AssignFrom_Table		(DeltaValue* dest, const DeltaValue* src);				
	static void AssignFrom_ProgramFunc	(DeltaValue* dest, const DeltaValue* src);	
	static void AssignFrom_LibraryFunc	(DeltaValue* dest, const DeltaValue* src);	
	static void AssignFrom_Bool			(DeltaValue* dest, const DeltaValue* src);				
	static void AssignFrom_ExternId		(DeltaValue* dest, const DeltaValue* src);			
	static void AssignFrom_Nil			(DeltaValue* dest, const DeltaValue* src);					
	static void AssignFrom_MethodFunc	(DeltaValue* dest, const DeltaValue* src);	
	static void AssignFrom_Undefined	(DeltaValue* dest, const DeltaValue* src);			

	typedef void (*AssignFunc) (DeltaValue*, const DeltaValue*);
	static AssignFunc assignFuncs[TOTAL_DELTAVALUE_TYPES];

	///////////////////////////////////////////////////////////////

	static void Collect_Number			(DeltaValue* val);				
	static void Collect_String			(DeltaValue* val);				
	static void Collect_Table			(DeltaValue* val);				
	static void Collect_ProgramFunc		(DeltaValue* val);	
	static void Collect_LibraryFunc		(DeltaValue* val);	
	static void Collect_Bool			(DeltaValue* val);				
	static void Collect_ExternId		(DeltaValue* val);			
	static void Collect_Nil				(DeltaValue* val);					
	static void Collect_MethodFunc		(DeltaValue* val);	
	static void Collect_Undefined		(DeltaValue* val);	
	
	typedef void (*CollectFunc) (DeltaValue*);
	static CollectFunc		collectFuncs[TOTAL_DELTAVALUE_TYPES];
	void					CollectGarbage (void);

	///////////////////////////////////////////////////////////////

	static bool Number_TrueTest			(const DeltaValue*);
	static bool String_TrueTest			(const DeltaValue*);
	static bool Bool_TrueTest			(const DeltaValue*);
	static bool Table_TrueTest			(const DeltaValue*);
	static bool ProgramFunc_TrueTest	(const DeltaValue*);
	static bool MethodFunc_TrueTest		(const DeltaValue*);
	static bool LibraryFunc_TrueTest	(const DeltaValue*);
	static bool Reference_TrueTest		(const DeltaValue*);
	static bool ExternId_TrueTest		(const DeltaValue*);
	static bool Nil_TrueTest			(const DeltaValue*);
	static bool Undefined_TrueTest		(const DeltaValue* val);

	typedef bool (*TrueTestFunc ) (const DeltaValue*);
	static TrueTestFunc trueTestFuncs[TOTAL_DELTAVALUE_TYPES];

	///////////////////////////////////////////////////////////////
	
	static bool Number_Equality			(DeltaValue*, DeltaValue*);
	static bool String_Equality			(DeltaValue*, DeltaValue*);
	static bool Bool_Equality			(DeltaValue*, DeltaValue*);
	static bool Table_Equality			(DeltaValue*, DeltaValue*);
	static bool ProgramFunc_Equality	(DeltaValue*, DeltaValue*);
	static bool LibraryFunc_Equality	(DeltaValue*, DeltaValue*);
	static bool Reference_Equality		(DeltaValue*, DeltaValue*);
	static bool ExternId_Equality		(DeltaValue*, DeltaValue*);
	static bool Nil_Equality			(DeltaValue*, DeltaValue*);
	static bool MethodFunc_Equality		(DeltaValue* obj1, DeltaValue* obj2);
	static bool Undefined_Equality		(DeltaValue* obj1, DeltaValue* obj2);
	
	typedef bool (*EqualityCheck)(DeltaValue*, DeltaValue*);
	static EqualityCheck equalityCheckFuncs[TOTAL_DELTAVALUE_TYPES];
	
	///////////////////////////////////////////////////////////////

	static void 		ToStringHandler_FromLibraryFunc	(
								DeltaValue*		val, 
								DeltaString*	cvtStr, 
								void (DeltaLibraryFuncArgsBinder::*f)(DeltaString* at, void*)
						);

	static void ToString_FromNumber						(DeltaValue*, DeltaString*);
	static void ToString_FromString						(DeltaValue*, DeltaString*);
	static void ToString_FromTable						(DeltaValue*, DeltaString*);
	static void ToStringNeverOverloaded_FromTable		(DeltaValue* val, DeltaString* at);
	static void ToString_FromProgramFunc				(DeltaValue*, DeltaString*);
	static void ToString_FromLibraryFunc				(DeltaValue* val, DeltaString* cvtStr);
	static void ToStringNeverOverloaded_FromLibraryFunc (DeltaValue* val, DeltaString* cvtStr);
	static void ToString_FromBool						(DeltaValue*, DeltaString*);
	static void ToString_FromReference					(DeltaValue*, DeltaString*);
	static void ToString_FromExternId					(DeltaValue*, DeltaString*);
	static void ToStringNeverOverloaded_FromExternId	(DeltaValue*, DeltaString*);
	static void ToString_FromNil						(DeltaValue*, DeltaString*);
	static void ToString_FromMethodFunc					(DeltaValue* val, DeltaString* cvtStr);
	static void ToString_FromUndefined					(DeltaValue*, DeltaString*);

	typedef void (*ConvertToString_Func) (DeltaValue*, DeltaString*);
	static ConvertToString_Func convertToStringFuncs[TOTAL_DELTAVALUE_TYPES];
	static ConvertToString_Func convertNeverOverloadedToStringFuncs[TOTAL_DELTAVALUE_TYPES];

	///////////////////////////////////////////////////////////////

	const std::string		ConvertToStringHandler (
								void (DeltaValue::*tostring) (DeltaString* at) const,
								util_ui32 maxLen
							) const;

	public:

	//-------------------------------------------------------------
	// Type conversion functions. Convert from Delta types
	// to local data types. Always check type and then
	// use the proper conversion function.
	//
	void					ToNumber (DeltaNumberValueType* num);
	void					ToLibraryFunc (DeltaLibraryFunc* func);
	void					ToBool (bool* boolVal);
	void					ToTable (DeltaTable** table);

	const std::string&		ToString (void) const;
	DeltaNumberValueType	ToNumber (void) const;
	DeltaLibraryFunc		ToLibraryFunc (void) const;
	const DeltaLibraryFuncArgsBinder*
							GetLibraryFuncArgsBinder (void) const;
	DeltaLibraryFuncArgsBinder*
							GetLibraryFuncArgsBinder (void);
	bool					ToBool (void) const;
	DeltaTable*				ToTable (void);
	const DeltaTable*		ToTable (void) const;

	//**********************

	void					ToProgramFunc (
								DeltaCodeAddress*				funcAddr, 
								DeltaVirtualMachine**			vm,
								DeltaClosure**					closure = (DeltaClosure**) 0
							);
	void					ToProgramFunc (
								UPTR(const DeltaStdFuncInfo)*	func, 
								DeltaVirtualMachine**			vm,
								DeltaClosure**					closure = (DeltaClosure**) 0
							);
	DeltaVirtualMachine*	GetProgramFuncVM (void) const;
	DeltaClosure*			GetProgramFuncClosure (void) const;
	const DeltaStdFuncInfo*	GetProgramFunc (void) const;

	//**********************

	void					ToMethodFunc (
								DeltaCodeAddress*				funcAddr, 
								DeltaTable**					table,
								DeltaVirtualMachine**			vm,
								DeltaClosure**					closure = (DeltaClosure**) 0
							);
	void					ToMethodFunc (
								UPTR(const DeltaStdFuncInfo)*	func,
								DeltaTable**					table,
								DeltaVirtualMachine**			vm,
								DeltaClosure**					closure = (DeltaClosure**) 0
							);
	DeltaTable*				GetMethodFuncSelf (void) const;
	DeltaVirtualMachine*	GetMethodFuncVM (void) const;
	DeltaClosure*			GetMethodFuncClosure (void) const;
	const DeltaStdFuncInfo*	GetMethodFunc (void) const;

	//**********************

	util_ui32				ToExternIdSerialNo (void) const;
	void*					ToExternId (std::string& typeStr);
	void*					ToExternId (void);

	const DeltaExternIdFieldGetter*	
							GetExternIdFieldGetter (void) const;
	DeltaTable*				GetExternIdUserData (void);
	const std::string		GetExternIdTypeString (void) const;
	const DeltaExternId*	GetExternId (void) const;
	DeltaExternId*			GetExternId (void);

	bool					ConvertToExternId (			// Returns false in case of error.
								const std::string&	typeStr, 
								DeltaValue*			at	// Set either as an externid (ok) or undefined (error).
							);

	//******************************

	void					FromNumber (DeltaNumberValueType num);
	void					FromString (const std::string& s);
	void					FromBool (bool boolVal);
	void					FromTable (DeltaTable* table);
	void					FromExternId (
								void*							val,
								DeltaExternIdType				type = DeltaExternId_NonCollectable,
								void							(*toString)(DeltaString*, void*) = (void(*)(DeltaString*, void*)) 0,
								const std::string&				typeStr = DELTA_EXTERNID_TYPESTR_UNDEF,
								const DeltaExternIdFieldGetter*	fieldGetter = DELTA_EXTERNID_NO_FIELD_GETTER
							);
	void					FromExternIdDirectly (DeltaExternId* externId);
	void					FromExternIdBySerialNo (util_ui32 serialNo);
	void 					FromLibraryFunc (
								DeltaLibraryFunc			func, 
								DeltaLibraryFuncArgsBinder* binder = (DeltaLibraryFuncArgsBinder*) 0
							);
	void 					FromProgramFunc (
								DeltaCodeAddress		funcAddr, 
								DeltaVirtualMachine*	vm,
								DeltaClosure*			closure = (DeltaClosure*) 0
							);
	void 					FromProgramFunc (
								const DeltaStdFuncInfo*	func, 
								DeltaVirtualMachine*	vm,
								DeltaClosure*			closure = (DeltaClosure*) 0
							);
	void 					FromMethodFunc (
								DeltaCodeAddress		funcAddr, 
								DeltaTable*				self, 
								DeltaVirtualMachine*	vm,
								DeltaClosure*			closure = (DeltaClosure*) 0
							);
	void 					FromMethodFunc (
								const DeltaStdFuncInfo*	func, 
								DeltaTable*				self, 
								DeltaVirtualMachine*	vm,
								DeltaClosure*			closure = (DeltaClosure*) 0
							);
	void					ChangeMethodFuncSelf (DeltaTable* table);
	void					FromNil (void);
	void					Undefine (void);

	//******************************

	#define	NUM_SHORT(v)	((util_i8)			v)
	#define	NUM_BYTE(v)		((util_ui8)			v)
	#define	NUM_INT(v)		((util_i32)			v)
	#define	NUM_WORD(v)		((util_ui32)		v)
	#define	NUM_LONG(v)		((signed long)		v)
	#define	NUM_WIDE(v)		((unsigned long)	v)

	static void				SetMaxLengthForConvertToStringBySkippingUserCode (util_ui32 maxLen = 0);
	static util_ui32		GetMaxLengthForConvertToStringBySkippingUserCode (void);
	const std::string		ConvertToString (void) const;
	const std::string		ConvertToStringBySkippingUserCode (void) const;
	void					ConvertToString (DeltaString* at) const;
	void					ConvertToStringBySkippingUserCode (DeltaString* at) const;

	DeltaValueType			Type (void) const 
								{ return type; }
	const char*				TypeStr (void) const;
	static const char*		GetTypeStr (DeltaValueType type);

	bool					AssignOverloadable (const DeltaValue& src)
								{ return AssignOverloadable(&src); }
	bool					AssignOverloadable (const DeltaValue* val);
	bool					Assign (const DeltaValue* val);
	bool					Assign (const DeltaValue& src)
								{ return Assign(&src); }
	void					operator= (const DeltaValue& val) 
								{ Assign(val); }

	bool					Equal (const DeltaValue& val) const;
	bool					Equal (const DeltaValue* val) const;
	bool					operator==(const DeltaValue& val) const 
								{ return Equal(val); }
	
	bool					TrueTest (void) const;
	operator				bool() const { return TrueTest(); }

	//******************************

	bool 					IsNumber (void) const		{ return type == DeltaValue_Number;			}	
	bool 					IsString (void) const		{ return type == DeltaValue_String;			}	
	bool 					IsTable (void) const		{ return type == DeltaValue_Table;			}	
	bool 					IsProgramFunc (void) const	{ return type == DeltaValue_ProgramFunc;	}
	bool 					IsLibraryFunc (void) const	{ return type == DeltaValue_LibraryFunc;	}
	bool 					IsBool (void) const			{ return type == DeltaValue_Bool;			}		
	bool 					IsExternId (void) const		{ return type == DeltaValue_ExternId;		}	
	bool 					IsMethodFunc (void) const	{ return type == DeltaValue_MethodFunc; 	}
	bool					IsUndefined (void) const	{ return type == DeltaValue_Undefined;		}
	bool					IsNil (void) const			{ return type == DeltaValue_Nil;			}

	//******************************
	// Call as a function. Applies only to function objects.
	// When arguments supplied as list or array they must be in reverse order.
	// For the versions with various arguments they must be in the normal order.
	// They return true if no error occured.

	bool IsCallable (void) const;

	#define	DARG const DeltaValue&
	bool operator()(DeltaValue* result = (DeltaValue*) 0);	// Argumentless
	bool operator()(DARG arg1,									DeltaValue* result = (DeltaValue*) 0);
	bool operator()(DARG arg1, DARG arg2,						DeltaValue* result = (DeltaValue*) 0);
	bool operator()(DARG arg1, DARG arg2, DARG arg3,			DeltaValue* result = (DeltaValue*) 0);
	bool operator()(DARG arg1, DARG arg2, DARG arg3, DARG arg4,	DeltaValue* result = (DeltaValue*) 0);
	#undef	DARG

	bool operator()(const std::list<DeltaValue>& args,			DeltaValue* result = (DeltaValue*) 0);
	bool operator()(const std::list<DeltaValue*>& args,			DeltaValue* result = (DeltaValue*) 0);
	bool operator()(UPTR(const DeltaValue)* args, util_ui16 n,	DeltaValue* result = (DeltaValue*) 0);
	bool operator()(UPTR(const DeltaValue)* nullEndingArgs,		DeltaValue* result = (DeltaValue*) 0);

	struct CallFunctor : public std::unary_function<DeltaValue, void> {
		void operator()(DeltaValue& f) const 
			{ f(); }
	};

	struct CallFunctorEx1 : public std::binary_function<DeltaValue, UPTR(const DeltaValue)*, void> {
		void operator()(DeltaValue& f, UPTR(const DeltaValue)* args) const 
			{ f(args); }
	};

	struct CallFunctorEx2 : public std::binary_function<DeltaValue, std::list<DeltaValue>&, void> {
		void operator()(DeltaValue& f, const std::list<DeltaValue>& args) const 
			{ f(args); }
	};

	/////////////////////////////////////////////
	// Residency does not have copy semantics, i.e. it will
	// not be assigned with copy construction or assignment.
	// It has to be explicitly set and only once.

	ResidencyType		GetResidencyType (void) const;
	void*				GetResidencyContext (void) const;
	void				SetResidencyType (ResidencyType t, void* c = (void*) 0);

	/////////////////////////////////////////////
	// Tags are user defined and allow users tag values
	// so that library functions or user code may perform
	// special-purpose processing.

	const std::string&	GetTypeTag (void) const				
							{ return typeTag; }
	void				SetTypeTag (const std::string& tag) 
							{ typeTag = tag; }

	//-------------------------------------------------------------
	
	DeltaValue (void);	
	DeltaValue (const DeltaValue& val);
	DeltaValue (const DeltaValue* val);
	DeltaValue (_Nil);	
	DeltaValue (const char* s);		// Needed to avoid implicit conversion to boolean.
	DeltaValue (const std::string& s);	
	DeltaValue (bool b);	
	DeltaValue (DeltaNumberValueType n);	
	DeltaValue (		
		void*							val,
		DeltaExternIdType				type								= DeltaExternId_NonCollectable,
		void							(*toString)(DeltaString*, void*)	= (void(*)(DeltaString*, void*)) 0,
		const std::string&				typeStr								= DELTA_EXTERNID_TYPESTR_UNDEF,
		const DeltaExternIdFieldGetter*	fieldGetter							= DELTA_EXTERNID_NO_FIELD_GETTER
	);
	DeltaValue (DeltaExternId* externId);	
	DeltaValue (DeltaTable* t);	
	DeltaValue (
		DeltaLibraryFunc			lf, 
		DeltaLibraryFuncArgsBinder* binder = (DeltaLibraryFuncArgsBinder*) 0
	);
	DeltaValue (
		DeltaCodeAddress		f, 
		DeltaVirtualMachine*	vm, 
		DeltaClosure*			closure = (DeltaClosure*) 0
	);
	DeltaValue (
		DeltaCodeAddress		f, 
		DeltaTable*				self, 
		DeltaVirtualMachine*	vm, 
		DeltaClosure*			closure = (DeltaClosure*) 0
	);
	DeltaValue (
		const DeltaStdFuncInfo* f, 
		DeltaVirtualMachine*	vm, 
		DeltaClosure*			closure = (DeltaClosure*) 0
	);
	DeltaValue (
		const DeltaStdFuncInfo* f, 
		DeltaTable*				self, 
		DeltaVirtualMachine*	vm, 
		DeltaClosure*			closure = (DeltaClosure*) 0
	);

	~DeltaValue();
};

//------------------------------------------------------------------

#endif	// Do not add stuff beyond this point.