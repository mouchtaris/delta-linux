// Symbol.h.
// Symbols for parsing in the Delta compiler.
// Also symbol table class.
// ScriptFighter Project.
// A. Savidis, October 1999.
//

#ifndef	SYMBOL_H
#define	SYMBOL_H

#include "DeltaStdDefs.h"
#include "DeltaByteCodeTypes.h"
#include "DeltaCompErrorMsg.h"
#include "DDebug.h"
#include "usingleton.h"
#include "utypes.h"
#include "LibraryFuncSignatures.h"
#include "LibraryTypes.h"
#include "ucrosscast.h"

#include <list>
#include <stack>
#include <functional>
#include <string>

#define	DELTA_SYMBOL_TABLE_HASH_SIZE			211
#define	NIL_SYMBOL								(DeltaSymbol*) 0
#define	DELTA_FUNCTION_MAINBLOCK_SCOPE_OFFSET	(+1)

///////////////////////////////////////////////////////////////////
// The symbol type is not actually stored in the symbol class.
//
enum DeltaSymbolType {
	DeltaSymbol_NotAVar				= 0,	// Every other type except variables.
	DeltaSymbol_VarInCurrScope		= 1,	// A variable in the current scope.
	DeltaSymbol_StaticVar			= 2		// A static variable (forced to be of global storage).
};

#define	DELTA_FUNCCLASS_NOTAFUNCTION			0
#define	DELTA_FUNCCLASS_PROGRAMFUNCTION			1
#define	DELTA_FUNCCLASS_ONEVENT					2
#define	DELTA_FUNCCLASS_METHOD					3
#define	DELTA_LOCAL_SCOPEOFFSET					0
#define	DELTA_FUNCTION_DEFAULT_EXPORT			0
#define	DELTA_FUNCTION_NO_EXPORT				1
#define	DELTA_OUTSIDE_FUNCTION_BLOCK_OFFSET		+2
#define	DELTA_SYMBOL_TYPE_IS_VARIABLE(type)		((type) != DeltaSymbol_NotAVar)

///////////////////////////////////////////////////////////////////

struct DeltaExpr;
class DeltaLibraryNamespace;
typedef std::list<DeltaLibraryTypeInfo> DeltaLibraryTypeInfoList;

struct DeltaSymbol {

	struct MatchByName : public std::binary_function<DeltaSymbol*, std::string, bool> {
		bool operator()(DeltaSymbol* sym, const std::string& name) const
			{ return DPTR(sym)->GetName() == name; }
	};

	// Common.
	std::string					name;
	util_ui32					scope;						// Global is 0, funcs and blocks inc by 1.
	util_ui32					line;						// Source code line.
	util_ui16					blockId;					// The block in which the symbol belongs.
	DeltaSymbol*				myFunc;						// Func in which defined, if not global.
	bool						isStatic;					// For static variables.
	DeltaSymbol*				next;

	// Variables.
	util_i16					offset;						// Logical offset for storage variables, always positive.
	bool						isFormal;					// If a formal func arg.
	util_ui16					initialisedCounter;			// To know if a non-formal var is used un-initialised.
	bool						possiblyUninitialised;		// Used only if not initialised.

	//**************************
	// When a variable is an imported function of a bytecode library.
	
	class ImportedFuncVarInfo {
		private:

		util_ui32			definitionQuad;
		bool				isUsed;
		std::string			libName, funcName;

		public:
		bool				IsUsed (void) const { return isUsed; }
		void				SetIsUsed (void) { isUsed = true; }
		const std::string	GetFunctionReadableName (void) const
								{ return libName + DELTA_LIBRARYNAMESPACE_SEPARATOR + funcName; }

		ImportedFuncVarInfo (util_ui32 quadNo, const std::string& _libName, const std::string& _funcName) : 
			definitionQuad	(quadNo),
			isUsed			(false),
			libName			(_libName),
			funcName		(_funcName)	
			{}
	};

	ImportedFuncVarInfo*		funcVarInfo;

