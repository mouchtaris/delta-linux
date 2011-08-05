// DeltaByteCodeTypes.h.
// Contains some definitions for Delta byte code.
// ScriptFighter Project.
// A. Savidis, October 1999.
// Updated July 2007, adding list of formal argument names and
// flag if the func is a method.
//

#ifndef	DELTABYTECODETYPES_H
#define	DELTABYTECODETYPES_H

#include <stdio.h>

#include "GenericReader.h"
#include "DeltaVars.h"
#include "DeltaStdDefs.h"
#include "DeltaUnparsedCallsInStmtHolder.h"
#include "utypes.h"
#include "uptr.h"
#include "ustrings.h"
#include "ustlhelpers.h"

#include <list>
#include <functional>
#include <string>

//-----------------------------------------------------------

class DBYTECODE_CLASS DeltaClosureVarInfo {

	public:
	enum VarType {
		VarTypeLocal		= 0,
		VarTypeFormal		= 1,
		VarTypeGlobal		= 2,
		VarTypeClosure		= 3
	};

	static bool			IsValidOperandType (VarType type) 
							{ return type >= VarTypeLocal && type <= VarTypeClosure; }

	private:	
	util_ui16			offset;
	VarType				type;
	std::string			name;
	util_ui32			line;
	bool				Invariant (void) const {
							return	IsValidOperandType(type)	&& 
									!name.empty()				&& 
									line != DELTA_CANTBE_A_SOURCE_LINE; 
						}

	public:
	util_ui16			GetOffset (void) const 
							{ DASSERT(Invariant()); return offset; }
	VarType				GetType (void) const
							{ DASSERT(Invariant()); return type; }
	const std::string	GetName (void) const
							{ DASSERT(Invariant()); return name; }
	util_ui32			GetLine (void) const
							{ DASSERT(Invariant()); return line; }

	DeltaClosureVarInfo (util_ui16 _offset, VarType _type, const std::string _name, util_ui32 _line) :
		offset	(_offset),
		type	(_type),
		name	(_name),
		line	(_line) { DASSERT(Invariant()); }
	~DeltaClosureVarInfo(){}
};

//-----------------------------------------------------------

enum DeltaFunctionReturnBasicType {
	DeltaFunctionReturnAnyValue		=	0,
	DeltaFunctionReturnAnyValues	=	1,
	DeltaFunctionReturnExternId		=	2,
	DeltaFunctionReturnObject		=	3,
	DeltaFunctionReturnBool			=	4,
	DeltaFunctionReturnNumber		=	5,
	DeltaFunctionReturnString		=	6,
	DeltaFunctionReturnFunction		=	7,
	DeltaFunctionReturnMethod		=	8,
	DeltaFunctionReturnLibFunction	=	9,
	DeltaFunctionReturnLibCallable	=	10,
	DeltaFunctionReturnVoid			=	11,
	DeltaFunctionReturnUndefined	=	12,
	DeltaFunctionReturnNil			=	13
};

/////////////////////////////////////////////////////////////

class DBYTECODE_CLASS DeltaFunctionReturnLibraryType {

	UDISABLE_ASSIGN_OPERATOR_AND_COPY_CONSTRUCTOR_PRIVATE(DeltaFunctionReturnLibraryType)

	private:
	typedef std::list<const DeltaFunctionReturnLibraryType*> BaseList;
	std::string	fullPath;
	BaseList	baseTypes;
	public:
	const std::string&			GetFullPath (void) const 
									{ DASSERT(!fullPath.empty()); return fullPath; }
	const BaseList&				GetBaseTypes (void) const
									{ DASSERT(!fullPath.empty()); return baseTypes; }
	void						AddBaseType (const DeltaFunctionReturnLibraryType* type)
									{ DASSERT(!fullPath.empty()); baseTypes.push_back(type); }
	DeltaFunctionReturnLibraryType*
								Clone (void) const;
	void 						WriteText (FILE* fp) const; // COMP
	bool 						Read (GenericReader& reader); // VM
	void						Write (GenericWriter& writer) const; // COMP

