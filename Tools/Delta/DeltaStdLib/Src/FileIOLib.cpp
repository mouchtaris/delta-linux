// FileIOLib.cpp.
// File I/O library functions for the DeltaVirtualMachine.
// ScriptFighter Project.
// A. Savidis, December 1999.
// Refactored December 2007, encompassed in the 'std' API object. 
//

#include <stdlib.h>
#include <math.h>

#ifdef _MSC_VER
#include <process.h>
#endif

#include "DeltaLibraryCreators.h"
#include "FileIOLib.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h> 
#include <functional>
#include <algorithm>
#include <map>

VCLASSID_IMPL(FILE_CID, FILE_TYPE_STR)
typedef ValidatableValue<FILE, FILE_CID> ValidatableFILE;

//------------------------------------------------------------------

static void fileopen_LibFunc (DeltaVirtualMachine*);
static void fileclose_LibFunc (DeltaVirtualMachine*);
static void fileremove_LibFunc (DeltaVirtualMachine*);
static void filewrite_LibFunc (DeltaVirtualMachine*);
static void fileread_LibFunc (DeltaVirtualMachine*);
static void filegetline_LibFunc (DeltaVirtualMachine*);
static void fileend_LibFunc (DeltaVirtualMachine*);
static void filetmpname_LibFunc (DeltaVirtualMachine*);
static void fileexecute_LibFunc (DeltaVirtualMachine*);

//------------------------------------------------------------------

static DeltaLibraryObjectCreator::FuncEntry funcs[] = {

	{ "open",		fileopen_LibFunc },
	{ "remove",		fileremove_LibFunc },
	{ "tmpname",	filetmpname_LibFunc },
	{ "execute",	fileexecute_LibFunc },

	{ "close",		fileclose_LibFunc },
	{ "write",		filewrite_LibFunc },
	{ "read",		fileread_LibFunc },
	{ "getline",	filegetline_LibFunc },
	{ "end",		fileend_LibFunc }
};

DELTALIBFUNC_DECLARECONSTS(4, uarraysize(funcs) - 4, "close", "end")

inline bool VAL_IS_FILE (const DeltaValue& val)
	{ return val.Type() == DeltaValue_ExternId && val.GetExternIdTypeString() == FILE_TYPE_STR; }

DELTALIBFUNC_DEFINE_METHODS_TABLE_AND_CALLER_CHECKER(VAL_IS_FILE(val), FILE_TYPE_STR)

////////////////////////////////////////////////////////////////////

typedef std::map<FILE*, std::string> FileMap;
static FileMap*	allFiles = (FileMap*) 0; 

static void Initialise (void) 
	{ unew(allFiles); DELTALIBFUNC_CREATE_METHODS_TABLE(); }

static void CleanUp (void) {
	std::for_each(
		DPTR(allFiles)->begin(),
		DPTR(allFiles)->end(),
		utuple_firstfunctoriser< std::pair<FILE*, std::string> >(uptr_fun(&fclose))
	);
	udelete(allFiles);
	DELTALIBFUNC_DESTROY_METHODS_TABLE();
}

DELTA_LIBRARY_SUBAPI_INSTALLER_EX(DeltaFile, "file", DELTA_STDLIB_NAMESPACE_PREFIX "file", Initialise(), CleanUp())

//------------------------------------------------------------------

#define	DELTA_RT	"rt"	// Read as text.
#define	DELTA_WT	"wt"	// Write as text.
#define	DELTA_AT	"at"	// Append as text.
#define	DELTA_RB	"rb"	// Read as binary.
#define	DELTA_WB	"wb"	// Write as binary.
#define	DELTA_AB	"ab"	// Append as binary.

static const char* fileOpenModes[] = {
	DELTA_RT,
	DELTA_WT,
	DELTA_AT,
	DELTA_RB,
	DELTA_WB,
	DELTA_AB
};

////////////////////////////////////////////////////////////////////