	bool						IsImportedFunctionVar (void) const
									{ return !!funcVarInfo; }
	void						SetAsImportedFunctionVar (util_ui32 quadNo, const std::string& libName, const std::string& funcName) {
									DASSERT(!IsImportedFunctionVar()); 
									DASSERT(ustrprefix(DELTA_IMPORTEDFUNCVAR_NAME_PREFIX, GetName()));
									funcVarInfo = DNEWCLASS(ImportedFuncVarInfo, (quadNo, libName, funcName));
									SetInitialised();
								}
	const ImportedFuncVarInfo*	GetImportedFuncVar (void) const
									{ DASSERT(IsImportedFunctionVar()); return funcVarInfo; }
	ImportedFuncVarInfo*		GetImportedFuncVar (void)
									{ DASSERT(IsImportedFunctionVar()); return funcVarInfo; }
	static const std::string	MakeImportedFuncVarName (const std::string& libName, const std::string& funcName)
									{ return uconstructstr(DELTA_IMPORTEDFUNCVAR_NAME_FORMAT, libName.c_str(), funcName.c_str()); }
							
	//**************************
	// When a variable is an bytecode library.

	struct ImportedByteCodeLibInfo {

		std::map<std::string, DeltaSymbol*> funcs;

		DeltaSymbol*	Get (const std::string& id) {
							std::map<std::string, DeltaSymbol*>::iterator i = funcs.find(id);
							if (i != funcs.end()) {
								DASSERT(ustrprefix(DELTA_IMPORTEDFUNCVAR_NAME_PREFIX, i->second->GetName()));
								DASSERT(i->second->IsImportedFunctionVar());
								return i->second;
							}
							else
								return (DeltaSymbol*) 0;
						}

		void			Add (const std::string& id, DeltaSymbol* sym) {
							DASSERT(sym->IsTopGlobal());
							DASSERT(sym->IsImportedFunctionVar());
							DASSERT(!Get(id));
							funcs[id] = sym;
						}
	};

	ImportedByteCodeLibInfo*	libVarInfo;

	bool						IsImportedLibVar (void) const
									{ return !!libVarInfo; }
	void						SetAsImportedLibVar (void) {
									DASSERT(!IsImportedLibVar()); 
									libVarInfo = DNEW(ImportedByteCodeLibInfo); 
									SetInitialised();
								}
	ImportedByteCodeLibInfo*	GetImportedLibVar (void)
									{ DASSERT(IsImportedLibVar()); return libVarInfo; }

	//**************************
	// Functions.
	struct FuncSig {
		
		std::list<std::string>		formals;
		DeltaLibraryTypeInfoList	retTypes;
		bool						isExported;
		bool						hasVarArgs;

		const DeltaLibraryTypeInfoList&
									GetReturnTypes (void) const 
										{ return retTypes; }

		DeltaLibraryTypeInfoList*	GetReturnTypesPtr (void)
										{ return &retTypes; }

		void						IncludeReturnType (const DeltaLibraryTypeInfo& typeInfo) {
										if (retTypes.empty())
											DASSERT(typeInfo.IsVoid());		// Initially assumed void function.
										else {
											DASSERT(!typeInfo.IsVoid());	// User return type.
											if (retTypes.size() == 1 && retTypes.front().IsVoid())
												retTypes.clear();			// Remove the initial void return type.
										}	
										DeltaLibraryTypeInfo::AddInTypeListIfNotInside(&retTypes, typeInfo);
									}

		void						AddFormal (const std::string& name) 
										{ formals.push_back(name); }
		bool						IsReturningSomething (void) const
										{ return !retTypes.empty() && (retTypes.size() != 1 || !retTypes.front().IsVoid()); }
		bool						IsExported (void) const
										{ return isExported; }
		bool						HasVarArgs (void) const 
										{ return hasVarArgs; }
		void						SetHasVarArgs (void)
										{ DASSERT(!hasVarArgs); hasVarArgs = true; }

		FuncSig (bool _isExported) : 
			isExported	(_isExported),
			hasVarArgs	(false)
			{}
	};

	// Special access method for functions with closures being used at scope
	// inner than their defintion point.

	struct FuncAccess {
		
		typedef std::pair<DeltaExpr*, DeltaSymbol*> UseInfo;
		typedef std::list<UseInfo>					UseInfoList;

		DeltaSymbol*				func;
		DeltaSymbol*				funcVar;
		bool						isUsed;
		util_ui32					assignQuadNo;
		UseInfoList					innerUses;
		