	DeltaFunctionReturnLibraryType (void) {}
	DeltaFunctionReturnLibraryType (const std::string& _fullPath) : fullPath(_fullPath){}
	~DeltaFunctionReturnLibraryType() { ulistclear<const DeltaFunctionReturnLibraryType>()(baseTypes); }
};

/////////////////////////////////////////////////////////////

class DBYTECODE_CLASS DeltaFunctionReturnType {

	// UDISABLE_ASSIGN_OPERATOR_AND_COPY_CONSTRUCTOR_PRIVATE(DeltaFunctionReturnType)

	private:
	enum MetaType { Basic = 0, Library = 1, None = 2 };
	MetaType metaType;
	union data_union_t {
		const DeltaFunctionReturnLibraryType*	libType;
		DeltaFunctionReturnBasicType			basicType;
	} data;

	public:
	bool						IsBasicType (void) const
									{ return metaType == Basic; }
	bool						IsLibraryType (void) const
									{ return metaType == Library; }
	DeltaFunctionReturnBasicType
								GetBasicType (void) const
									{ DASSERT(IsBasicType()); return data.basicType; }
	const DeltaFunctionReturnLibraryType*
								GetLibraryType (void) const
									{ DASSERT(IsLibraryType()); return data.libType; }
	static const char*			GetBasicTypeString (DeltaFunctionReturnBasicType type);
	static bool					GetBasicTypeFromString (const std::string& typeId, DeltaFunctionReturnBasicType* type);

	void 						WriteText (FILE* fp) const; // COMP
	bool 						Read (GenericReader& reader); // VM
	void						Write (GenericWriter& writer) const; // COMP

	DeltaFunctionReturnType (void)											: metaType(None)	{ uzeromemory(&data);	}
	DeltaFunctionReturnType (DeltaFunctionReturnBasicType type)				: metaType(Basic)	{ data.basicType = type;	}
	DeltaFunctionReturnType (const DeltaFunctionReturnLibraryType* type)	: metaType(Library) { data.libType = type;	}
	DeltaFunctionReturnType (const DeltaFunctionReturnType& r);
	~DeltaFunctionReturnType() { if (metaType == Library) udelete(data.libType); }
};

/////////////////////////////////////////////////////////////

class DBYTECODE_CLASS DeltaFunctionReturnTypes {

	private:
	typedef std::list<const DeltaFunctionReturnType*> TypeList;
	TypeList retTypes;

	public:
	const TypeList&				GetReturnTypes (void) const	
									{ return retTypes; }
	void						IncludeReturnType (DeltaFunctionReturnBasicType type)
									{ retTypes.push_back(DNEWCLASS(DeltaFunctionReturnType, (type))); }
	void						IncludeReturnType (const DeltaFunctionReturnLibraryType* type)
									{ retTypes.push_back(DNEWCLASS(DeltaFunctionReturnType, (type))); }
	void						Clear (void) { ulistclear<const DeltaFunctionReturnType>()(retTypes); }

	void 						WriteText (FILE* fp) const; // COMP
	bool 						Read (GenericReader& reader); // VM
	void						Write (GenericWriter& writer) const; // COMP

	UOVERLOADED_VOID_ASSIGN_VIA_COPY_CONSTRUCTOR(DeltaFunctionReturnTypes)

	DeltaFunctionReturnTypes (void){}
	DeltaFunctionReturnTypes (const DeltaFunctionReturnTypes& r);
	~DeltaFunctionReturnTypes() { Clear(); }
};

//-----------------------------------------------------------

class DBYTECODE_CLASS DeltaStdFuncInfo {

	public:
	typedef std::list<DeltaClosureVarInfo>	ClosureVarsInfo;

	private:
	bool						isAnonymous;
	bool						isExported;
	std::string					name;
	DeltaCodeAddress			addr;
	bool						hasVarArgs;			// If ending with ...
	ustrlist					arguments;			// Names of formal arguments.
	util_ui16					serial;				// When two functions have the same serial, they are equal.
	bool						isMethod;			// If it is a method (member function).
	ClosureVarsInfo				closureVars;		// Referenced locals / formals of the outer function.
	bool						isGlobal;
	DeltaFunctionReturnTypes	retTypes;			// Return types information written ONLY for global named functions.