static bool DeltaLegalOpenMode (const std::string& mode) {
	for (util_ui8 i = 0; i < uarraysize(fileOpenModes); ++i)
		if (mode == fileOpenModes[i])
			return true;
	return false;
}

////////////////////////////////////////////////////////////////////
// Use first character for open class, and second for mode.
//
static FILE* DeltaOpenFile (const char* file, const char* mode) {
	char _mode[2] = { mode[0], '\0' };
	FILE* fp = (mode[1] == 'b') ? ubinaryfileopen(file, _mode) : fopen(file, _mode);
	return fp;
}

////////////////////////////////////////////////////////////////////

static void FILE2String (DeltaString* at, void* val) {
	
	ValidatableFILE* vfp = VGET_VALUE(FILE, FILE_CID, val);

	if (!vfp)
		DPTR(at)->Add(
			uconstructstr("%s(handle 0x%x, closed!)", FILE_TYPE_STR, (util_ui32) val)
		);
	else {
		FileMap::iterator fileIter = DPTR(allFiles)->find(DPTR(vfp)->GetValue());
		DASSERT(fileIter != DPTR(allFiles)->end());
		DPTR(at)->Add(
			uconstructstr(
				"%s(stream 0x%x, path '%s')", 
				FILE_TYPE_STR,
				(util_ui32) DPTR(vfp)->GetSerialNo(),
				fileIter->second.c_str()
			)
		);
	}
}

////////////////////////////////////////////////////////////////////

DVM_FUNC FILE*	DeltaFile_Get (void* val, const std::string& typeStr) {
	if (FILE_TYPE_STR != typeStr)
		return (FILE*) 0;
	else {
		ValidatableFILE* vfp = VGET_VALUE(FILE, FILE_CID, val);
		if (!vfp)
			return (FILE*) 0;
		else
			return vfp->GetValue();
	}
}

DVM_FUNC FILE* DeltaFile_Get (DeltaValue& obj) {
	if (obj.Type() != DeltaValue_ExternId)
		return (FILE*) 0;
	else {
		std::string	typeStr;
		void* val = obj.ToExternId(typeStr);
		return DeltaFile_Get(val, typeStr);
	}	
}

////////////////////////////////////////////////////////////////////
// ExternId fileopen(f,m).
// f should be a string var, carrying the file name.
// m should be a string var, carrying the open mode, being one of
// the delta open modes defined above.
//
void fileopen_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("fileopen");
	DeltaTotalArgsCheck(2, CURR_FUNC, RESET_NIL_RETURNVALUE);

	DeltaValue* argObj;
	GET_STRING(file, CURR_FUNC, RESET_NIL_RETURNVALUE, 0);
	GET_STRING(mode, CURR_FUNC, RESET_NIL_RETURNVALUE, 1);

	if (!DeltaLegalOpenMode(mode.c_str())) 
		{ RESET_NIL_RETURNVALUE; }

	FILE* fp = DeltaOpenFile(file.c_str(), mode.c_str());
	if (!fp)
		{ RESET_NIL_RETURNVALUE; }

	(*DPTR(allFiles))[fp] =  file;
	ValidatableFILE* vfp = DNEWCLASS(ValidatableFILE, (fp));

	DPTR(vm)->GetReturnValue().FromExternId(
		(void*) DPTR(vfp)->GetSerialNo(),	// Handle.
		DeltaExternId_NonCollectable,
		FILE2String,
		FILE_TYPE_STR,
		DELTA_EXTERNID_NO_FIELD_GETTER
	);

	DELTALIBFUNC_DELEGATE_METHODS(DPTR(vm)->GetReturnValue());
}

//------------------------------------------------------------------
// Local variables declared are: fileVal, fileValTypeStr, fp.
//
#define	GET_FILE(argno, retvalreset, func)										\
	DeltaValue* argObj;															\
	GET_POLYMORPHIC_EXTERNID(fileVal, FILE_TYPE_STR, retvalreset, argno);		\
	ValidatableFILE* vfp = VGET_VALUE(FILE, FILE_CID, fileVal);					\
	if (!vfp) {																	\
		DPTR(vm)->PrimaryError(													\
			"invalid file handle 0x%x, probably closed!",						\
			(util_ui32) fileVal													\
		);																		\
		retvalreset;															\
	}																			\
	FILE* fp = vfp->GetValue();													\
	DASSERT(allFiles);															\
	FileMap::iterator fileIter = DPTR(allFiles)->find(fp);						\
	DASSERT(fileIter != DPTR(allFiles)->end());

