// BufferLib.cpp
// Platform independent buffer I/0 for Delta.
// A. Savidis, March 2009.
//

#include "DeltaLibraryCreators.h"
#include "BufferLib.h"
#include "FileIOLib.h"
#include "ListLib.h"

DLIB_REGAIN_COLLECTABLESIMPLE_FUNC(ubinaryio::InputBuffer, INPUTBUFFER_TYPE_STR, INPUTBUFFER_VAL)
DLIB_REGAIN_COLLECTABLESIMPLE_FUNC(ubinaryio::OutputBuffer, OUTPUTBUFFER_TYPE_STR, OUTPUTBUFFER_VAL)

//---------------------------------------------------
// INPUT BUFFER

namespace input {

static DeltaExternIdFieldGetter* getter = (DeltaExternIdFieldGetter*) 0;

static bool GetKeys (void* val, DeltaValue* at) 
	{ return DPTR(getter)->GetAllKeys(val, at); }

static bool GetSize (void* val, DeltaValue* at) 
	{ at->FromNumber(INPUTBUFFER_VAL(val)->GetSize()); return true; }

static bool GetRemaining (void* val, DeltaValue* at) 
	{ at->FromNumber(INPUTBUFFER_VAL(val)->GetRemainingBytes()); return true; }

//************************

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",			&GetKeys,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "size",			&GetSize,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "remaining",		&GetRemaining,	DELTA_GETBYSTRING_NO_PRECOND	}
};

/////////////////////////////////////////////////////

static void inputbuffer_new_LibFunc (DeltaVirtualMachine*);
static void inputbuffer_set_LibFunc (DeltaVirtualMachine*);
static void inputbuffer_rewind_LibFunc (DeltaVirtualMachine*);
static void inputbuffer_size_LibFunc (DeltaVirtualMachine*);
static void inputbuffer_remaining_LibFunc (DeltaVirtualMachine*);
static void inputbuffer_eof_LibFunc (DeltaVirtualMachine*);

static DeltaLibraryObjectCreator::FuncEntry funcs[] = {
	{ "new",			inputbuffer_new_LibFunc },
	{ "set",			inputbuffer_set_LibFunc },
	{ "rewind",			inputbuffer_rewind_LibFunc },
	{ "size",			inputbuffer_size_LibFunc },
	{ "remaining",		inputbuffer_remaining_LibFunc },
	{ "eof",			inputbuffer_eof_LibFunc }
};

inline bool VAL_IS_INPUTBUFFER (const DeltaValue& val)
	{ return val.Type() == DeltaValue_ExternId && val.GetExternIdTypeString() == INPUTBUFFER_TYPE_STR; }
DELTALIBFUNC_DEFINE_METHODS_TABLE_AND_CALLER_CHECKER(VAL_IS_INPUTBUFFER(val), INPUTBUFFER_TYPE_STR)

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "set", "eof")

DLIB_INITIALIZE_CLEANUP_FUNCS_EX(
	InputBuffer, 
	"inputbuffer", 
	DELTA_STDLIB_NAMESPACE_PREFIX "inputbuffer_",
	DELTALIBFUNC_CREATE_METHODS_TABLE();,
	DELTALIBFUNC_DESTROY_METHODS_TABLE();
)

DLIB_COLLECTABLESIMPLE_TOSTRING_FUNC(INPUTBUFFER_TYPE_STR)


static void Destructor (void* val) 
	{ DDELETE((ubinaryio::InputBuffer*) val); }

DLIB_COLLECTABLESIMPLE_TOEXTERNID_FUNC_WITH_LIBRARY_METHODS(
	ubinaryio::InputBuffer, 
	INPUTBUFFER_TYPE_STR, 
	ToBuffer, 
	&Destructor
)

}	// namespace input

/////////////////////////////////////////////////////

void Install_DeltaInputBuffer_Lib (DeltaObject* mainAPI) 
	{ input::Install_DeltaInputBuffer_Lib(mainAPI); }