	public:
	bool						IsGlobal (void) const
									{ return isGlobal; }
	const std::string&			GetName (void) const
									{ return name; }

	DeltaCodeAddress			GetAddress (void) const
									{ return addr; }

	const ustrlist&				GetFormals (void) const
										{ return arguments; }
	util_ui16					GetTotalFormals (void) const 
									{ return (util_ui16) arguments.size(); }
	const std::string			GetFormalName (util_ui16 i) const 
									{ DASSERT(i < GetTotalFormals()); return ulistgetbyindex(arguments, i); }
	bool						HasVarArgs (void) const
									{ return hasVarArgs; }

	void 						Set (const DeltaStdFuncInfo& info) 
									{ udestructor_invocation(this); new (this) DeltaStdFuncInfo(info); }

	util_ui16					GetSerial (void) const
									{ return serial; }
	bool						IsMethod (void) const 
									{ return isMethod; }
	bool						IsNamedFunction (void) const
									{ return !isMethod && !isAnonymous; }
	bool						IsExported (void) const
									{ return isExported; }
	void						SetClosureVarsInfo (const ClosureVarsInfo& vars)
									{ DASSERT(closureVars.empty()); closureVars = vars; }
	const ClosureVarsInfo&		GetClosureVarsInfo (void) const
									{ return closureVars; }
	bool						HasClosureVars (void) const
									{ return !closureVars.empty(); }

	const DeltaFunctionReturnTypes&
								GetReturnTypes (void) const // COMP (using dbc)
									{ DASSERT(IsGlobal() && IsNamedFunction()); return retTypes; }
	DeltaFunctionReturnTypes*	GetReturnTypesPtr (void)		// COMP (code gen)
									{ DASSERT(IsGlobal() && IsNamedFunction()); return &retTypes; }
	void						SafeClearReturnTypes (void)	// VM
									{ retTypes.Clear(); }						

	void 						WriteText (FILE* fp) const; // COMP
	bool 						Read (GenericReader& reader); // VM
	static DeltaStdFuncInfo*	ReadFunctionTable (GenericReader& reader, util_ui16* total); // COMP / VM
	void						Write (GenericWriter& writer) const; // COMP

	UASSIGN_OPERATOR_RETURN_VOID(DeltaStdFuncInfo)

	///////////////////////////////////////////////

	DeltaStdFuncInfo (
		const std::string&				_name, 
		bool							_isExported,
		bool							_isAnonymous,
		DeltaCodeAddress				_addr,
		bool							_hasVarArgs,
		util_ui16						_serial,
		const std::list<std::string>&	_arguments,
		bool							_isMethod,
		bool							_isGlobal
	) : isAnonymous			(_isAnonymous),
		isExported			(_isExported),
		name				(_name),
		addr				(_addr),
		hasVarArgs			(_hasVarArgs),
		arguments			(_arguments),
		serial				(_serial),
		isMethod			(_isMethod),
		isGlobal			(_isGlobal)
		{}

	DeltaStdFuncInfo (const DeltaStdFuncInfo& info) :
		isAnonymous			(info.isAnonymous),
		isExported			(info.isExported),
		name				(info.name),
		addr				(info.addr),
		hasVarArgs			(info.hasVarArgs),
		arguments			(info.arguments),
		serial				(info.serial),
		isMethod			(info.isMethod),
		closureVars			(info.closureVars),
		isGlobal			(info.isGlobal),
		retTypes			(info.retTypes)
		{}

	DeltaStdFuncInfo (void) :
		isAnonymous			(false),
		isExported			(false),
		name				(""),
		addr				(0),
		hasVarArgs			(false),
		serial				(0),
		isMethod			(false),
		isGlobal			(false){}
};

//-----------------------------------------------------------

struct DBYTECODE_CLASS DeltaCodeLine {

	util_ui16 line;
	util_ui16 funcSerial;	// 0 means global.

	static util_ui32 SizeOf (void) { return 2 * sizeof(util_ui16); }

	DeltaCodeLine (const DeltaCodeLine& item) : 
			line(item.line), 
			funcSerial(item.funcSerial) {}

