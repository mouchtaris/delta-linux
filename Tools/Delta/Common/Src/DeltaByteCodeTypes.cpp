// DeltaByteCodeTypes.cpp
// Some common facilities for manipulattion
// of Delta types.
// ScriptFighter Project.
// A. Savidis, November 1999.
// Last update, February 2005, changing debug information layout.
//

#include "DDebug.h"
#include "DeltaByteCodeTypes.h"
#include "DeltaVersionDefs.h"
#include "DeltaFunc.h"
#include "ustrings.h"
#include "uerrorclass.h"
#include "ufiles.h"
#include <algorithm>
#include <new>

#define	ERROR_HANDLER(what, errclass)							\
	uerror::GetSingleton().post##errclass(						\
		"Loading %s: error in reading '%s'!", CURR_TYPE, what	\
	); goto FAIL;

//-----------------------------------------------------------

DeltaFunctionReturnLibraryType* DeltaFunctionReturnLibraryType::Clone (void) const {
	DeltaFunctionReturnLibraryType* type = DNEWCLASS(DeltaFunctionReturnLibraryType, (fullPath));
	for (BaseList::const_iterator i = baseTypes.begin(); i != baseTypes.end(); ++i)
		type->AddBaseType((*i)->Clone());
	return type;
}

/////////////////////////////////////////////////////////////

void  DeltaFunctionReturnLibraryType::WriteText (FILE* fp) const {
	DASSERT(!fullPath.empty()); 
	fprintf(fp, "<%s>", fullPath.c_str());
	if (!baseTypes.empty()) {
		fputs(":{", fp);
		std::for_each(
			baseTypes.begin(), 
			baseTypes.end(),
			std::bind2nd(std::mem_fun(&DeltaFunctionReturnLibraryType::WriteText), fp)
		);
		fputs("}", fp);
	}
}

/////////////////////////////////////////////////////////////

#define	CURR_TYPE "return library type"

bool DeltaFunctionReturnLibraryType::Read (GenericReader& reader) {

	UCHECK_PRIMARY_ERROR(reader.read(fullPath, false), "full path");

	util_ui16 total;
	UCHECK_PRIMARY_ERROR(reader.read(&total), "total base library types");

	for (util_ui16 i = 0; i <total; ++i) {
		DeltaFunctionReturnLibraryType* base = DNEW(DeltaFunctionReturnLibraryType);
		AddBaseType(base);
		UCHECK_DOMINO_ERROR(DPTR(base)->Read(reader), uconstructstr("base library type #%u", i));
	}

	return true;
	FAIL: return false;
}

/////////////////////////////////////////////////////////////

void DeltaFunctionReturnLibraryType::Write (GenericWriter& writer) const {
	DASSERT(!fullPath.empty()); 
	writer.write(fullPath);
	writer.write((util_ui16) baseTypes.size());
	for (BaseList::const_iterator i = baseTypes.begin(); i != baseTypes.end(); ++i)
		DPTR(*i)->Write(writer);
}

/////////////////////////////////////////////////////////////

static const char* basicTypeStrings[] = {
	"Any",							// AnyValue		
	"...",							// AnyValues	
	"ExternId",						// EternId	
	DELTA_OBJECT_TYPESTRING,		// Object		
	DELTA_BOOL_TYPESTRING,			// Bool			
	DELTA_NUMBER_TYPESTRING,		// Number		
	DELTA_STRING_TYPESTRING,		// String		
	DELTA_PROGRAMFUNC_TYPESTRING,	// Function
	DELTA_METHODFUNC_TYPESTRING,	// Method		
	DELTA_LIBRARYFUNC_TYPESTRING,	// LibFunction	
	"Callable",						// Callable
	"Void",							// Void
	DELTA_UNDEFINED_TYPESTRING,		// Undefined
	DELTA_NIL_TYPESTRING			// Nil
};

/////////////////////////////////////////////////////////////

DeltaFunctionReturnType::DeltaFunctionReturnType (const DeltaFunctionReturnType& r) {
	if (r.IsBasicType())
		new(this) DeltaFunctionReturnType(r.GetBasicType());
	else
		new(this) DeltaFunctionReturnType(DPTR(r.GetLibraryType())->Clone());
}

