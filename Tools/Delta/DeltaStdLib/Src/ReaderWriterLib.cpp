// ReaderWriterLib.cpp
// Generic readers / writers for Delta.
// A. Savidis, March 2009.
//

#include "DeltaLibraryCreators.h"
#include "ReaderWriterLib.h"
#include "BufferLib.h"
#include "FileIOLib.h"
#include "DeltaStdLibFuncNames.h"

//---------------------------------------------------

#define	NO_DESTRUCTOR (void(*)(void*)) 0

template <class Wrapper>
static void Destructor (void* val) {
	Wrapper* wrapper = (Wrapper*) val;
	DDELETE(wrapper->second);
	DDELETE(wrapper->first);
	DDELETE(wrapper);
}

///////////////////////////////////////////////////////

DLIB_REGAIN_COLLECTABLESIMPLE_FUNC(ReaderWrapper, READER_TYPE_STR, READER_WRAPPER_VAL)
DLIB_REGAIN_COLLECTABLESIMPLE_FUNC(WriterWrapper, WRITER_TYPE_STR, WRITER_WRAPPER_VAL)

inline GenericReader* READER_VAL (void* val)
	{ return READER_WRAPPER_VAL(val)->first; }

inline GenericWriter* WRITER_VAL (void* val)
	{ return WRITER_WRAPPER_VAL(val)->first; }

//---------------------------------------------------
// READER

namespace reader {

static DeltaExternIdFieldGetter* getter = (DeltaExternIdFieldGetter*) 0;

static bool GetKeys (void* val, DeltaValue* at) 
	{ return DPTR(getter)->GetAllKeys(val, at); }

static bool GetContext (void* val, DeltaValue* at) 
	{ at->FromString(READER_VAL(val)->context()); return true; }

static bool GetIsEof (void* val, DeltaValue* at) 
	{ at->FromBool(READER_VAL(val)->eof()); return true; }

static bool GetFrom (void* val, DeltaValue* at) 
	{ at->Assign(READER_WRAPPER_VAL(val)->second); return true; }

//************************

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",		&GetKeys,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "context",	&GetContext,	DELTA_GETBYSTRING_NO_PRECOND	},
	{ "eof",		&GetIsEof,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "from",		&GetFrom,		DELTA_GETBYSTRING_NO_PRECOND	}
};

static void reader_fromfile_LibFunc (DeltaVirtualMachine*);
static void reader_frominputbuffer_LibFunc (DeltaVirtualMachine*);
static void reader_read_ui8_LibFunc (DeltaVirtualMachine*);
static void reader_read_ui16_LibFunc (DeltaVirtualMachine*);
static void reader_read_i16_LibFunc (DeltaVirtualMachine*);
static void reader_read_ui32_LibFunc (DeltaVirtualMachine*);
static void reader_read_i32_LibFunc (DeltaVirtualMachine*);
static void reader_read_bool8_LibFunc (DeltaVirtualMachine*);
static void reader_read_real64_LibFunc (DeltaVirtualMachine*);
static void reader_read_string_LibFunc (DeltaVirtualMachine*);
static void reader_read_buffer_LibFunc (DeltaVirtualMachine*);

static DeltaLibraryObjectCreator::FuncEntry funcs[] = {
	{ "fromfile",				reader_fromfile_LibFunc },
	{ "frominputbuffer",		reader_frominputbuffer_LibFunc },
	{ "read_ui8",				reader_read_ui8_LibFunc },
	{ "read_ui16",				reader_read_ui16_LibFunc },
	{ "read_i16",				reader_read_i16_LibFunc },
	{ "read_ui32",				reader_read_ui32_LibFunc },
	{ "read_i32",				reader_read_i32_LibFunc },
	{ "read_bool8",				reader_read_bool8_LibFunc },
	{ "read_real64",			reader_read_real64_LibFunc },
	{ "read_string",			reader_read_string_LibFunc },
	{ "read_buffer",			reader_read_buffer_LibFunc }
};

inline bool VAL_IS_READER (const DeltaValue& val)
	{ return val.Type() == DeltaValue_ExternId && val.GetExternIdTypeString() == READER_TYPE_STR; }
DELTALIBFUNC_DEFINE_METHODS_TABLE_AND_CALLER_CHECKER(VAL_IS_READER(val), READER_TYPE_STR)

DELTALIBFUNC_DECLARECONSTS(2, uarraysize(funcs) - 2, "read_ui8", "read_buffer")

DLIB_INITIALIZE_CLEANUP_FUNCS_EX(
	Reader, 
	"reader", 
	DELTA_STDLIB_NAMESPACE_PREFIX "reader_",
	DELTALIBFUNC_CREATE_METHODS_TABLE();,
	DELTALIBFUNC_DESTROY_METHODS_TABLE();
)

DLIB_COLLECTABLESIMPLE_TOSTRING_FUNC(READER_TYPE_STR)

DLIB_COLLECTABLESIMPLE_TOEXTERNID_FUNC_WITH_LIBRARY_METHODS(
	ReaderWrapper, 
	READER_TYPE_STR, 
	ToExternId, 
	&Destructor<ReaderWrapper>
)

}	// namespace reader