	DeltaCodeLine (util_ui16 _line, util_ui16 _funcSerial) : 
			line(_line), 
			funcSerial(_funcSerial) {}

	DeltaCodeLine (void) : 
			line(0), 
			funcSerial(0) {}
};

///////////////////////////////////////////////

struct DBYTECODE_CLASS DeltaCodeLineCollection {

	std::list<DeltaCodeLine> lines;

	struct FindNextPred : public std::binary_function<DeltaCodeLine, DeltaCodeLine, bool> {
		bool operator()(const DeltaCodeLine& l1, const DeltaCodeLine& l2) const
			{ return l1.funcSerial == l2.funcSerial && l2.line < l1.line; }
	};

	struct EqualPred : public std::binary_function<DeltaCodeLine, util_ui16, bool> {
		bool operator()(const DeltaCodeLine& l1, util_ui16 l2) const
			{ return l1.line == l2; }
	};

	void		Clear (void) { lines.clear(); }
	util_ui16	FindNextLine (util_ui16 line, util_ui16 funcSerial) const;

	void		Write (FILE *fp); // COMP
	bool		Read (GenericReader& reader);  // DBG
	void		Write (GenericWriter& writer) const; // COMP

	bool		IsValidLine (util_ui16 line) const;

	void		AddLine (util_ui16 line, util_ui16 serial) {
					if (!IsValidLine(line))
						lines.push_back(DeltaCodeLine(line, serial));
				}

	DeltaCodeLineCollection (void) {}
	~DeltaCodeLineCollection() {}
};

///////////////////////////////////////////////
// Holds the distribution of code lines, so that
// an empty line can be decided as to where it falls.
//
struct DBYTECODE_CLASS DeltaCodeLineChunk {

	util_ui16	funcSerial;
	util_ui16	start;
	util_ui16	end;

	struct LineInPred : public std::binary_function<DeltaCodeLineChunk, util_ui16, bool> {
		bool operator()(const DeltaCodeLineChunk& chunk, util_ui16 l) const
			{ return chunk.start <= l && l <= chunk.end; }
	};

	DeltaCodeLineChunk (void) :
		funcSerial(DELTA_FUNCSERIAL_OF_GLOBAL_CODE),
		start(0),
		end(0) {}

	DeltaCodeLineChunk (const DeltaCodeLineChunk& chunk) :
		funcSerial(chunk.funcSerial),
		start(chunk.start),
		end(chunk.end) {}

	DeltaCodeLineChunk (util_ui16 _funcSerial, util_ui16 _start, util_ui16 _end) :
		funcSerial(_funcSerial),
		start(_start),
		end(_end) {}
};

///////////////////////////////////////////////

struct DBYTECODE_CLASS DeltaCodeLineChunkCollection {

	std::list<DeltaCodeLineChunk> chunks;

	const util_ui16*	
				FindFuncSerialFromLine (util_ui16 line) const;

	void		Clear (void) 
					{ chunks.clear(); }			

	void		AppendChunk (DeltaCodeLineChunk& chunk) 
					{ chunks.push_back(chunk); }
				
	void		Write (FILE *fp) const; // COMP
	bool		Read (GenericReader& reader); // DBG
	void		Write (GenericWriter& writer) const; // COMP

	DeltaCodeLineChunkCollection (void) {}
	~DeltaCodeLineChunkCollection() { chunks.clear(); }
};

///////////////////////////////////////////////

class DBYTECODE_CLASS DeltaDebugFuncInfo;

class DBYTECODE_CLASS DeltaDebugProgramInfo {

	private:
	typedef std::map<DeltaCodeAddress, DeltaDebugFuncInfo*> FuncMap;
	typedef std::list<std::string>							NameList;
	typedef	std::map<std::string, DeltaNumberValueType>		NumConsts;
	typedef	std::map<std::string, std::string>				StrConsts;

