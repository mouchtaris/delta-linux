// DeltaStdLibTemplates.h
// Helper templates and macros to produce library functions.
// ScriptFighter Project.
// A. Savidis, November 2008.
//

#ifndef	DELTASTDLIBTEMPLATES_H
#define	DELTASTDLIBTEMPLATES_H

#include "DeltaByteCodeTypes.h"
#include "DeltaValue.h"
#include "DeltaTable.h"
#include "utypes.h"
#include "uerrorclass.h"
#include "DeltaStdLibErrors.h"
#include "DeltaVirtualMachine.h"
#include <string>

////////////////////////////////////////////////////////////
// Macros named as _<name>_ are meant to be private, not to be used outside
// the header. All the rest are for free use.
////////////////////////////////////////////////////////////

#define	_DLIB_GET_ARG_(type, func, n)							\
	DASSERT(!arg);												\
  	arg = DPTR(vm)->GetActualArg(n);							\
	if (arg->Type() != type) {									\
		error = uconstructstr(									\
					"in %s() arg #%d: %s expected, %s passed!", \
					func, n,									\
					DeltaValue::GetTypeStr(type),				\
					arg->TypeStr()								\
				);												\
		return false;											\
	}
	
#define	_DLIB_GET_NUMBER_(func, n)					\
	_DLIB_GET_ARG_(DeltaValue_Number, func, n);	\
	DeltaNumberValueType num = arg->ToNumber()		\

#define	_DLIB_GET_BOOL_(func, n)					\
	_DLIB_GET_ARG_(DeltaValue_Bool, func, n);		\
	val = arg->ToBool()	\

#define	_DLIB_GET_STRING_(func, n)					\
	_DLIB_GET_ARG_(DeltaValue_String, func, n);	\
	val = arg->ToString();							\

#define	_DLIB_GET_TABLE_(func, n)					\
	_DLIB_GET_ARG_(DeltaValue_Table, func, n);		\
	val = arg->ToTable();							\

#define	_DLIB_GET_EXTERNID_(func, n)				\
	_DLIB_GET_ARG_(DeltaValue_ExternId, func, n);	\
	val = arg->ToExternId();

#define	_DLIB_GET_EXTERNID_LOOSE_(func, n)			\
		arg = DPTR(vm)->GetActualArg(n);			\
		val = (void*) 0;

#define	_DLIB_GET_ANY_(func, n) \
	val = arg = DPTR(vm)->GetActualArg(n)

////////////////////////////////////////////////////////////

#define	_DLIB_ARG_BASIC_TOSTRING_													\
		const std::string	ToString (void) const	{								\
								DASSERT(arg);										\
								return arg->ConvertToStringBySkippingUserCode();	\
							}

#define	_DLIB_ARG_BASIC_MEMBERS_													\
		std::string		error;														\
		std::string		name;														\
		DeltaValue*	arg;															\
		const std::string&	GetError (void) const { return error; }					\
		const std::string&	GetName (void) const { return name; }					\
		const DeltaValue*	GetArg (void) const { return arg; }						\
		DeltaArg (const std::string _name) : name(_name), arg((DeltaValue*) 0){}	\

#define	_DLIB_ARG_MEMBERS_EXCEPT_TOSTRING_(T)						\
		T				val;										\
		T				GetValue (void) const { return val; }		\
		void			SetValue (T& _val) { val = _val; }			\
		_DLIB_ARG_BASIC_MEMBERS_

#define	_DLIB_ARG_MEMBERS_(T)										\
		_DLIB_ARG_MEMBERS_EXCEPT_TOSTRING_(T)						\
		_DLIB_ARG_BASIC_TOSTRING_

#define	_DLIB_BASIC_FUNCTOR_(get)												\
	bool operator()(DeltaVirtualMachine* vm, const char* func, util_ui32 n)		\
		{ get(func, n); return true; }

////////////////////////////////////////////////////////////

template <typename T> struct DeltaArg {};

template <> struct DeltaArg<util_i32> {
	
	_DLIB_ARG_MEMBERS_(util_i32)
	bool operator()(DeltaVirtualMachine* vm, const char* func, util_ui32 n) {
		_DLIB_GET_NUMBER_(func, n);
		if (((util_i32) num) != num) {
			error = uconstructstr("in %s() arg %s (#%d): int expected, real passed!", func, name.c_str(), n);
			return false;
		}
		else
			{ val = (util_i32) num; return true; }
	}
};

////////////////////////////////////////////////////////////