/////////////////////////////////////////////////////

namespace reader {

DLIB_FUNC_START(reader_fromfile, 1, Nil)
DLIB_GET_FILE(fp)
reader::ToExternId(
	DLIB_RETVAL_PTR, 
	DNEWCLASS(
		ReaderWrapper, 
		(DNEWCLASS(PortableBinFileReader, (fp)), DNEWCLASS(DeltaValue, (DLIB_ARG_OBJECT(fp_val))))
	)
);
DLIB_FUNC_END

DLIB_FUNC_START(reader_frominputbuffer, 1, Nil)
DLIB_GET_INPUTBUFFER;
reader::ToExternId(
	DLIB_RETVAL_PTR, 
	DNEWCLASS(
		ReaderWrapper, 
		(DNEWCLASS(PortableBufferReader, (ib)), DNEWCLASS(DeltaValue, (DLIB_ARG_OBJECT(ib_val))))
	)
);
DLIB_FUNC_END

#define	READ_NUMBER(suffix, type)				\
DLIB_FUNC_START(reader_read_##suffix, 1, Nil)	\
DLIB_GET_READER									\
type val;										\
DLIB_RESETRETURN_CHECK(!reader->read(&val));	\
DLIB_RETVAL_REF.FromNumber(val);				\
DLIB_FUNC_END

READ_NUMBER(ui8,	util_ui8)
READ_NUMBER(ui16,	util_ui16)
READ_NUMBER(i16,	util_i16)
READ_NUMBER(ui32,	util_ui32)
READ_NUMBER(i32,	util_i32)
READ_NUMBER(real64, double)

DLIB_FUNC_START(reader_read_bool8, 1, Nil)
DLIB_GET_READER		
bool val;
DLIB_RESETRETURN_CHECK(!reader->read(&val));
DLIB_RETVAL_REF.FromBool(val);
DLIB_FUNC_END

DLIB_FUNC_START(reader_read_string, 1, Nil)
DLIB_GET_READER		
std::string val;
DLIB_RESETRETURN_CHECK(!reader->read(val, false));
DLIB_RETVAL_REF.FromString(val);
DLIB_FUNC_END

DLIB_FUNC_START(reader_read_buffer, 2, Nil)
DLIB_GET_READER
DLIB_ARG(util_ui32,	size);
util_ui32 n			=  DLIB_ARGVAL(size);
util_ui8* buffer	= DNEWARR(util_ui8, n);
DLIB_RESETRETURN_CHECK(!reader->read(buffer, n));
ubinaryio::InputBuffer* ib = DNEWCLASS(ubinaryio::InputBuffer, (buffer, n));
DeltaInputBuffer_Make(DLIB_RETVAL_PTR, ib);
DLIB_FUNC_END

}

//---------------------------------------------------
// WRITER

namespace writer {

static DeltaExternIdFieldGetter* getter = (DeltaExternIdFieldGetter*) 0;

static bool GetKeys (void* val, DeltaValue* at) 
	{ return DPTR(getter)->GetAllKeys(val, at); }

static bool GetContext (void* val, DeltaValue* at) 
	{ at->FromString(WRITER_VAL(val)->context()); return true; }

static bool GetTo (void* val, DeltaValue* at) 
	{ at->Assign(WRITER_WRAPPER_VAL(val)->second); return true; }

//************************

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",		&GetKeys,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "context",	&GetContext,	DELTA_GETBYSTRING_NO_PRECOND	},
	{ "to",			&GetTo,			DELTA_GETBYSTRING_NO_PRECOND	}
};

static void writer_fromfile_LibFunc (DeltaVirtualMachine*);
static void writer_fromoutputbuffer_LibFunc (DeltaVirtualMachine*);
static void writer_write_ui8_LibFunc (DeltaVirtualMachine*);
static void writer_write_ui16_LibFunc (DeltaVirtualMachine*);
static void writer_write_i16_LibFunc (DeltaVirtualMachine*);
static void writer_write_ui32_LibFunc (DeltaVirtualMachine*);
static void writer_write_i32_LibFunc (DeltaVirtualMachine*);
static void writer_write_bool8_LibFunc (DeltaVirtualMachine*);
static void writer_write_real64_LibFunc (DeltaVirtualMachine*);
static void writer_write_string_LibFunc (DeltaVirtualMachine*);
static void writer_write_buffer_LibFunc (DeltaVirtualMachine*);

static DeltaLibraryObjectCreator::FuncEntry funcs[] = {
	{ "fromfile",				writer_fromfile_LibFunc },
	{ "fromoutputbuffer",		writer_fromoutputbuffer_LibFunc },
	{ "write_ui8",				writer_write_ui8_LibFunc },
	{ "write_ui16",				writer_write_ui16_LibFunc },
	{ "write_i16",				writer_write_i16_LibFunc },
	{ "write_ui32",				writer_write_ui32_LibFunc },
	{ "write_i32",				writer_write_i32_LibFunc },
	{ "write_bool8",			writer_write_bool8_LibFunc },
	{ "write_real64",			writer_write_real64_LibFunc },
	{ "write_string",			writer_write_string_LibFunc },
	{ "write_buffer",			writer_write_buffer_LibFunc }
};

inline bool VAL_IS_WRITER (const DeltaValue& val)
	{ return val.Type() == DeltaValue_ExternId && val.GetExternIdTypeString() == WRITER_TYPE_STR; }
DELTALIBFUNC_DEFINE_METHODS_TABLE_AND_CALLER_CHECKER(VAL_IS_WRITER(val), WRITER_TYPE_STR)

DELTALIBFUNC_DECLARECONSTS(2, uarraysize(funcs) - 2, "write_ui8", "write_buffer")

DLIB_INITIALIZE_CLEANUP_FUNCS_EX(
	Writer, 
	"writer", 
	DELTA_STDLIB_NAMESPACE_PREFIX "writer_",
	DELTALIBFUNC_CREATE_METHODS_TABLE();,
	DELTALIBFUNC_DESTROY_METHODS_TABLE();
)

DLIB_COLLECTABLESIMPLE_TOSTRING_FUNC(WRITER_TYPE_STR)

DLIB_COLLECTABLESIMPLE_TOEXTERNID_FUNC_WITH_LIBRARY_METHODS(
	WriterWrapper, 
	WRITER_TYPE_STR, 
	ToExternId, 
	&Destructor<WriterWrapper>
)

}	// namespace writer