	DeltaVars						globals;
	util_ui16						totalFuncs;
	FuncMap							funcMap;			// Indexed by addresses.
	DeltaDebugFuncInfo*				funcs;				// Ordered by serial number.
	DeltaCodeLineCollection			lines;				// Lines with code, with function information.
	DeltaCodeLineChunkCollection	chunks;				// All lines, in chunks with respective function serial.
	DeltaUnparsedCallsInStmtHolder	calls;				// Per stmt its unparsed calls when more than one.
	bool							isDynamicCode;		// If compiled from a string.
	std::string						dynamicSource;		// The string conmtaining the source.
	NameList						openedNamespaces;	// All namespaces opened due to using directives.
	NumConsts						numLibraryConsts;	// Numeric used library consts.
	StrConsts						strLibraryConsts;	// String used library consts.
	NameList						byteCodeLibs;		// All used byte code libraries.

	public:
	void							SetDynamicCode (const std::string& src);
	bool							IsDynamicCode (void) const
										{ return isDynamicCode; }
	const std::string&				GetDynamicCode (void) const 
										{ return dynamicSource; }

	void							AddOpenedNamespace (const std::string& name)									// COMP
										{ openedNamespaces.push_back(name); }
	const NameList&					GetOpenedNamespaces (void) const												// DBG
										{ return openedNamespaces; }

	void							AddUsedByteCodeLibrary (const std::string& name)								// COMP
										{ byteCodeLibs.push_back(name); }
	const NameList&					GetUsedByteCodeLibraries (void) const											// DBG
										{ return byteCodeLibs; }
	void							AddUsedLibraryConst (const std::string& name, DeltaNumberValueType val)			// COMP
										{ numLibraryConsts[name] = val; }
	void							AddUsedLibraryConst (const std::string& name, const std::string& val)			// COMP
										{ strLibraryConsts[name] = val; }
	bool							GetUsedLibraryConst (const std::string& name, DeltaNumberValueType* val) const;	// DBG
	bool							GetUsedLibraryConst (const std::string& name, std::string* val) const;			// DBG

	void							SetTotalFuncs (util_ui16 _totalFuncs);
	util_ui16						GetTotalFuncs (void) const
										{ return totalFuncs; }
	DeltaDebugFuncInfo*				GetFunc (util_ui16 serial);
	const DeltaDebugFuncInfo&		GetFunc (util_ui16 serial) const;
	DeltaDebugFuncInfo*				GetFuncByAddr (DeltaCodeAddress addr);
	const DeltaDebugFuncInfo*		GetFuncByAddr (DeltaCodeAddress addrf) const;

	DeltaCodeLineCollection&		GetLines (void) { return lines; }
	util_ui16						GetNextLine (util_ui16 line) const;	// In same func serial.

	DeltaCodeLineChunkCollection&	GetChunks (void) 
										{ return chunks; }	
	DeltaVars&						GetGlobals (void) 
										{ return globals; }
	const DeltaVars&				GetGlobals (void) const 
										{ return globals; }
	
	DeltaUnparsedCallsInStmtHolder& GetCalls (void) 
										{ return calls; }
	const DeltaUnparsedCallsInStmtHolder& 
									GetCalls (void) const
										{ return calls; }

	void							SetCalls (const DeltaUnparsedCallsInStmtHolder& _calls)
										{ udestructor_invocation(&calls); new (&calls) DeltaUnparsedCallsInStmtHolder(_calls); }

	void							Clear (void);
									
	void							WriteText (FILE* fp) const;	// COMP
	bool							Read (GenericReader& reader); // DBG
	void							Write (GenericWriter& writer) const; // COMP

	DeltaDebugProgramInfo (void);
	~DeltaDebugProgramInfo() { Clear(); }
};

//-----------------------------------------------------------

typedef util_ui8 DeltaVMOpcode;