/////////////////////////////////////////////////////////////

const char*	DeltaFunctionReturnType::GetBasicTypeString (DeltaFunctionReturnBasicType type) {
	DASSERT(type <= DeltaFunctionReturnNil);
	return basicTypeStrings[type];
}

bool DeltaFunctionReturnType::GetBasicTypeFromString (const std::string& typeId, DeltaFunctionReturnBasicType* type) {
	util_ui32 n = uarraysize(basicTypeStrings);
	util_ui32 i = ustrpos(basicTypeStrings, n, typeId.c_str());
	if (i == n) 
		return false;
	else {
		*type = (DeltaFunctionReturnBasicType) i;
		return true;
	}
}

/////////////////////////////////////////////////////////////

void DeltaFunctionReturnType::WriteText (FILE* fp) const {
	DASSERT(metaType != None);
	if (IsBasicType())
		fputs(GetBasicTypeString(data.basicType), fp);
	else
		GetLibraryType()->WriteText(fp);
	fputs(".", fp);
}

/////////////////////////////////////////////////////////////

#undef	CURR_TYPE 
#define	CURR_TYPE "return type"

bool DeltaFunctionReturnType::Read (GenericReader& reader) {

	util_ui16 val;
	UCHECK_PRIMARY_ERROR(reader.read(&val), "meta type");
	metaType = (MetaType) val;
	UCHECK_PRIMARY_ERROR(IsBasicType() || IsLibraryType(), "meta type (invalid value)");

	if (IsBasicType()) {
		UCHECK_PRIMARY_ERROR(reader.read(&val), "basic type");
		data.basicType = (DeltaFunctionReturnBasicType) val;
		UCHECK_PRIMARY_ERROR(GetBasicType() <= DeltaFunctionReturnNil, "basic type (invalid value)");
	}
	else {
		DeltaFunctionReturnLibraryType* type = DNEW(DeltaFunctionReturnLibraryType);
		data.libType = type;
		UCHECK_DOMINO_ERROR(DPTR(type)->Read(reader), "library type");
	}

	return true;
	FAIL: return false;
}

/////////////////////////////////////////////////////////////

void DeltaFunctionReturnType::Write (GenericWriter& writer) const {
	DASSERT(metaType != None);
	writer.write((util_ui16) metaType);
	if (IsBasicType())
		writer.write((util_ui16) GetBasicType());
	else
		DNULLCHECK(GetLibraryType())->Write(writer);
}

/////////////////////////////////////////////////////////////

void DeltaFunctionReturnTypes::WriteText (FILE* fp) const {
	if (!retTypes.empty()) {
		fputs("RETURN TYPES:", fp);
		std::for_each(
				retTypes.begin(), 
				retTypes.end(),
				std::bind2nd(std::mem_fun(&DeltaFunctionReturnType::WriteText), fp)
			);
		fputs("\n", fp);
	}
}

/////////////////////////////////////////////////////////////

#undef	CURR_TYPE 
#define	CURR_TYPE "return types"

bool DeltaFunctionReturnTypes::Read (GenericReader& reader) {
	util_ui16 total;
	UCHECK_PRIMARY_ERROR(reader.read(&total), "total return types");
	for (util_ui16 i = 0; i < total; ++i) {
		DeltaFunctionReturnType* type = DNEW(DeltaFunctionReturnType);
		UCHECK_DOMINO_ERROR(DPTR(type)->Read(reader),  uconstructstr("return type #%u", i));
		retTypes.push_back(type);
	}

	return true;
	FAIL: return false;
}

/////////////////////////////////////////////////////////////

DeltaFunctionReturnTypes::DeltaFunctionReturnTypes (const DeltaFunctionReturnTypes& r) {
	for (TypeList::const_iterator i = r.retTypes.begin(); i != r.retTypes.end(); ++i)
		retTypes.push_back(DNEWCLASS(DeltaFunctionReturnType, (**i)));
}

/////////////////////////////////////////////////////////////

void DeltaFunctionReturnTypes::Write (GenericWriter& writer) const {
	writer.write((util_ui16) retTypes.size());
	for (TypeList::const_iterator i = retTypes.begin(); i != retTypes.end(); ++i)
		DPTR(*i)->Write(writer);
}