template <> struct DeltaArg<util_ui32> {
	
	_DLIB_ARG_MEMBERS_(util_ui32)
	bool operator()(DeltaVirtualMachine* vm, const char* func, util_ui32 n) {
		_DLIB_GET_NUMBER_(func, n);
		if (((util_ui32) num) != num) {
			error = uconstructstr("in %s() arg %s (#%d): unsigned expected, real passed!", func, name.c_str(), n);
			return false;
		}
		else
			{ val = (util_ui32) num; return true; }
	}
};

////////////////////////////////////////////////////////////

template <> struct DeltaArg<double> {
	_DLIB_ARG_MEMBERS_(double)
	bool operator()(DeltaVirtualMachine* vm, const char* func, util_ui32 n) 
		{ _DLIB_GET_NUMBER_(func, n); val = num; return true; }
};

////////////////////////////////////////////////////////////
// Handles polymorphic externids.

template <> struct DeltaArg<void*> {

	_DLIB_ARG_MEMBERS_EXCEPT_TOSTRING_(void*)
	DeltaValue	result;
	util_ui32	serialNo;

	bool				Get (DeltaVirtualMachine* vm, const char* func, util_ui32 n)
							{ _DLIB_GET_EXTERNID_(func, n); return true; }

	bool				GetPolymorphic (DeltaVirtualMachine* vm, const char* func, util_ui32 n)
							{ _DLIB_GET_EXTERNID_LOOSE_(func, n); unullify(val); return true; }

	const std::string	GetTypeStr (void) const {
							DASSERT(arg); 
							if (arg->Type() == DeltaValue_ExternId)
								return arg->GetExternIdTypeString();
							else
								return arg->TypeStr();
						}

	util_ui32			GetSerialNo (void) const
							{ DASSERT(arg); return serialNo; }

	const std::string	ToString (void) const	{
							DASSERT(arg && val);
							if (arg->Type() == DeltaValue_ExternId)
								return uconstructstr(
											"%s(0x%x, type '%s')",
											arg->TypeStr(),
											arg->ToExternIdSerialNo(), 
											GetTypeStr().c_str()
										);
							else
								return arg->TypeStr();
						}

	bool operator()(DeltaVirtualMachine* vm, const char* func, util_ui32 n, const char* typeExpected) {
		DASSERT(!arg);
		serialNo = 0;
		if (!GetPolymorphic(vm, func, n)) 
			return false;
		else
		if (!arg->ConvertToExternId(typeExpected, &result)) {
			error = uconstructstr(
				"in %s() while trying to convert arg %s (#%d) of type '%s' to externid '%s'!",
				func,
				name.c_str(),
				n,
				arg->TypeStr(),
				typeExpected
			);
			return false;
		}
		else
		if (result.Type() == DeltaValue_Undefined) {
			error = uconstructstr(
				"in %s() arg %s (#%d) of type '%s': no conversion to externid '%s' was possible!",
				func,
				name.c_str(),
				n,
				arg->TypeStr(),
				typeExpected
			);
			return false;
		}
		else {
			DASSERT(typeExpected == result.GetExternIdTypeString());
			val = result.ToExternId();
			serialNo = result.ToExternIdSerialNo();
			return true; 
		}
	}
};

////////////////////////////////////////////////////////////

template <> struct DeltaArg<DeltaTable*> {

	_DLIB_ARG_MEMBERS_EXCEPT_TOSTRING_(DeltaTable*)
	_DLIB_BASIC_FUNCTOR_(_DLIB_GET_TABLE_)

	const std::string ToString (void) const	{
			DASSERT(arg && val);				
			return	uconstructstr(
						"%s(0x%x)", 
						DPTR(val)->GetExtClassString(), 
						DPTR(val)->GetSerialNo()
					);
	}
};

////////////////////////////////////////////////////////////

template <> struct DeltaArg<const char*> {

	_DLIB_ARG_BASIC_MEMBERS_ 
	_DLIB_ARG_BASIC_TOSTRING_
	std::string val;

	_DLIB_BASIC_FUNCTOR_(_DLIB_GET_STRING_)

	void		SetValue (const std::string& _val) { val = _val; }
	const char*	GetValue (void) const { return val.c_str(); }
	
};

////////////////////////////////////////////////////////////

#define	_DLIB_BASIC_DELTAARG_(type, get)										\
template <> struct DeltaArg<type> {												\
	_DLIB_ARG_MEMBERS_(type)													\
	_DLIB_BASIC_FUNCTOR_(get)													\
};