//------------------------------------------------------------------
// fileclose(f).
// f is a FILE* carried as an externid type.
//
void fileclose_LibFunc (DeltaVirtualMachine* vm) { 

	ISSUE_FUNC("fileclose");
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_EMPTY);
	GET_FILE(0, RESET_EMPTY, CURR_FUNC);

	if (fclose(fp) != 0)
		DPTR(vm)->PrimaryError(
			"File stream 0x%x for path '%s' to 'fileclose' could not be closed!",
			(util_ui32) fp,
			fileIter->second.c_str()
		);
	
	DDELETE(vfp);
	DPTR(allFiles)->erase(fileIter);
}

//	Alternative style when the extern id is a reference to a base class.
#if 0
	DLIB_ARGEX_BASECLASS(file)															\
	DLIB_ERROR_CHECK(																	\
		DLIB_ARGEX_TYPESTR(file) != FILE_TYPE_STR,										\
		uconstructstr(																	\
			"call (%s): f is '%s' not '%s'",											\
			DLIB_CALLSIG_CROPPED, DLIB_ARGEX_TYPESTR(file), FILE_TYPE_STR				\
		)																				\
	)																					\

#endif

#if 0	// It is an example of a different implementation style.

#define	GET_FILE2																		\
	DLIB_GET_FILE(fp);																	\
	DASSERT(allFiles);																	\
	FileMap::iterator fileIter = DPTR(allFiles)->find(fp);								\
	DASSERT(fileIter != DPTR(allFiles)->end());

DLIB_FUNC_START(fileclose2, 1, Empty)
GET_FILE2
DLIB_ERROR_CHECK(
	fclose(fp), 
	uconstructstr(
		"file handle 0x%x, path '%s', could not be closed!",
		(util_ui32) DLIB_ARGVAL(fp_val),
		fileIter->second.c_str()
	)
)
DDELETE(fp_wrap);
DPTR(allFiles)->erase(fileIter);
DLIB_FUNC_END

#endif

//------------------------------------------------------------------
// filewrite(f,v1,...,vn).
// Textual write to file 'f' of values 'v1',...,'vn'
// 'v' is allowed to be any Delta object.
//
void filewrite_LibFunc (DeltaVirtualMachine* vm) { 

	ISSUE_FUNC("filewrite");
	DeltaAtLeastTotalArgsCheck(2, CURR_FUNC, RESET_EMPTY);
	GET_FILE(0, RESET_EMPTY, CURR_FUNC);

	for (util_ui32 argNo = 1; argNo < vm->TotalActualArgs(); ++argNo) {

		DeltaValue*	obj		= vm->GetActualArg(argNo);
		std::string	objStr	= obj->ConvertToString().c_str();
		util_i32	n		= fprintf(fp, "%s", objStr.c_str());
		if ( n >= 0 && ((size_t)n) != objStr.length())
			DPTR(vm)->PrimaryError(
				"'filewrite', stream 0x%x apth '%s' failed, %d/%d bytes written: arg #%d, value '%s'!",
				(util_ui32) fp,
				fileIter->second.c_str(),
				n,
				objStr.length(),
				argNo,
				objStr.c_str()
			);
	}
}

//------------------------------------------------------------------

#define	READ_STRING		"string"
#define	READ_NUMBER		"number"
#define	READ_BOOL		"bool"

static const char* readTypes[]={
	READ_STRING,
	READ_NUMBER,
	READ_BOOL
};

////////////////////////////////////////////////////////////////////

#define	TOTAL_READ_TYPES \
	sizeof(readTypes)/sizeof(char*)