//-----------------------------------------------------------

void DeltaStdFuncInfo::Write (GenericWriter& writer) const{

	writer.write(name);
	writer.write(isAnonymous);
	writer.write(isExported);
	writer.write(isMethod);
	writer.write(isGlobal);
	writer.write(addr); 
	writer.write(hasVarArgs); 

	// Arguments.
	writer.write(GetTotalFormals()); 
	for (	std::list<std::string>::const_iterator i = arguments.begin(); 
			i != arguments.end(); 
			++i	)
			writer.write(*i);

	// Closure.
	writer.write((util_ui16) closureVars.size());
	for (ClosureVarsInfo::const_iterator i = closureVars.begin();
				i != closureVars.end();
				++i	) {
		writer.write(i->GetOffset());
		writer.write((util_ui32) i->GetType());
		writer.write(i->GetName());
		writer.write(i->GetLine());
	}

	writer.write(serial); 

	if (IsGlobal() && IsNamedFunction())
		retTypes.Write(writer);
}

/////////////////////////////////////

static const char* closureVarTypes[] = { "local", "formal", "global", "closure" };

void DeltaStdFuncInfo::WriteText (FILE* fp) const {

	std::string args;
	for (	std::list<std::string>::const_iterator i = arguments.begin();
			i != arguments.end();
			++i	)
			args += (i == arguments.begin() ? "" : ",") + *i;
	if (HasVarArgs())
		args +=  "...";

	fprintf(
		fp,
		"%s %s %s '%s'(%s), ADDR %u, FORMALS %u, SERIAL %u.\n",
		isGlobal	? "GLOBAL"		: "NON-GLOBAL",
		isMethod	? "METHOD"		: "FUNCTION",
		isExported	? "EXPORTED"	: "NON-EXPORTED",
		name.c_str(), 
		args.c_str(),
		addr, 
		GetTotalFormals(),
		serial
	);

	if (HasClosureVars()) {
		args.clear();
		for (	ClosureVarsInfo::const_iterator i = closureVars.begin();
				i != closureVars.end();
				++i	)
				args += uconstructstr(
							"%s%s:%s(offset %u, line %u)", 
							i == closureVars.begin() ? "" : ",",
							i->GetName().c_str(),
							closureVarTypes[(util_ui32) i->GetType()],
							i->GetOffset(),
							i->GetLine()
						);
		fprintf(fp, "CLOSUREVARS[%s]\n", args.c_str());
	}

	if (IsGlobal() && IsNamedFunction())
		retTypes.WriteText(fp);
}

/////////////////////////////////////

bool DeltaStdFuncInfo::Read(GenericReader& reader) {

	DASSERT(arguments.empty());

	UCHECK_PRIMARY_ERROR(reader.read(name, false),					"name");
	UCHECK_PRIMARY_ERROR(reader.read(&isAnonymous),					"is anonymous");
	UCHECK_PRIMARY_ERROR(reader.read(&isExported),					"is exported");
	UCHECK_PRIMARY_ERROR(reader.read(&isMethod),					"is method");
	UCHECK_PRIMARY_ERROR(reader.read(&isGlobal),					"is global");
	UCHECK_PRIMARY_ERROR(reader.read(&addr),						"address");
	UCHECK_PRIMARY_ERROR(reader.read(&hasVarArgs),					"has var args");

	util_ui16 totalArguments;
	UCHECK_PRIMARY_ERROR(reader.read(&totalArguments),				"total formals");

	for (util_ui16 i = 0; i < totalArguments; ++i) {
		std::string name;
		UCHECK_PRIMARY_ERROR(reader.read(name, false), uconstructstr("formal arg '%u' name", i));
		arguments.push_back(name);
	}

	util_ui16 n;
	UCHECK_PRIMARY_ERROR(reader.read(&n), "total closure vars");
	for (util_ui16 i = 0; i < n; ++i) {
		util_ui16	offset;
		util_ui32	type;
		std::string	name;
		util_ui32	line;
		UCHECK_PRIMARY_ERROR(reader.read(&offset), uconstructstr("closure var #%u offset", i));
		UCHECK_PRIMARY_ERROR(reader.read(&type), uconstructstr("closure var #%u type", i));
		UCHECK_PRIMARY_ERROR(DeltaClosureVarInfo::IsValidOperandType((DeltaClosureVarInfo::VarType) type), "closure var type (invalid value)");
		UCHECK_PRIMARY_ERROR(reader.read(name, false), uconstructstr("closure var #%u name", i));
		UCHECK_PRIMARY_ERROR(!name.empty(), uconstructstr("closure var #%u name (name empty)", i));
		UCHECK_PRIMARY_ERROR(reader.read(&line), uconstructstr("closure var #%u line", i));
		UCHECK_PRIMARY_ERROR(line != DELTA_CANTBE_A_SOURCE_LINE, uconstructstr("closure var #%u line (line is zero)", i));
		closureVars.push_back(DeltaClosureVarInfo(offset, (DeltaClosureVarInfo::VarType) type, name, line));
	}

	UCHECK_PRIMARY_ERROR(reader.read(&serial), "serial number");

	if (IsGlobal() && IsNamedFunction()) {
		UCHECK_DOMINO_ERROR(retTypes.Read(reader), "return types");
	}

	return true;
	FAIL: return false;
}