_DLIB_BASIC_DELTAARG_(bool,			_DLIB_GET_BOOL_)
_DLIB_BASIC_DELTAARG_(DeltaValue*,	_DLIB_GET_ANY_)

////////////////////////////////////////////////////////////
// Return value reseters.
//
#define	_DLIB_RESET_(name,val,call)								\
struct Delta##name##Reset {										\
	DeltaVirtualMachine* vm;									\
	void operator()(void)										\
		{ vm->GetReturnValue().call(val); }						\
	Delta##name##Reset (DeltaVirtualMachine* _vm) : vm(_vm){}	\
};

#define	_DLIB_EMPTY_
#define	_DLIB_PARAM_(x)	, x

_DLIB_RESET_(True,	true,			FromBool)
_DLIB_RESET_(False,	false,			FromBool)
_DLIB_RESET_(Zero,	0,				FromNumber)
_DLIB_RESET_(Nil,	_DLIB_EMPTY_,	FromNil)

struct DeltaEmptyReset {
	void operator()(void){}
	DeltaEmptyReset (DeltaVirtualMachine* vm){}
};

#define	DLIB_RESET_RETURN	if (true) { reseter(); return; } else

////////////////////////////////////////////////////////////
// Begins lib functions. Pass as reset one of the following:
// Empty, False, Zero, or Nil. They define what a lib function is
// supposed to return in exceptional cases and in errors.
// You can cause a reset-return sequence via DLIB_RESET_RETURN
// (needs semicolons). In funcs with optional args the m concerns
// the extra arguments.
//
#define	DLIB_FUNC_START_OPT_ARGS(name, n, m, reset)			\
	static void name##_LibFunc (DeltaVirtualMachine* vm) {	\
		ISSUE_FUNC(#name);									\
		util_ui32 POSSIBLY_UNUSED_LOCAL(_argNo) = 0;		\
		Delta##reset##Reset reseter(vm);					\
		if (DPTR(vm)->TotalActualArgs() < (n)) {			\
			DPTR(vm)->PrimaryError(							\
				DEFORMAT_MOREARGSNO,						\
				(n),										\
				vm->TotalActualArgs(),						\
				CURR_FUNC									\
			);												\
			DLIB_RESET_RETURN;								\
		}													\
		if (DPTR(vm)->TotalActualArgs() > (n) + (m)) {		\
			DPTR(vm)->PrimaryError(							\
				DEFORMAT_LESSARGSNO,						\
				(n) + (m),									\
				vm->TotalActualArgs(),						\
				CURR_FUNC									\
			);												\
			DLIB_RESET_RETURN;								\
		}													\
		std::string _sig1, _sig2;

//*****************************

#define	DLIB_FUNC_START(name, n, reset)						\
	static void name##_LibFunc (DeltaVirtualMachine* vm) {	\
		ISSUE_FUNC(#name);									\
		util_ui32 POSSIBLY_UNUSED_LOCAL(_argNo) = 0;		\
		Delta##reset##Reset reseter(vm);					\
		if (DPTR(vm)->TotalActualArgs() != (n)) {			\
			DPTR(vm)->PrimaryError(							\
				DEFORMAT_ARGSNO,							\
				(n),										\
				vm->TotalActualArgs(),						\
				CURR_FUNC									\
			);												\
			DLIB_RESET_RETURN;								\
		}													\
		std::string _sig1, _sig2;

//*****************************

#define	_DLIB_ARG_TEMPLATE_(T, name, type, call)			\
	DeltaArg<T> name(#name);								\
	if (!call(name)(vm, CURR_FUNC, _argNo type)) {			\
		DPTR(vm)->PrimaryError(name.GetError().c_str());	\
		DLIB_RESET_RETURN;									\
	}														\
	if (_argNo) {_sig1 += ","; _sig2 += ","; }				\
	_sig1 += name.GetName();								\
	_sig2 += name.ToString();								\
	++_argNo;								

//*****************************
// Optional args when provided should match.
// Optional args cannot be followed by normal args.

#define	_DLIB_OPT_ARG_TEMPLATE_(T, name, type, call, value)	\
	DeltaArg<T> name(#name);								\
	if (DPTR(vm)->TotalActualArgs() > _argNo) {				\
		if (!call(name)(vm, CURR_FUNC, _argNo type)) {		\
			DPTR(vm)->PrimaryError(name.GetError().c_str());\
			DLIB_RESET_RETURN;								\
		}													\
		if (_argNo) {_sig1 += ","; _sig2 += ","; }			\
		_sig1 += std::string("[") + name.GetName()	+ "]";	\
		_sig2 += std::string("[") + name.ToString() + "]";	\
		++_argNo;											\
	} else													\
		name.SetValue(value);								\

//*****************************
// Use DLIB_ARG to introduce comma separated arguments after
// the function header. If you want to retrieve an externid
// of a fixed type string use the DLIB_ARGEX instead. If you wish
// to retrieve an extern id with various eligible types (C++ polymorphic)
// use DLIB_ARGEX_BASECLASS then test its type via DLIB_ARGEX_TYPESTR.

#define	_DLIB_CALLGET_(x)	x.Get
#define	_DLIB_ID_(x)		x

#define	DLIB_ARG(T, name)									\
	_DLIB_ARG_TEMPLATE_(T, name, _DLIB_EMPTY_, _DLIB_ID_)

#define	DLIB_OPT_ARG(T, name, value)						\
	_DLIB_OPT_ARG_TEMPLATE_(T, name, _DLIB_EMPTY_, _DLIB_ID_, value)

// Use this when the externid can be polymorphic in a Delta fashion
// (i.e. its userdata belogn to a subobject tree).
#define	DLIB_ARGEX(name, type)								\
	_DLIB_ARG_TEMPLATE_(void*, name, _DLIB_PARAM_(type), _DLIB_ID_)

#define	DLIB_ARGEX_BASECLASS(name)							\
	_DLIB_ARG_TEMPLATE_(void*, name, _DLIB_EMPTY_, _DLIB_CALLGET_)

#define	DLIB_ARGEX_JUSTVALUE(name)							\
	_DLIB_ARG_TEMPLATE_(void*, name, _DLIB_EMPTY_, _DLIB_CALLGET_)

//*****************************

#define	_DLIB_CALL_CROP_			32
#define	DLIB_ARGVAL(name)			(name.GetValue())
#define	DLIB_ARGSTR(name)			(name.ToString().c_str())
#define	DLIB_ARG_OBJECT(name)		(name.GetArg())
#define	DLIB_CALLSIG				_sig2.c_str()
#define	DLIB_CALLSIG_CROPPED		ucropstr(_sig2, _DLIB_CALL_CROP_, "..").c_str()
#define	DLIB_RETVAL_REF				DPTR(vm)->GetReturnValue()
#define	DLIB_RETVAL_PTR				&DLIB_RETVAL_REF
#define	DLIB_ARGEX_TYPESTR(name)	(name.GetTypeStr())
#define	DLIB_ARGEX_SERIALNO(name)	(name.GetSerialNo())

//*****************************

#define	DLIB_CALL_CHECK_OPEN(f)		if (!(f)(

#define	DLIB_CALL_CHECK_CLOSE													\
		)) {																	\
			DPTR(vm)->PrimaryError("%s(%s) failed!", CURR_FUNC, _sig1.c_str());	\
			DLIB_RESET_RETURN;													\
		}

#define	DLIB_CALL_CHECK_CLOSE_POSSIBLE_PRIMARY											\
		)) {																			\
			DPTR(vm)->PossiblePrimaryError("%s(%s) failed!", CURR_FUNC, _sig1.c_str());	\
			DLIB_RESET_RETURN;															\
		}