#define DeltaVM_ADD								0
#define DeltaVM_OBJGET							1
#define DeltaVM_OBJNEW							2
#define DeltaVM_OBJNEWSET						3
#define DeltaVM_OBJNEWEMPTY						4
#define DeltaVM_OBJSET							5
#define DeltaVM_ASSIGN							6
#define DeltaVM_ASSIGNTEMPVAR					7
#define DeltaVM_CALLFUNC						8
#define DeltaVM_CALLPROGRAMFUNC					9
#define DeltaVM_CALLLAMBDAPROGRAMFUNC			10
#define DeltaVM_CALLLAMBDAMETHODFUNC			11
#define DeltaVM_CALLEXTFUNC						12
#define	DeltaVM_CALLEXTPREBOUNDFUNC				13
#define DeltaVM_DIV								14
#define DeltaVM_JUMP							15
#define DeltaVM_JEQ								16
#define DeltaVM_JNE								17
#define DeltaVM_JGE								18
#define DeltaVM_JGT								19
#define DeltaVM_JLE								20
#define DeltaVM_JLT								21
#define DeltaVM_JTRUETEST						22
#define DeltaVM_JFALSETEST						23
#define DeltaVM_MOD								24
#define DeltaVM_MUL								25
#define DeltaVM_SUB								26
#define	DeltaVM_PUSHARG							27
#define	DeltaVM_FUNCRET							28
#define	DeltaVM_FUNCENTER						29
#define	DeltaVM_PROGRAMBEGIN					30
#define DeltaVM_OBJDONE							31
#define	DeltaVM_PUSHLATEBOUNDARG				32
#define	DeltaVM_BLOCKENTER						33
#define	DeltaVM_BLOCKEXIT						34
#define DeltaVM_BOUNDEDOBJGET					35
#define DeltaVM_BOUNDEDOBJSET					36
#define DeltaVM_ASSERT							37
#define DeltaVM_TRAPENABLE						38
#define DeltaVM_TRAPDISABLE						39
#define DeltaVM_TRAP							40
#define DeltaVM_THROW							41
#define	DeltaVM_PROGRAMEND						42
#define	DeltaVM_OBJNEWATTR						43
#define	DeltaVM_OBJSETATTR						44
#define	DeltaVM_OBJGETATTR						45
#define	DeltaVM_CALLOBJGETMETHOD				46
#define	DeltaVM_CALLOBJBOUNDEDGETMETHOD			47
#define	DeltaVM_FOREACHBEGIN					48
#define	DeltaVM_FOREACHCHECKEND					49
#define	DeltaVM_FOREACHGETVAL					50
#define	DeltaVM_FOREACHGETINDEX					51
#define	DeltaVM_FOREACHFWD						52
#define	DeltaVM_FOREACHEND						53
#define	DeltaVM_GETRETVAL						54
#define	DeltaVM_RETURNVAL						55
#define	DeltaVM_RETURN							56
#define DeltaVM_NOP								57

#define	DELTA_TOTAL_VMINSTRUCTIONS	DeltaVM_NOP + 1

//-----------------------------------------------------------

typedef util_ui8 DeltaOperandType;

#define DeltaOperand_Label						0
#define DeltaOperand_Local						1
#define DeltaOperand_Global						2
#define DeltaOperand_Formal						3
#define DeltaOperand_ClosureVar					4
#define DeltaOperand_ProgramFunc				5
#define DeltaOperand_LibFunc					6
#define DeltaOperand_MethodFunc					7
#define DeltaOperand_ConstString				8
#define DeltaOperand_ConstNumber				9
#define DeltaOperand_ConstBool					10
#define DeltaOperand_ConstNil					11
#define DeltaOperand_Lambda						12
#define DeltaOperand_NewSelf					13
#define	DeltaOperand_Internal					14
#define DeltaOperand_NotUsed					15

#define	DELTA_TOTAL_OPERAND_TYPES				DeltaOperand_NotUsed + 1	// Last is an error.
#define	DELTA_STORAGE_TYPE_FIRST				DeltaOperand_Local
#define	DELTA_STORAGE_TYPE_LAST					DeltaOperand_ClosureVar
#define	DELTA_TOTAL_STORAGE_TYPES				((DELTA_STORAGE_TYPE_LAST - DELTA_STORAGE_TYPE_FIRST) + 1)

//-----------------------------------------------------------