/////////////////////////////////////

#undef	CURR_TYPE
#define	CURR_TYPE	"function table"

DeltaStdFuncInfo* DeltaStdFuncInfo::ReadFunctionTable (GenericReader& reader, util_ui16* total) {

	DeltaStdFuncInfo* funcTable = (DeltaStdFuncInfo*) 0;
	UCHECK_PRIMARY_ERROR(reader.read(total), "total functions");

	if (*total) {
		DeltaStdFuncInfo* p = funcTable = DNEWARR(DeltaStdFuncInfo, *total);
		for (util_ui16 i = 0; i < *total; ++i, ++p)
			UCHECK_DOMINO_ERROR(p->Read(reader), uconstructstr("function #%u", i));
	}

	return funcTable;
	FAIL: if (funcTable) DDELARR(funcTable); return (DeltaStdFuncInfo*) 0;
}

///-----------------------------------------------------------

DeltaDebugProgramInfo::DeltaDebugProgramInfo (void) {
	totalFuncs = 0;
	unullify(funcs);
	isDynamicCode = false;
}

void DeltaDebugProgramInfo::SetDynamicCode (const std::string& src) {
	isDynamicCode = true;
	dynamicSource = src;
}

/////////////////////////////////////

void DeltaDebugProgramInfo::Clear (void) {

	globals.Clear();
	openedNamespaces.clear();
	numLibraryConsts.clear();
	strLibraryConsts.clear();
	
	if (funcs) {
		DASSERT(totalFuncs);
		DDELARR(funcs);
		funcMap.clear();
		unullify(funcs);
		totalFuncs = 0;
	}
	else
		DASSERT(funcMap.empty() && !funcs);

	lines.Clear();
	chunks.Clear();
	calls.Clear();

	isDynamicCode = false;
}

/////////////////////////////////////

void DeltaDebugProgramInfo::SetTotalFuncs (util_ui16 _totalFuncs) {
	if ((totalFuncs = _totalFuncs))	// Intentional assignment.
		funcs = DNEWARR(DeltaDebugFuncInfo, totalFuncs);
	else
		funcs = (DeltaDebugFuncInfo*) 0;
}

/////////////////////////////////////

DeltaDebugFuncInfo* DeltaDebugProgramInfo::GetFunc (util_ui16 serial) {
	DASSERT(serial < totalFuncs);
	return funcs + serial;
}

const DeltaDebugFuncInfo& DeltaDebugProgramInfo::GetFunc (util_ui16 serial) const {
	DASSERT(serial < totalFuncs);
	return funcs[serial];
}

DeltaDebugFuncInfo* DeltaDebugProgramInfo::GetFuncByAddr (DeltaCodeAddress addr) {
	FuncMap::iterator i = funcMap.find(addr);
	return i != funcMap.end() ? i->second : (DeltaDebugFuncInfo*) 0;
}