#define	DLIB_ERROR_CHECK(cond, error)											\
		if (cond) {																\
			DPTR(vm)->PrimaryError("%s(%s): %s!",								\
				CURR_FUNC, _sig1.c_str(), ucstringarg(error)					\
			);																	\
			DLIB_RESET_RETURN;													\
		} else;

#define	DLIB_ERROR_REPORT(error)												\
		if (true) {																\
			DPTR(vm)->PrimaryError("%s(%s): %s!",								\
				CURR_FUNC, _sig1.c_str(), ucstringarg(error)					\
			);																	\
			DLIB_RESET_RETURN;													\
		} else;

#define	DLIB_RESETRETURN_CHECK(cond)	if (cond) { DLIB_RESET_RETURN; }
#define	DLIB_CALL_VOID(f)				((f)(
#define	DLIB_CALL_CLOSE					));
#define	DLIB_CALL_RETURN(type,f)		DPTR(vm)->GetReturnValue().From##type((f)(
#define	DLIB_FUNC_END					}

////////////////////////////////////////////////////////////
// Wrapping a hierarchy of native classes so that the wrapper class hierarchy is
// isomorphic to the wrapper class hierarchy, to support polymorphism in all exported
// library functions in a way similar to the native library. Only one externid type is 
// actually exported and this is the native wrapper superclass (see below).