#define	DELTA_ASSIGN_OPERAND_RVALUE				arg1
#define	DELTA_ASSIGN_OPERAND_LVALUE				result
#define	DELTA_GETRETVAL_OPERAND_LVALUE			result
#define	DELTA_RETURNVAL_OPERAND_RVALUE			result
#define	DELTA_BINARY_ARITH_OPERAND_LEFT			arg1
#define	DELTA_BINARY_ARITH_OPERAND_RIGHT		arg2
#define	DELTA_BINARY_ARITH_OPERAND_RESULT		result
#define	DELTA_BINARY_RELAT_OPERAND_LEFT			arg1
#define	DELTA_BINARY_RELAT_OPERAND_RIGHT		arg2
#define	DELTA_OPERAND_JUMP_TARGET				result
#define	DELTA_BLOCK_OPERAND_TOTAL_LOCALS		arg1
#define	DELTA_BLOCK_OPERAND_IS_MAIN_BLOCK		arg2
#define	DELTA_BLOCK_OPERAND_BLOCKID				result
#define	DELTA_FOREACHBEGIN_OPERAND_CONTAINER	arg1
#define	DELTA_FOREACHBEGIN_OPERAND_ITERATOR		result
#define	DELTA_FOREACHCHECKEND_OPERAND_CONTAINER	arg1
#define	DELTA_FOREACHCHECKEND_OPERAND_ITERATOR	arg2
#define	DELTA_FOREACHGETVAL_OPERAND_ITERATOR	arg1
#define	DELTA_FOREACHGETVAL_OPERAND_VARIABLE	result
#define	DELTA_FOREACHGETINDEX_OPERAND_ITERATOR	arg1
#define	DELTA_FOREACHGETINDEX_OPERAND_VARIABLE	result
#define	DELTA_FOREACHFWD_OPERAND_ITERATOR		arg1
#define	DELTA_FOREACHEND_OPERAND_ITERATOR		arg1
#define	DELTA_CALLEXTFUNC_OPERAND_FUNC			result
#define	DELTA_CALLFUNC_OPERAND_FUNC				result
#define	DELTA_FUNCENTER_OPERAND_FUNCINFO		arg1
#define	DELTA_FUNCENTER_OPERAND_TOTALLOCALS		arg2
#define	DELTA_FUNCENTER_OPERAND_BLOCKID			result
#define	DELTA_OBJNEW_OPERAND_RESULT				result
#define	DELTA_OBJNEWEMPTY_OPERAND_RESULT		result
#define	DELTA_SET_OPERAND_INDEX					arg2
#define	DELTA_SET_OPERAND_CONTENT				arg1
#define	DELTA_SET_OPERAND_CONTAINER				result
#define	DELTA_GET_OPERAND_INDEX					arg2
#define	DELTA_GET_OPERAND_CONTENT				result
#define	DELTA_GET_OPERAND_CONTAINER				arg1
#define	DELTA_OBJNEWATTR_OPERAND_SETTER			arg1
#define	DELTA_OBJNEWATTR_OPERAND_GETTER			arg2
#define	DELTA_OBJNEWATTR_OPERAND_NAME			result
#define	DELTA_OBJSETGETATTR_OPERAND_NAME		arg2
#define	DELTA_OBJSETATTR_OPERAND_OBJECT			result
#define	DELTA_OBJSETATTR_OPERAND_CONTENT		arg1
#define	DELTA_OBJGETATTR_OPERAND_OBJECT			arg1
#define	DELTA_OBJGETATTR_OPERAND_CONTENT		result
#define	DELTA_TRAPENABLE_OPERAND_ADDRESS		result
#define	DELTA_TRAPDISABLE_OPERAND_ADDRESS		result
#define	DELTA_TRAP_OPERAND_VARIABLE				result
#define	DELTA_TRAP_OPERAND_BLOCKID				arg1
#define	DELTA_THROW_OPERAND_EXCEPTION			result
#define	DELTA_JUMP_OPERAND_EXIT_BLOCKS_TOTAL	arg1
#define	DELTA_JUMP_TRUEFALSE_TEST_ARG			arg1

//-----------------------------------------------------------

#define	DELTA_TEMPVAR_OPERAND_SET_MASK			0x80000000
#define	DELTA_TEMPVAR_OPERAND_CLR_MASK			0x7fffffff

class DBYTECODE_CLASS DeltaOperand {

	private:
	util_ui8		type;			// Operand type.
	util_ui32		val;			// Operand information packed.

	bool			IsValidType (void) const
						{ return type <= DeltaOperand_NotUsed; }
	