const DeltaDebugFuncInfo* DeltaDebugProgramInfo::GetFuncByAddr (DeltaCodeAddress addr) const {
	FuncMap::const_iterator i = funcMap.find(addr);
	return i != funcMap.end() ? i->second : (DeltaDebugFuncInfo*) 0;
}

/////////////////////////////////////

bool DeltaDebugProgramInfo::GetUsedLibraryConst (const std::string& name, DeltaNumberValueType* val) const {
	NumConsts::const_iterator i = numLibraryConsts.find(name);
	if (i != numLibraryConsts.end())
		{ *val = i->second; return true; }
	else
		return false;
}

bool DeltaDebugProgramInfo::GetUsedLibraryConst (const std::string& name, std::string* val) const {
	StrConsts::const_iterator i = strLibraryConsts.find(name);
	if (i != strLibraryConsts.end())
		{ *val = i->second; return true; }
	else
		return false;
}

/////////////////////////////////////
// Layout:
// |Globals|Opened Namespaces|Used Library Consts|
// |Total Funcs|Funcs|
// |Lines|Chunks|
// |Dynamic Source|
// |Calls|
//

static void WriteStringList (GenericWriter& writer, const std::list<std::string>& l) {
	writer.write((util_ui16) l.size());
	for (std::list<std::string>::const_iterator i = l.begin(); i != l.end(); ++i)
		writer.write(*i);
}

template <class Tval> static void WriteLibConsts (
		GenericWriter&						writer, 
		const std::map<std::string, Tval>&	m
	) {
	writer.write((util_ui16) m.size());
	for (typename std::map<std::string, Tval>::const_iterator i = m.begin(); i != m.end(); ++i)
		{ writer.write(i->first); writer.write(i->second); }		
}

/////////////////////////////////////

void DeltaDebugProgramInfo::Write (GenericWriter& writer) const {

	globals.Write(writer);
	WriteStringList(writer, openedNamespaces);
	WriteStringList(writer, byteCodeLibs);
	WriteLibConsts(writer, numLibraryConsts);
	WriteLibConsts(writer, strLibraryConsts);
	writer.write(totalFuncs);
	for (util_ui16 i = 0; i < totalFuncs; ++i)
		funcs[i].Write(writer);

	// Those two are never writen in text mode.
	lines.Write(writer);
	chunks.Write(writer);
	writer.write(IsDynamicCode());
	if (IsDynamicCode())
		writer.write(GetDynamicCode());
	calls.Write(writer);
}

/////////////////////////////////////

void DeltaDebugProgramInfo::WriteText (FILE* fp) const {

	fprintf(fp,"\n***GLOBALS***\n");
	GetGlobals().WriteText(fp);

	if (!openedNamespaces.empty()) {
		fprintf(fp,"\n***OPENED NAMESPACES***\n");
		for (NameList::const_iterator i = openedNamespaces.begin(); i != openedNamespaces.end(); ++i)
			fprintf(fp, "\t%s\n", i->c_str());
	}

	if (!byteCodeLibs.empty()) {
		fprintf(fp,"\n***USED BYTE CODE LIBRARIES***\n");
		for (NameList::const_iterator i = byteCodeLibs.begin(); i != byteCodeLibs.end(); ++i)
			fprintf(fp, "\t%s\n", i->c_str());
	}

	if (!numLibraryConsts.empty() || !strLibraryConsts.empty()) {
		fprintf(fp,"\n***USED LIBRARY CONSTS***\n");
		for (NumConsts::const_iterator i = numLibraryConsts.begin(); i != numLibraryConsts.end(); ++i)
			fprintf(fp, "\t%s = %f\n", i->first.c_str(), i->second);
		for (StrConsts::const_iterator i = strLibraryConsts.begin(); i != strLibraryConsts.end(); ++i)
			fprintf(fp, "\t%s = \"%s\"\n", i->first.c_str(), i->second.c_str());
	}

	if (GetTotalFuncs()) {
		fprintf(fp,"\n***FUNCTION DETAILS***\n");
		for (util_ui16 i = 0; i < GetTotalFuncs(); ++i)
			GetFunc(i).WriteText(fp);
	}

	if (calls.GetTotal()) {
		fprintf(fp,"\n***STMTS WITH MULTIPLE CALLS***\n");
		calls.WriteText(fp);
	}

	if (IsDynamicCode())
		fprintf(fp,"\n***DYNAMIC SOURCE***\n%s\n", GetDynamicCode().c_str());
}