/////////////////////////////////////////////////////

namespace writer {

DLIB_FUNC_START(writer_fromfile, 1, Nil)
DLIB_GET_FILE(fp)
writer::ToExternId(
	DLIB_RETVAL_PTR, 
	DNEWCLASS(
		WriterWrapper, 
		(DNEWCLASS(PortableBinFileWriter, (fp)), DNEWCLASS(DeltaValue, (DLIB_ARG_OBJECT(fp_val))))
	)
);
DLIB_FUNC_END
 
DLIB_FUNC_START(writer_fromoutputbuffer, 1, Nil)
DLIB_GET_OUTPUTBUFFER
writer::ToExternId(
	DLIB_RETVAL_PTR, 
	DNEWCLASS(
		WriterWrapper, 
		(DNEWCLASS(PortableBufferWriter, (*DPTR(ob))), DNEWCLASS(DeltaValue, (DLIB_ARG_OBJECT(ob_val))))
	)
);
DLIB_FUNC_END

#define	WRITE_SIMPLE(suffix, arg, cast)				\
DLIB_FUNC_START(writer_write_##suffix, 2, Empty)	\
DLIB_GET_WRITER										\
DLIB_ARG(arg, val);									\
writer->write((cast) DLIB_ARGVAL(val));				\
DLIB_FUNC_END

WRITE_SIMPLE(ui8,		util_ui32,		util_ui8)
WRITE_SIMPLE(ui16,		util_ui32,		util_ui16)
WRITE_SIMPLE(i16,		util_i32,		util_i16)
WRITE_SIMPLE(ui32,		util_ui32,		util_ui32)
WRITE_SIMPLE(i32,		util_i32,		util_i32)
WRITE_SIMPLE(real64,	double,			double)
WRITE_SIMPLE(bool8,		bool,			bool)
WRITE_SIMPLE(string,	const char*,	std::string)

DLIB_FUNC_START(writer_write_buffer, 2, Nil)
DLIB_GET_WRITER
DLIB_ARGEX_BASECLASS(buffer);
util_ui32 size = 0;
util_ui8* data = (util_ui8*) 0;

if (DLIB_ARGEX_TYPESTR(buffer) == OUTPUTBUFFER_TYPE_STR) {
	ubinaryio::OutputBuffer* ob = DLIB_EXTRACT_COLLECTABLESIMPLE_ARG_VALUE<ubinaryio::OutputBuffer>(buffer, OUTPUTBUFFER_TYPE_STR);
	data = (util_ui8*) ob->MakeBuffer(&size);
}
else
if (DLIB_ARGEX_TYPESTR(buffer) == INPUTBUFFER_TYPE_STR) {
	ubinaryio::InputBuffer* ib = DLIB_EXTRACT_COLLECTABLESIMPLE_ARG_VALUE<ubinaryio::InputBuffer>(buffer, INPUTBUFFER_TYPE_STR);
	data = (util_ui8*) ib->MakeRemainingBuffer(&size);
}
else
	DLIB_ERROR_REPORT(OUTPUTBUFFER_TYPE_STR " or " INPUTBUFFER_TYPE_STR " expected");

if (size) 
	{ DASSERT(data); DPTR(writer)->write(data, size); DDELARR(data); }
DLIB_FUNC_END

}	// namespace

//---------------------------------------------------

void Install_DeltaReaderWriter_Lib (DeltaObject* mainAPI) {
	reader::Install_DeltaReader_Lib(mainAPI); 
	writer::Install_DeltaWriter_Lib(mainAPI); 
}

void CleanUp_DeltaReaderWriter_Lib (void) {
	reader::CleanUp_DeltaReader_Lib(); 
	writer::CleanUp_DeltaWriter_Lib(); 
}

//---------------------------------------------------