void CleanUp_DeltaInputBuffer_Lib (void) 
	{ input::CleanUp_DeltaInputBuffer_Lib(); }

void DeltaInputBuffer_Make (DeltaValue* at, ubinaryio::InputBuffer* buffer)
	{ input::ToBuffer(at, buffer); }

DVM_FUNC ubinaryio::InputBuffer* DeltaInputBuffer_Get (DeltaValue& val) {
	if (val.Type() != DeltaValue_ExternId)
		return (ubinaryio::InputBuffer*) 0;
	return INPUTBUFFER_VAL(val.ToExternId());
}

DVM_FUNC ubinaryio::OutputBuffer* DeltaOutputBuffer_Get (DeltaValue& val) {
	if (val.Type() != DeltaValue_ExternId)
		return (ubinaryio::OutputBuffer*) 0;
	return OUTPUTBUFFER_VAL(val.ToExternId());
}

/////////////////////////////////////////////////////
// IMPLEMENTATION OF INPUTBUFFER LIBRARY FUNCTIONS.

namespace input {

DLIB_FUNC_START(inputbuffer_new, 1, Nil)
DLIB_GET_OUTPUTBUFFER
ubinaryio::InputBuffer* ib = DNEWCLASS(ubinaryio::InputBuffer, (*DPTR(ob)));
input::ToBuffer(DLIB_RETVAL_PTR, ib);
DLIB_FUNC_END

DLIB_FUNC_START(inputbuffer_set, 2, Empty)
DLIB_GET_INPUTBUFFER
DLIB_GET_OUTPUTBUFFER
DPTR(ib)->Set(*DPTR(ob));
DLIB_FUNC_END

DLIB_FUNC_START(inputbuffer_rewind, 1, Empty)
DLIB_GET_INPUTBUFFER
DPTR(ib)->Rewind();
DLIB_FUNC_END

DLIB_FUNC_START(inputbuffer_size, 1, Zero)
DLIB_GET_INPUTBUFFER
DLIB_RETVAL_REF.FromNumber(DPTR(ib)->GetSize());
DLIB_FUNC_END

DLIB_FUNC_START(inputbuffer_remaining, 1, Zero)
DLIB_GET_INPUTBUFFER
DLIB_RETVAL_REF.FromNumber(DPTR(ib)->GetRemainingBytes());
DLIB_FUNC_END

DLIB_FUNC_START(inputbuffer_eof, 1, True)
DLIB_GET_INPUTBUFFER
DLIB_RETVAL_REF.FromBool(DPTR(ib)->IsEof());
DLIB_FUNC_END

}

//---------------------------------------------------
// OUTPUT BUFFER

namespace output {

/////////////////////////////////////////////////////

static DeltaExternIdFieldGetter* getter = (DeltaExternIdFieldGetter*) 0;

static bool GetKeys (void* val, DeltaValue* at) 
	{ return DPTR(getter)->GetAllKeys(val, at); }

static bool GetSize (void* val, DeltaValue* at) 
	{ at->FromNumber(OUTPUTBUFFER_VAL(val)->GetSize()); return true; }

static bool GetTotalPackets (void* val, DeltaValue* at) 
	{ at->FromNumber(OUTPUTBUFFER_VAL(val)->GetTotalPackets()); return true; }

//************************

static const std::string PacketToString (const  ubinaryio::OutputBuffer::Packet& packet);

static bool GetPackets (void* val, DeltaValue* at) {
	
	typedef ubinaryio::OutputBuffer::PacketList PacketList;
	const PacketList& packets = OUTPUTBUFFER_VAL(val)->GetPackets(); 
	
	DeltaList_MakeForDebuggerOnly(*at);
	std::list<DeltaValue>* l = DeltaList_Get(*at);
	DASSERT(l);

	for (PacketList::const_iterator i = packets.begin(); i != packets.end(); ++i)
		l->push_back(DeltaValue(PacketToString(*i)));
	return true; 
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "size",				&GetSize,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "totalpackets",		&GetTotalPackets,	DELTA_GETBYSTRING_NO_PRECOND	},
	{ "packets",			&GetPackets,		DELTA_GETBYSTRING_NO_PRECOND	}
};