/////////////////////////////////////

#undef	CURR_TYPE
#define	CURR_TYPE	"program debug info"

static bool ReadStringList (
		GenericReader&			reader, 
		std::list<std::string>& l, 
		const char*				totalDescr, 
		const char*				itemDescr
	) {
	util_ui16 total;
	UCHECK_PRIMARY_ERROR(reader.read(&total), totalDescr);

	for (util_ui16 i = 0; i < total; ++i) {
		std::string s;
		UCHECK_PRIMARY_ERROR(reader.read(s, false), uconstructstr("%s #%u", itemDescr, i)); 
		l.push_back(s);
	}
	
	return true;
	FAIL: return false;
}

/////////////////////////////////////

struct StrValReader {
	typedef std::string val_type;
	std::string s;
	std::string&			get (void) { return s; }
	bool					read (GenericReader& r) { return r.read(s, true); }
};

//***********************************

struct NumValReader {
	typedef DeltaNumberValueType val_type;
	DeltaNumberValueType n;
	DeltaNumberValueType&	get (void) { return n; }
	bool					read (GenericReader& r) { return r.read(&n); }
};

//***********************************

template <class Tvalreader> static bool ReadLibConsts (
		GenericReader&											reader, 
		std::map<std::string, typename Tvalreader::val_type>&	consts
	) {

	util_ui16 total;
	UCHECK_PRIMARY_ERROR(reader.read(&total), "total used library consts");

	for (util_ui16 i = 0; i < total; ++i) {
		
		std::string name;
		UCHECK_PRIMARY_ERROR(reader.read(name, false), uconstructstr("library const name #%u", i)); 

		Tvalreader val;
		UCHECK_PRIMARY_ERROR(val.read(reader), uconstructstr("library const value #%u", i)); 
		consts[name] = val.get();
	}
	
	return true;
	FAIL: return false;
}

/////////////////////////////////////

bool DeltaDebugProgramInfo::Read (GenericReader& reader) {

	UCHECK_DOMINO_ERROR(globals.Read(reader), "globals");
	UCHECK_DOMINO_ERROR(
		ReadStringList(reader, openedNamespaces, "total opened namespaces", "opened namespace"),
		"opened namespaces info"
	);
	UCHECK_DOMINO_ERROR(
		ReadStringList(reader, byteCodeLibs, "total bytecode library names", "bytecode library name"),
		"used bytecode libraries info"
	);
	UCHECK_DOMINO_ERROR(ReadLibConsts<NumValReader>(reader, numLibraryConsts), "used numeric library consts");
	UCHECK_DOMINO_ERROR(ReadLibConsts<StrValReader>(reader, strLibraryConsts), "used string library consts");
	UCHECK_PRIMARY_ERROR(reader.read(&totalFuncs), "total functions");

	if (totalFuncs) {
		funcs = DNEWARR(DeltaDebugFuncInfo, totalFuncs);
		for (util_ui16 i = 0; i < totalFuncs; ++i) {
			UCHECK_DOMINO_ERROR(
				funcs[i].Read(reader), 
				uconstructstr("function #%d", i)
			); 
			funcMap[funcs[i].GetAddress()] = funcs + i;
		}
	}

	UCHECK_DOMINO_ERROR(lines.Read(reader), "source line numbers");
	UCHECK_DOMINO_ERROR(chunks.Read(reader), "source line chunks");
	UCHECK_DOMINO_ERROR(reader.read(&isDynamicCode), "is dynamic source");

	if (IsDynamicCode()) {
		UCHECK_DOMINO_ERROR(reader.read(dynamicSource, false), "dynamic source");
	}

	UCHECK_DOMINO_ERROR(calls.Read(reader), "calls");

	return true;
	FAIL: Clear(); return false;
}

/////////////////////////////////////

util_ui16 DeltaDebugProgramInfo::GetNextLine (util_ui16 lineNo) const {
	const util_ui16* funcSerial = chunks.FindFuncSerialFromLine(lineNo);
	return lines.FindNextLine(lineNo, funcSerial ? *funcSerial : DELTA_FUNCSERIAL_OF_GLOBAL_CODE);
}