	public:
	bool			operator==(const DeltaOperand& operand) const 
						{ return type == operand.type && val == operand.val; }
	bool			IsStorageType (void) const 
						{ return type >= DELTA_STORAGE_TYPE_FIRST && type <= DELTA_STORAGE_TYPE_LAST; }
	bool			IsTempVar (void) const
						{ DASSERT(IsStorageType()); return (val & DELTA_TEMPVAR_OPERAND_SET_MASK) != 0; }
	bool			IsInternal (void) const 
						{ return type == DeltaOperand_Internal; }
	bool			IsConstStringOrNumber (void) const
						{ return type == DeltaOperand_ConstString || type == DeltaOperand_ConstNumber; }
	DeltaOperandType GetType (void) const 
						{ DASSERT(IsValidType()); return (DeltaOperandType) type; }
	void			SetType (DeltaOperandType t) 
						{ DASSERT(IsValidType()); type = t; }
	void			SetVarValue (bool isTemp, util_ui32 v) 
						{ DASSERT(IsValidType()); val = v | (isTemp ? DELTA_TEMPVAR_OPERAND_SET_MASK : 0); }
	void			SetValue (util_ui32 v) 
						{ DASSERT(IsValidType()); val = v; }
	util_ui32		GetValue (void) const
						{ DASSERT(IsValidType()); return !IsStorageType() ? val : (val & DELTA_TEMPVAR_OPERAND_CLR_MASK); }

	void			Write (FILE* fp) const;
	void			Write (GenericWriter& writer) const;
	bool			Read (GenericReader& reader);

	DeltaOperand (util_ui8 t, util_ui32 v) : type(t), val(v){}
	DeltaOperand (void) : type(DeltaOperand_NotUsed), val(0){}
};

///////////////////////////////////////////////

struct DBYTECODE_CLASS DeltaInstruction {

	typedef void*		InlineCachePtr;

	DeltaVMOpcode		opcode;
	DeltaOperand		arg1;
	DeltaOperand		arg2;
	DeltaOperand		result;		// Holds target label in case of branch instructions.
	util_ui16			line;		// Source code line (if 0, same as previous).
	InlineCachePtr		executor;	// Cached instruction-specific executor.

	bool				IsCall (void) const {
							return	opcode == DeltaVM_CALLOBJGETMETHOD			||
									opcode == DeltaVM_CALLOBJBOUNDEDGETMETHOD	||
									opcode == DeltaVM_CALLFUNC					||
									opcode == DeltaVM_CALLPROGRAMFUNC			||
									opcode == DeltaVM_CALLEXTFUNC				||
									opcode == DeltaVM_CALLLAMBDAPROGRAMFUNC		||
									opcode == DeltaVM_CALLLAMBDAMETHODFUNC		||
									opcode == DeltaVM_CALLEXTPREBOUNDFUNC;
						}
	void				SetOpCode (DeltaVMOpcode _opcode)
							{ opcode = _opcode; }
	DeltaVMOpcode		GetOpCode (void) const 
							{ return opcode; }
	InlineCachePtr		GetExecutor (void) const
							{ return executor; }
	void				SetExecutor (InlineCachePtr _executor)
							{ executor = _executor; }
	void				ResetExecutor (void)
							{ unullify(executor); }
	
	void 				Write (GenericWriter& writer) const;
	void 				Write (FILE* fp);
	bool 				Read (GenericReader& reader);

	DeltaInstruction (void): 
		opcode		(DeltaVM_NOP), 
		line		(DELTA_CANTBE_A_SOURCE_LINE),
		executor	((void*) 0)
		{}
};

/////////////////////////////////////////////////////////////

DBYTECODE_FUNC bool DeltaWriteVersionInformation (GenericWriter& writer);
DBYTECODE_FUNC bool DeltaReadVersionInformation (GenericReader& reader);

///////////////////////////////////////////////
// This type may be set to either DeltaObject (inheritance enabled)
// or DeltaTable (inheritance free).
//
#define	DELTA_TABLE		DeltaTable
#define	DELTA_OBJECT	DeltaObject

//-----------------------------------------------------------
#endif	// Do not ad stuff beyond this point.