		void						RecordInnerUse (DeltaExpr* funcExpr, DeltaSymbol* clientFunc);
		UseInfoList&				GetInnerUses (void) 
										{ return innerUses; }
		void						ClearInnerUses (void)	
										{ innerUses.clear(); }

		void						CreateFunctionVar (void);
		DeltaSymbol*				GetFunctionVar (void) 
										{ return DNULLCHECK(funcVar); }
		void						SetFunctionVarUsed (void) 
										{ isUsed = true; }
		bool						IsFunctionVarUsed (void) const
										{ return isUsed; }
		void						SetAssignQuadNo (util_ui32 n) 
										{ assignQuadNo = n; }
		util_ui32					GetAssignQuadNo (void) const
										{ return assignQuadNo; }

		FuncAccess (DeltaSymbol* _func) :
			func			(_func),
			funcVar			((DeltaSymbol*) 0),
			isUsed			(false),
			assignQuadNo	(0)
			{}
		~FuncAccess(){}
	};
	
	//**************************

	bool						isUserDefinedFunc;			// If it is a normal user defined function.
	util_ui8					funcClass;					// If user defined function, its class.
	std::string					methodName;					// If it is a method with an explicit name.
	FuncSig*					funcSig;					// Signature information for user-defined functions.
	FuncAccess*					funcAccess;					// Access method for functions with closure.
	bool						isLibraryFunc;				// If it is an external function.
	bool						isOrphanMethod;				// A method outside object constructors.
	DeltaExpr*					methodVar;					// The var carrying the orphan method value.
	DeltaLibraryFuncSignatures*	libFuncSigs;				// All signatures of a library function.
	util_ui32					serial;						// Serial, if function.
	util_ui16					totalBlocks;				// Total blocks for this function.
	DeltaCodeAddress			addr;						// Address if a function, given upon code generation.
	std::string					sourceCode;					// The unparsed source code.

	//**************************
	// Function closure.
	
	class ClosureInfo {

		private:
		typedef std::map<DeltaSymbol*, util_ui32> SymOrderMap;
		std::list<DeltaSymbol*>			vars;				// Pseudo vars belonging to the function's closure.
		SymOrderMap						varOrders;			// By order of first appearence in the function block.

		public:
		void							Add (DeltaSymbol* sym) {
											DASSERT(varOrders.find(sym) == varOrders.end());
											varOrders[sym]		= (util_ui32) vars.size();
											DPTR(sym)->offset	= (util_ui16) vars.size();	// Offset is order of appearence.
											vars.push_back(sym);
										}
		DeltaSymbol*					Get (const std::string& name) {
											std::list<DeltaSymbol*>::iterator i;
											i = std::find_if(
													vars.begin(),
													vars.end(),
													std::bind2nd(MatchByName(), name)
												);
											return i != vars.end() ? *i : (DeltaSymbol*) 0;
										}
		const std::list<DeltaSymbol*>&	GetAll (void) const 
											{ return vars; }
		ClosureInfo(void){}
		~ClosureInfo(){}
	};
	
	ClosureInfo*				closureInfo;
	ClosureInfo*				GetClosure (void)
									{ return closureInfo; }
	bool						HasClosure (void) const
									{ return !!closureInfo; }
	void						AddClosureVar (DeltaSymbol* sym) {
									if (!closureInfo) 
										closureInfo = DNEW(ClosureInfo);
									else
										DASSERT(!DPTR(closureInfo)->GetAll().empty());
									DPTR(closureInfo)->Add(sym);
								}
	DeltaSymbol*				GetClosureVar (const std::string& name) 
									{ return closureInfo ? DPTR(closureInfo)->Get(name) : (DeltaSymbol*) 0; }
	const std::list<DeltaSymbol*>&
								GetClosureVars (void) const
									{ DASSERT(HasClosure() && !DPTR(closureInfo)->GetAll().empty()); return DPTR(closureInfo)->GetAll(); }

	//**************************

	// Consts.
	bool						isLibraryConst;				// Is it a library constant?
	bool						isUserDefinedConst;			// Is it a symbolic (named) constant?
	DeltaExpr*					constExpr;					// Its const expression.
	