//-----------------------------------------------------------

void DeltaCodeLineCollection::Write (GenericWriter& writer) const {

	writer.write((util_ui16) lines.size());

	for (std::list<DeltaCodeLine>::const_iterator i = lines.begin(); i != lines.end(); ++i) {
		writer.write(i->line);
		writer.write(i->funcSerial);
	}
}

/////////////////////////////////////

void DeltaCodeLineCollection::Write (FILE *fp) {

	ufwrite((util_ui16) lines.size(), fp);

	for (	std::list<DeltaCodeLine>::iterator i = lines.begin();
			i != lines.end();
			++i	) {
				ufwrite(i->line, fp);
				ufwrite(i->funcSerial, fp);
			}
}

/////////////////////////////////////

#undef	CURR_TYPE
#define	CURR_TYPE	"code lines"

bool DeltaCodeLineCollection::Read (GenericReader& reader) {

	lines.clear();

	util_ui16 total;
	UCHECK_PRIMARY_ERROR(reader.read(&total), "total");

	while (total--) {

		util_ui16 line;
		UCHECK_PRIMARY_ERROR(reader.read(&line), "line");

		util_ui16 funcSerial;
		UCHECK_PRIMARY_ERROR(reader.read(&funcSerial), "func serial");

		lines.push_back(DeltaCodeLine(line, funcSerial));
	}

	return true;
	FAIL: Clear(); return false;
}

/////////////////////////////////////

bool DeltaCodeLineCollection::IsValidLine (util_ui16 lineNo) const {
	return	std::find_if(
				lines.begin(),
				lines.end(),
				std::bind2nd(
					DeltaCodeLineCollection::EqualPred(),
					lineNo
				)
			) !=  lines.end();
}
				
/////////////////////////////////////

util_ui16 DeltaCodeLineCollection::FindNextLine (util_ui16 lineNo, util_ui16 funcSerial) const {

	std::list<DeltaCodeLine>::const_iterator i =	std::find_if(
														lines.begin(),
														lines.end(),
														std::bind2nd(
															FindNextPred(), 
															DeltaCodeLine(lineNo, funcSerial)
														)
													);
	return i != lines.end() ? i->line : 0;
}

//-----------------------------------------------------------

void DeltaCodeLineChunkCollection::Write (GenericWriter& writer) const {

	writer.write((util_ui16) chunks.size());

	for (	std::list<DeltaCodeLineChunk>::const_iterator i = chunks.begin();
			i != chunks.end();
			++i	) {
			writer.write(i->funcSerial);
			writer.write(i->start);
			writer.write(i->end);
	}
}

/////////////////////////////////////

void DeltaCodeLineChunkCollection::Write (FILE *fp) const {

	ufwrite((util_ui16) chunks.size(), fp);

	for (	std::list<DeltaCodeLineChunk>::const_iterator i = chunks.begin();
			i != chunks.end();
			++i	) {
			ufwrite(i->funcSerial, fp);
			ufwrite(i->start, fp);
			ufwrite(i->end, fp);
	}
}

/////////////////////////////////////

#undef	CURR_TYPE
#define	CURR_TYPE	"lines chunk"

bool DeltaCodeLineChunkCollection::Read (GenericReader& reader) {

	Clear();

	util_ui16 total;
	UCHECK_PRIMARY_ERROR(reader.read(&total), "total");

	while (total--) {

		util_ui16 funcSerial, start, end;

		UCHECK_PRIMARY_ERROR(reader.read(&funcSerial), "func serial");
		UCHECK_PRIMARY_ERROR(reader.read(&start), "start line");
		UCHECK_PRIMARY_ERROR(reader.read(&end), "end line");

		chunks.push_back(DeltaCodeLineChunk(funcSerial, start, end));
	}

	return true;
	FAIL: Clear(); return false;
}

/////////////////////////////////////