#define	LEGAL_READ_TYPE(type) \
	(ustrpos(readTypes, TOTAL_READ_TYPES, type) != TOTAL_READ_TYPES)

////////////////////////////////////////////////////////////////////
	
static void DeltaFileRead (FILE* fp, const char* type, const char* path, DeltaVirtualMachine* vm) {

	static char buffer[1024];
	vm->GetReturnValue().FromNil();

	if (fscanf(fp, "%s", buffer) != 1)
		return;

	DASSERT(strlen(buffer) < sizeof(buffer));

	if (!strcmp(type, READ_STRING))
		vm->GetReturnValue().FromString(buffer);
	else	
	if (!strcmp(type, READ_NUMBER))
		vm->GetReturnValue().FromNumber(atof(buffer));
	else
	if (!strcmp(type, READ_BOOL)) {
		ustrupper(buffer);
		if (!strcmp("TRUE", buffer))
			vm->GetReturnValue().FromBool(true);
		else
		if (!strcmp("FALSE", buffer))
			vm->GetReturnValue().FromBool(false);
	}
	else
		DASSERT(false);	// Should have been checked already by caller.
}

////////////////////////////////////////////////////////////////////
// fileread(f, s).
// Read next item from f, of type indicated by 's'.
// 's' should be any of the types allowed in fileread.
// If failure on reading, Nil is returned.
//
void fileread_LibFunc (DeltaVirtualMachine* vm) { 

	ISSUE_FUNC("fileread");
	DeltaTotalArgsCheck(2, CURR_FUNC, RESET_NIL_RETURNVALUE);

	GET_FILE(0, RESET_EMPTY, CURR_FUNC);
	GET_STRING(readType, CURR_FUNC, RESET_EMPTY, 1);

	if (!LEGAL_READ_TYPE(readType.c_str())) {
		DPTR(vm)->PrimaryError(
			"in '%s', stream 0x%x, path '%s', illegal read type '%s'!",
			CURR_FUNC,
			(util_ui32) fp,
			fileIter->second.c_str(),
			readType.c_str()
		);

		RESET_NIL_RETURNVALUE;
	}

	DeltaFileRead(
		fp, 
		readType.c_str(), 
		fileIter->second.c_str(), 
		vm
	);
}

//------------------------------------------------------------------
// filegetline(f, s).
// Reads a line from a file.
// On failure to read, nil is returned.
//
void filegetline_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("filegetline");
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_NIL_RETURNVALUE);
	GET_FILE(0, RESET_NIL_RETURNVALUE, CURR_FUNC);

	std::string line;
	while (!feof(fp)) {
		char c = getc(fp);
		if (c != '\n')
			line += c;
		else
			break;
	}

	DPTR(vm)->GetReturnValue().FromString(line);
}

//------------------------------------------------------------------
// fileend(f).
// Returns a boolean, true, if file end reached,
// else false.
//
void fileend_LibFunc (DeltaVirtualMachine* vm) { 

	ISSUE_FUNC("fileend");
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_TRUE_RETURNVALUE);
	GET_FILE(0, RESET_TRUE_RETURNVALUE, CURR_FUNC);

	vm->GetReturnValue().FromBool(!!feof(fp));
}

//------------------------------------------------------------------
// Bool fileremove(filePath).

DLIB_FUNC_START(fileremove, 1, False)
DLIB_ARG(const char*, filePath)
DLIB_RETVAL_REF.FromBool(remove(DLIB_ARGVAL(filePath)) != -1);
DLIB_FUNC_END

//------------------------------------------------------------------
// String filetmpname().

DLIB_FUNC_START(filetmpname, 0, Nil)
DLIB_RETVAL_REF.FromString(tmpnam((char*) 0));
DLIB_FUNC_END

//------------------------------------------------------------------
// fileexecute(s).

DLIB_FUNC_START(fileexecute, 1, Empty)
DLIB_ARG(const char*, cmmd)
system(DLIB_ARGVAL(cmmd));
DLIB_FUNC_END

//-------------------------------------------------------------------