	// Closure vars.
	bool						isClosureVarAccess;
	DeltaSymbol*				closureVarAccessed;
	bool						IsClosureVarAccess (void) const
									{ return isClosureVarAccess; }
	bool						IsDirectClosureVarAccess (void) const
									{ DASSERT(IsClosureVarAccess()); return !closureVarAccessed->isClosureVarAccess; }
	DeltaSymbol*				GetClosureVarAccessed (void) const
									{ return closureVarAccessed; }
	void						SetIsClosureVarAccess (DeltaSymbol* var, DeltaSymbol* func) {
									DASSERT(!isClosureVarAccess && !closureVarAccessed);
									isClosureVarAccess = true;
									closureVarAccessed = var;
									func->AddClosureVar(this);
								}

	const std::string&			GetName (void) const			
									{ return name; }
	util_ui32					GetLine (void) const			
									{ return line; }
	DeltaSymbol*				GetMyFunction (void) 
									{ return myFunc; }
	const DeltaSymbol*			GetMyFunction (void) const
									{ return myFunc; }
	bool						IsInnerFunctionOf (DeltaSymbol* func) const
									{ return GetMyFunction() && (func == GetMyFunction() || GetMyFunction()->IsInnerFunctionOf(func)); }

	// Functions.
	bool						IsGlobalFunction (void) const 
									{ return !myFunc; }
	void						SetSourceCode (const std::string& text)
									{ sourceCode = text; }
	bool						HasVarArgs (void) const 
									{ DASSERT(isUserDefinedFunc || IsImportedFunctionVar()); return DPTR(funcSig)->HasVarArgs(); }
	void						SetHasVarArgs (void)
									{ DASSERT(!DPTR(funcSig)->HasVarArgs()); return DPTR(funcSig)->SetHasVarArgs(); }
	bool						IsFunction (void) const			
									{ return isUserDefinedFunc || isLibraryFunc; }
	bool						IsUserDefinedFunction (void) const
									{ return isUserDefinedFunc; }
	bool						IsLibraryFunction (void) const			
									{ return isLibraryFunc; }
	bool						IsMethod (void) const
									{ return isUserDefinedFunc && funcClass == DELTA_FUNCCLASS_METHOD; }
	bool						IsOrphanMethod (void) const
									{ return IsMethod() && isOrphanMethod; }
	void						SetOrphanMethod (DeltaExpr* _methodVar)
									{ DASSERT(IsMethod()); isOrphanMethod = true; methodVar = DNULLCHECK(_methodVar); }
	DeltaExpr*					GetMethodVar (void)
									{ DASSERT(IsOrphanMethod()); return methodVar; }
	bool						IsAnonymousFunction (void) const 
									{ return IsFunction() && name[0] == DELTA_HIDDEN_SYMBOL_ID_PREFIX[0]; }	
	const std::string			GetFunctionClass (void) const;
	const std::string			GetFunctionReadableName (void) const;
	std::list<std::string>&		GetFormals (void)				
									{ DASSERT(isUserDefinedFunc || IsImportedFunctionVar()); return DPTR(funcSig)->formals; }
	const std::list<std::string>&		
								GetFormals (void) const				
									{ DASSERT(isUserDefinedFunc || IsImportedFunctionVar()); return DPTR(funcSig)->formals; }
	util_ui32					GetSerial (void) const
									{ DASSERT(isUserDefinedFunc); return serial; }
	util_ui32					GetTotalFormals (void) const
									{ DASSERT(isUserDefinedFunc || IsImportedFunctionVar()); return (util_ui32) DPTR(funcSig)->formals.size(); }
	void						SetAddress (DeltaCodeAddress _addr)
									{ DASSERT(isUserDefinedFunc); addr = _addr; }
	DeltaCodeAddress			GetAddress (void) const
									{ DASSERT(isUserDefinedFunc); return addr; }
	void						AddFormal (const std::string& name)
									{ DASSERT(isUserDefinedFunc || IsImportedFunctionVar()); return DPTR(funcSig)->AddFormal(name); }
	void						IncludeReturnType (const DeltaLibraryTypeInfo& typeInfo)
									{ DASSERT(isUserDefinedFunc); DPTR(funcSig)->IncludeReturnType(typeInfo); }
	bool						IsFunctionReturningSomething (void) const 
									{ DASSERT(isUserDefinedFunc); return DPTR(funcSig)->IsReturningSomething(); }
	bool						IsFunctionExported (void) const 
									{ DASSERT(isUserDefinedFunc); return DPTR(funcSig)->IsExported(); }
	DeltaLibraryFuncSignatures*	GetLibraryFuncSignatures (void) 
									{ DASSERT(isLibraryFunc); return libFuncSigs; }
	FuncSig*					GetFunctionSignature (void)
									{ DASSERT(isUserDefinedFunc || IsImportedFunctionVar()); return funcSig; }
	FuncAccess*					GetFunctionAccess (void)
									{ DASSERT(isUserDefinedFunc); return funcAccess; }
	const FuncSig&				GetFunctionSignature (void) const
									{ DASSERT(isUserDefinedFunc || IsImportedFunctionVar()); return *DPTR(funcSig); }
	const std::string			GetTypeDescription (void) const;