const util_ui16* DeltaCodeLineChunkCollection::FindFuncSerialFromLine (util_ui16 lineNo) const {

	std::list<DeltaCodeLineChunk>::const_iterator i =	std::find_if(
															chunks.begin(),
															chunks.end(),
															std::bind2nd(
																DeltaCodeLineChunk::LineInPred(),
																lineNo
															)
														);
	return i != chunks.end() ? &i->funcSerial : (util_ui16*) 0;
}

//-----------------------------------------------------------
// Write firstly the operand type. If the operand is in use,
// write the val.
//
void DeltaOperand::Write (GenericWriter& writer) const {
	writer.write(type);
	if (type != DeltaOperand_NotUsed)
		writer.write(val);
}

void DeltaOperand::Write (FILE* fp) const {
	ufwrite(type, fp);
	if (type != DeltaOperand_NotUsed)
		ufwrite(val, fp);
}

/////////////////////////////////////
// Read the type first. If the operand is in use, read
// the whole structure.
//

#undef	CURR_TYPE
#define	CURR_TYPE	"instruction operand"

bool DeltaOperand::Read (GenericReader& reader) {

	UCHECK_PRIMARY_ERROR(reader.read(&type), "type");
	UCHECK_PRIMARY_ERROR(IsValidType(), "type (invalid value)");
	UCHECK_PRIMARY_ERROR(type == DeltaOperand_NotUsed || reader.read(&val), "val");

	return true;
	FAIL: return false;
}

//-----------------------------------------------------------

void DeltaInstruction::Write (GenericWriter& writer) const {
	writer.write((util_ui32) opcode);
	arg1.Write(writer);
	arg2.Write(writer);
	result.Write(writer);
	writer.write(line);
}

void DeltaInstruction::Write (FILE* fp) {
	util_ui32 _opcode = (util_ui32) opcode;
	fwrite(&_opcode, sizeof(_opcode), 1, fp);
	arg1.Write(fp);
	arg2.Write(fp);
	result.Write(fp);
	ufwrite(line, fp);
}

/////////////////////////////////////

#undef	CURR_TYPE
#define	CURR_TYPE	"instruction"

bool DeltaInstruction::Read (GenericReader& reader) {
	util_ui32 _opcode;
	UCHECK_PRIMARY_ERROR(reader.read(&_opcode), "opcode");
	UCHECK_PRIMARY_ERROR(_opcode < DELTA_TOTAL_VMINSTRUCTIONS, "invalid opcode");

	opcode = (DeltaVMOpcode) _opcode;
	UCHECK_DOMINO_ERROR(arg1.Read(reader), "arg1");
	UCHECK_DOMINO_ERROR(arg2.Read(reader), "arg2");
	UCHECK_DOMINO_ERROR(result.Read(reader), "result");
	UCHECK_PRIMARY_ERROR(reader.read(&line), "line");

	return true;
	FAIL: return false;
}

//-----------------------------------------------------------

DBYTECODE_FUNC bool DeltaWriteVersionInformation (GenericWriter& writer) {
	writer.write((util_ui32) DELTA_MAGIC_NO);
	writer.write((util_ui16) DELTA_VERSION_MAJOR_NO);
	writer.write((util_ui16) DELTA_VERSION_MINOR_NO);
	return true;
}

/////////////////////////////////////

#undef	CURR_TYPE
#define	CURR_TYPE	"version"

DBYTECODE_FUNC bool DeltaReadVersionInformation (GenericReader& reader) {

	util_ui32 magic;
	UCHECK_PRIMARY_ERROR(reader.read(&magic), "magic number");
	UCHECK_PRIMARY_ERROR(DELTA_ISVALID_MAGICNO(magic), "invalid magic number");

	util_ui16 major, minor;
	UCHECK_PRIMARY_ERROR(reader.read(&major), "version major number");
	UCHECK_PRIMARY_ERROR(reader.read(&minor), "version minor number");
	UCHECK_PRIMARY_ERROR(!DELTA_IS_OLDER_VERSION(major, minor), "version; older, rebuild scripts");
	UCHECK_PRIMARY_ERROR(!DELTA_IS_NEWER_VERSION(major, minor), "version; newer, rebuild compiler");

	DASSERT(DELTA_IS_CURRENT_VERSION(major, minor));

	return true;
	FAIL: return false;
}

//-----------------------------------------------------------