/////////////////////////////////////////////////////
// Converters from a packet to string.

template <typename T> struct int_format_traits {};
template <> struct int_format_traits<util_ui8>		{ static const char* get (void) { return "ui8[%u]"; } };
template <> struct int_format_traits<util_ui16>		{ static const char* get (void) { return "ui16[%u]"; } };
template <> struct int_format_traits<util_i16>		{ static const char* get (void) { return "i16[%d]"; } };
template <> struct int_format_traits<util_ui32>		{ static const char* get (void) { return "ui32[%u]"; } };
template <> struct int_format_traits<util_i32>		{ static const char* get (void) { return "i32[%d]"; } };

template <class T>
static const std::string IntegerToString (void* b, util_ui32 n) {
	T val;
	ubinaryio::ReadInteger(b, &n, &val);
	return uconstructstr(int_format_traits<T>::get(), val);
}

static const std::string BoolToString (void* b, util_ui32 n) {
	bool val;
	ubinaryio::ReadBool(b, &n, &val);
	return std::string("bool[") + ubool2conststr(val) + "]";
}

static const std::string DoubleToString (void* b, util_ui32 n) {
	double val;
	ubinaryio::ReadDouble(b, &n, &val);
	return std::string("double[") + unum2string(val) + "]";
}

static const std::string StringToString (void* b, util_ui32 n) {
	std::string val;
	ubinaryio::ReadString(b, &n, val);
	return std::string("string[") + val + "]";
}

static const std::string BufferToString (void* b, util_ui32 n) {
	std::string s(uconstructstr("buffer(%u)[", n));
	util_ui8* p = (util_ui8*) b;
	while (n--) {
		s.append(uint2hexstring((util_ui32) (*p++)));
		if (n)
			s.append(".");
	}

	return s;
}

//************************

typedef const std::string (*PacketToStringFunc)(void*, util_ui32);

PacketToStringFunc packetToStringsFuncs[] = {
	&IntegerToString<util_ui8>,
	&IntegerToString<util_ui16>,
	&IntegerToString<util_i16>,
	&IntegerToString<util_ui32>,
	&IntegerToString<util_i32>,
	&BoolToString,
	&DoubleToString,
	&StringToString,
	&BufferToString
};

static const std::string PacketToString (const  ubinaryio::OutputBuffer::Packet& packet) {
	DASSERT(((util_ui32) packet.third) < uarraysize(packetToStringsFuncs));
	return (*packetToStringsFuncs[packet.third])(packet.first, packet.second);
}

/////////////////////////////////////////////////////

static void outputbuffer_new_LibFunc (DeltaVirtualMachine*);
static void outputbuffer_append_LibFunc (DeltaVirtualMachine*);
static void outputbuffer_isempty_LibFunc (DeltaVirtualMachine*);
static void outputbuffer_size_LibFunc (DeltaVirtualMachine*);
static void outputbuffer_totalpackets_LibFunc (DeltaVirtualMachine*);
static void outputbuffer_clear_LibFunc (DeltaVirtualMachine*);
static void outputbuffer_flush_LibFunc (DeltaVirtualMachine*);

static DeltaLibraryObjectCreator::FuncEntry funcs[] = {
	{ "new",			outputbuffer_new_LibFunc },
	{ "append",			outputbuffer_append_LibFunc },
	{ "isempty",		outputbuffer_isempty_LibFunc },
	{ "size",			outputbuffer_size_LibFunc },
	{ "totalpackets",	outputbuffer_totalpackets_LibFunc },
	{ "clear",			outputbuffer_clear_LibFunc },
	{ "flush",			outputbuffer_flush_LibFunc }
};