// The following creates the superclass for wrappers that is actually the only
// class exported to an entire native library of related (via inheritance) classes.
// It is defined once and before the rest of wrappers, per native class, are defined.
// ClassId below may be produced using uvalidatable.h macros like VCLASSID_DEFINE.

template <class ClassId> 
class DeltaNativeClassWrapperSuperclass : public virtual Validatable {
	protected:
	void* castToNativeInst;

	public:
	virtual const char*	GetExtClassString (void) const 
							{ return ClassId::ID; }
	void*				GetCastToNativeInstance (void)
							{ return castToNativeInst; }
	void				SetCastToNativeInstance (void* inst)
							{ castToNativeInst = inst; }
	DeltaNativeClassWrapperSuperclass (void) : castToNativeInst((void*) 0){}
};

// Internal utility macro, will not use outside.
#define	_DLIB_NATIVE_CLASS_WRAPPER_STD_MEMBERS_									\
	private:																	\
	void* nativeInst;															\
	protected:																	\
	typedef Native NativeClass;													\
	public:																		\
	virtual const char*	GetExtClassString (void) const { return ClassId::ID; }	\
	void*				GetNativeInstance (void) { return nativeInst; }

//*****************************
// WrapperSuper is the wrapper superclass defined using the previous template.
// This template is used to produce wrappers for toplevel native classes.

template <class ClassId, class WrapperSuper, class Native> 
class DeltaNativeClassNoBaseWrapper : public virtual WrapperSuper {
	
	_DLIB_NATIVE_CLASS_WRAPPER_STD_MEMBERS_
	
	DYNAMIC_CAST_NOBASE_INLINE_IMPL_EX(
		DeltaNativeClassNoBaseWrapper::GetExtClassString(), 
		this->SetCastToNativeInstance(nativeInst)
	)

	DeltaNativeClassNoBaseWrapper (void* _nativeInst) : nativeInst(_nativeInst) {}
};

//*****************************

template <class ClassId, class Base, class Native> 
class DeltaNativeClassSingleBaseWrapper : public Base {

	_DLIB_NATIVE_CLASS_WRAPPER_STD_MEMBERS_

	DYNAMIC_CAST_SINGLEBASE_INLINE_IMPL_EX(
		Base,
		DeltaNativeClassSingleBaseWrapper::GetExtClassString(), 
		this->SetCastToNativeInstance(nativeInst)
	)

	DeltaNativeClassSingleBaseWrapper (void* _nativeInst) : 
		Base		((typename Base::NativeClass*) ((Native*) _nativeInst)),
		nativeInst	(_nativeInst) 
			{}
};

//*****************************

template <class ClassId, class Base1, class Base2, class Native> 
class DeltaNativeClassDualBaseWrapper : public Base1, public Base2 {

	_DLIB_NATIVE_CLASS_WRAPPER_STD_MEMBERS_

	DYNAMIC_CAST_DUALBASE_INLINE_IMPL_EX(
		Base1,
		Base2,
		DeltaNativeClassDualBaseWrapper::GetExtClassString(), 
		this->SetCastToNativeInstance(nativeInst)
	)

	DeltaNativeClassDualBaseWrapper (void* _nativeInst) :
			Base1		((typename Base1::NativeClass*) ((Native*) _nativeInst)),
			Base2		((typename Base2::NativeClass*) ((Native*) _nativeInst)),
			nativeInst	(_nativeInst)
			{}
};

//*****************************

template <class ClassId, class Base1, class Base2, class Base3, class Native> 
class DeltaNativeClassTripleBaseWrapper : public Base1, public Base2, public Base3 {

	_DLIB_NATIVE_CLASS_WRAPPER_STD_MEMBERS_

	DYNAMIC_CAST_TRIPLEBASE_INLINE_IMPL_EX(
		Base1,
		Base2,
		Base3,
		DeltaNativeClassTripleBaseWrapper::GetExtClassString(), 
		this->SetCastToNativeInstance(nativeInst)
	)

	DeltaNativeClassTripleBaseWrapper (void* _nativeInst) :
			Base1		((typename Base1::NativeClass*) ((Native*) _nativeInst)),
			Base2		((typename Base2::NativeClass*) ((Native*) _nativeInst)),
			Base3		((typename Base3::NativeClass*) ((Native*) _nativeInst)),
			nativeInst	(_nativeInst)
			{}
};

////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