	// Variables
	bool						IsUserDefinedConst (void) const	{ return isUserDefinedConst; }
	bool						IsLibraryConst (void) const		{ return isLibraryConst; }
	DeltaExpr*					GetConst (void)					{ DASSERT(isUserDefinedConst || isLibraryConst); return constExpr; }
	bool						IsGlobal (void) const			{ return !myFunc; }	// Any var not in a function (but can be in a block).
	bool						IsTopGlobal (void) const		{ return IsGlobal() && blockId == DELTA_MAIN_BLOCK_VALUE; }
	bool						IsStatic (void) const			{ return isStatic; }
	bool						IsFormal (void) const			{ return isFormal; }
	bool						IsLocal (void) const			{ return myFunc && !isFormal && scope != 0; }	
	bool						IsTempVar (void) const			{ return !isUserDefinedFunc && name[0] == DELTA_HIDDENVAR_ID_PREFIX && name[1] == 't'; }
	bool						IsLibrarySymbol (void) const	{ return IsLibraryFunction() || IsLibraryConst(); }
	bool						IsInitialised (void) const	
									{ return initialisedCounter != 0; }
	void						SetInitialised (void)		
									{ ++initialisedCounter; }
	void						ResetInitialised (bool possibly = true)		
									{ DASSERT(initialisedCounter); if (!--initialisedCounter) possiblyUninitialised = possibly; }
	void						SetSureUninitialised (void)		
									{ DASSERT(!initialisedCounter); possiblyUninitialised = false; }
	void						CheckUninitialised (void) const;

	util_ui16					GetOffset (void) const			{ return offset; }
	void						SetOffset (util_ui16 _offset)	{ DASSERT(!offset); offset = _offset; }
	void						ShiftOffset (util_ui16 shift)	{ offset += shift; }
	util_ui16					GetBlockId (void) const			{ return blockId; }

	// All.
	util_ui16					GetScope (void) const
									{ return scope; }
	void						SetScope (util_ui16 _scope);

	bool						IsOfSameScopeAs (util_ui16 _scope) const
									{ return scope == _scope; }
	bool						IsOfInnerScopeThan (util_ui16 _scope) const
									{ return scope > _scope; }
	bool						IsOfSameOrInnerScopeThan (util_ui16 _scope) const
									{ return scope >= _scope; }
	bool						IsOfOuterScopeThan (util_ui16 _scope) const
									{ return scope < _scope; }
	bool						IsOfSameOrOuterScopeThan (util_ui16 _scope) const
									{ return scope <= _scope; }
	
	DeltaSymbol (void) :
		scope					(0),
		line					(DELTA_CANTBE_A_SOURCE_LINE),
		blockId					(0),
		myFunc					(NIL_SYMBOL),
		isStatic				(false),
		next					(NIL_SYMBOL),
		offset					(0),
		isFormal				(false),
		initialisedCounter		(0),
		possiblyUninitialised	(false),
		funcVarInfo				((ImportedFuncVarInfo*) 0),
		libVarInfo				((ImportedByteCodeLibInfo*) 0),
		isUserDefinedFunc		(false),
		funcClass				(DELTA_FUNCCLASS_NOTAFUNCTION),
		funcSig					((FuncSig*) 0),
		funcAccess				((FuncAccess*) 0),
		isLibraryFunc			(false),
		isOrphanMethod			(false),
		methodVar				((DeltaExpr*) 0),
		libFuncSigs				((DeltaLibraryFuncSignatures*) 0),
		serial					(0),
		totalBlocks				(0),
		addr					(DELTA_CANTBE_A_FUNCTION_ADDRESS),
		closureInfo				((ClosureInfo*)0),
		isLibraryConst			(false),
		isUserDefinedConst		(false),
		constExpr				((DeltaExpr*) 0),
		isClosureVarAccess		(false),
		closureVarAccessed		((DeltaSymbol*) 0) {}