inline bool VAL_IS_OUTPUTBUFFER (const DeltaValue& val)
	{ return val.Type() == DeltaValue_ExternId && val.GetExternIdTypeString() == OUTPUTBUFFER_TYPE_STR; }
DELTALIBFUNC_DEFINE_METHODS_TABLE_AND_CALLER_CHECKER(VAL_IS_OUTPUTBUFFER(val), OUTPUTBUFFER_TYPE_STR)

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "append", "flush")

DLIB_INITIALIZE_CLEANUP_FUNCS_EX(
	OutputBuffer, 
	"outputbuffer", 
	DELTA_STDLIB_NAMESPACE_PREFIX "outputbuffer_",
	DELTALIBFUNC_CREATE_METHODS_TABLE();,
	DELTALIBFUNC_DESTROY_METHODS_TABLE();
)

DLIB_COLLECTABLESIMPLE_TOSTRING_FUNC(OUTPUTBUFFER_TYPE_STR)

static void Destructor (void* val) 
	{ DDELETE((ubinaryio::OutputBuffer*) val); }

DLIB_COLLECTABLESIMPLE_TOEXTERNID_FUNC_WITH_LIBRARY_METHODS(
	ubinaryio::OutputBuffer, 
	OUTPUTBUFFER_TYPE_STR, 
	ToBuffer, 
	&Destructor
)

}	// namespace output

/////////////////////////////////////////////////////

void Install_DeltaOutputBuffer_Lib (DeltaObject* mainAPI) 
	{ output::Install_DeltaOutputBuffer_Lib(mainAPI); }

void CleanUp_DeltaOutputBuffer_Lib (void) 
	{ output::CleanUp_DeltaOutputBuffer_Lib(); }

void DeltaOutputBuffer_Make (DeltaValue* at, ubinaryio::OutputBuffer* buffer)
	{ output::ToBuffer(at, buffer); }

/////////////////////////////////////////////////////
// IMPLEMENTATION OF OUTPUTBUFFER LIBRARY FUNCTIONS.

namespace output {

DLIB_FUNC_START(outputbuffer_new, 0, Empty)
ubinaryio::OutputBuffer* ib = DNEW(ubinaryio::OutputBuffer);
output::ToBuffer(DLIB_RETVAL_PTR, ib);
DLIB_FUNC_END

DLIB_FUNC_START(outputbuffer_append, 2, Empty)
DLIB_GET_OUTPUTBUFFER_EX(dest)
DLIB_GET_OUTPUTBUFFER_EX(src)
DPTR(dest)->Append(*DPTR(src));
DLIB_FUNC_END

DLIB_FUNC_START(outputbuffer_isempty, 1, True)
DLIB_GET_OUTPUTBUFFER
DLIB_RETVAL_REF.FromBool(DPTR(ob)->IsEmpty());
DLIB_FUNC_END

DLIB_FUNC_START(outputbuffer_size, 1, Zero)
DLIB_GET_OUTPUTBUFFER
DLIB_RETVAL_REF.FromNumber(DPTR(ob)->GetSize());
DLIB_FUNC_END

DLIB_FUNC_START(outputbuffer_totalpackets, 1, Zero)
DLIB_GET_OUTPUTBUFFER
DLIB_RETVAL_REF.FromNumber(DPTR(ob)->GetTotalPackets());
DLIB_FUNC_END

DLIB_FUNC_START(outputbuffer_clear, 1, Empty)
DLIB_GET_OUTPUTBUFFER
DPTR(ob)->Clear();
DLIB_FUNC_END

DLIB_FUNC_START(outputbuffer_flush, 2, Empty)
DLIB_GET_OUTPUTBUFFER
DLIB_GET_FILE(fp)
DPTR(ob)->Flush(fp);
DLIB_FUNC_END

}

//---------------------------------------------------