		DeltaSymbol (const std::string& name, DeltaSymbolType type);
		~DeltaSymbol();
};

typedef std::list<DeltaSymbol*> DeltaSymboList;

//------------------------------------------------------------------

class DeltaSymbolTable {

	friend class DeltaCodeGenerator;

	private:
	struct GiveStaticVarOffsetFunctor : public std::binary_function<DeltaSymbol*, util_ui16*, bool> {
		bool operator()(DeltaSymbol* var, util_ui16* offset) const 
			{ var->SetOffset((*offset)++); return true; }
	};

	static DeltaSymbolTable*	singletonPtr;
	DeltaSymbol*				Table[DELTA_SYMBOL_TABLE_HASH_SIZE];
	DeltaSymboList				allSymbols;		// All symbols.
	DeltaSymboList				staticVars;		// Only static variables.
	DeltaSymboList				globalVars;		// All global vars excluding statics.
	DeltaSymbol*				inactiveHead;
	DeltaSymbol*				inactiveTail;
	std::stack<util_ui32>		tempCounterStack;
	util_ui32					tempCount;
	util_ui32					tempFuncCount;

	void				AppendInactiveSymbols (DeltaSymbol* from, DeltaSymbol* to);
	void				ValidateBucket (DeltaSymbol* bucket) const;

	bool				IsTempVar (const std::string& id) const
							{ return ustrprefix(DELTA_TEMPVAR_NAME_PREFIX, id); }

	///////////////////////////////////////////////////////////////////

	public:
	DeltaSymbol*		NewSymbol (const std::string& name, bool storage = true);
	DeltaSymbol*		NewStatic (const std::string& name);
	DeltaSymbol*		NewTemp (bool storage = true);

	const std::string	NewTempName (void);	
	void				ResetTemp (void);
	const std::string	NewTempFuncName (bool isMethod = false);
	void				ResetFuncTemp (void);

	void				PushAndResetTempCounter (void);
	void				PopAndRestoreTempCounter (void);


	void				Install (DeltaSymbol* symbol, bool installAtOuterScope = false);
	void				Uninstall (const std::string& name);
	DeltaSymbol*		Lookup (const std::string& name);
	DeltaSymbol*		Lookup (const std::string& name, util_ui16 scope);

	void				InstallLibraryFunctions (const std::list<UPTR(const char*)>* externFuncs);
	void				InactivateSymbols (util_ui16 scope);

	void				RegisterClosureFunctionVar (DeltaSymbol* funcVar, DeltaSymbol* clientFunc);

	util_ui16			GetTotalStaticVars (void) const
							{ return (util_ui16) staticVars.size(); }
	void				GiveOffsetsToStaticVariables (util_ui16 totalGlobals);
	void				ShiftOffsetsOfGlobalVariablesAfterStatics (void);
	void				ForeachGlobalVar (void (*f)(DeltaSymbol*, void*), void* closure = (void*) 0);

	void				CleanUp (void);
	void				ValidateAll (void) const;

	////////////////////////////////////////////

	USINGLETON_APISTYLE_DECLARE_PUBLICSTDMETHODS

	static DeltaSymbolTable*	
						GetPtr (void) 
							{ DASSERT(singletonPtr); return singletonPtr; }

	DeltaSymbolTable (void);
	~DeltaSymbolTable();
};

////////////////////////////////////////////

#define	DELTASYMBOLS	GetDeltaSymbols()

inline DeltaSymbolTable& GetDeltaSymbols (void) {
	DASSERT(DeltaSymbolTable::GetPtr());
	return *DeltaSymbolTable::GetPtr();
}

#endif	// Do not add stuff beyond this point.
