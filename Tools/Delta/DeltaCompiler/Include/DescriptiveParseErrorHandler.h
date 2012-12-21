// DescriptiveParseErrorHandler.h
// A method to support descriptive parse erros for S/R parsers
// without relying on implementation-dependent aspects (only reductions).
// ScriptFighter Project.
// A. Savidis, July 2007.
//

#ifndef	DESCRIPTIVEPARSEERRORHANDLER_H
#define	DESCRIPTIVEPARSEERRORHANDLER_H

#include "CompilerComponentDirectory.h"
#include "utypes.h"
#include "DDebug.h"
#include "Symbol.h"
#include "ucallbacks.h"

#include <stack>
#include <map>
#include <string>

//////////////////////////////////////////////////////
// Macros to be injected in the parser definition file.
// SM:	start main symbol
// EM:	end main symbol (pop all till its start)
// SG:	start group symbol
// EG:	end group symbol (pop all till its start)
// PE:	push expected / examined symbol
// OE:  an optional version of PE, removing with next push action automatically
// EI:	end if expected symbol (pop it)
//

#define	SM(id)	\
	DESCRIPTIVE_ERROR_HANDLER.Push(id, DescriptiveParseErrorHandler::role_main_symbol)

#define	EM(id)	\
	DESCRIPTIVE_ERROR_HANDLER.PopTill(id, DescriptiveParseErrorHandler::role_main_symbol)

#define	SG(id)	\
	DESCRIPTIVE_ERROR_HANDLER.Push(id, DescriptiveParseErrorHandler::role_group_symbol)

#define	EG(id)	\
	DESCRIPTIVE_ERROR_HANDLER.PopTill(id, DescriptiveParseErrorHandler::role_group_symbol)

#define	PE(id)	\
	DESCRIPTIVE_ERROR_HANDLER.Push(id, DescriptiveParseErrorHandler::role_any_expected_symbol)

#define	OE(id)	\
	DESCRIPTIVE_ERROR_HANDLER.Push(id, DescriptiveParseErrorHandler::role_any_expected_temp_symbol)

#define	EI(id)	\
	DESCRIPTIVE_ERROR_HANDLER.PopIf(id, DescriptiveParseErrorHandler::role_any_expected_symbol)

#define	PE2(id1,id2) \
		PE(id1); PE(id2)

#define	PE3(id1,id2, id3) \
		PE2(id1, id2); PE(id3)

//////////////////////////////////////////////////////
// The identifiers of the grammar symbols. S_ stands for
// symbols, while T_ stands for tokens.
//
#define	S_FUNC					1
#define	T_FUNC					2
#define	S_FNAME					3
#define	S_FARGS					4
#define	S_BLOCK					5
#define	T_LPAR					6
#define	T_RPAR					7
#define	T_IDENT					8
#define	T_COMMA					9

#define	S_ASSRT					10
#define	S_EXPR					11
#define	S_STMT					12
#define	T_SEMI					13
#define	T_BREAK					14
#define	T_CONT					15

#define	T_ASSIGN				16
#define	S_CONST					17
#define	S_ASSIGNEXPR			18
#define	T_ADDA					19
#define	T_SUBA					20
#define	T_MODA					21
#define	T_DIVA					22
#define	T_MULA					23

#define	S_RELAT					24
#define	T_GT					25
#define	T_GE					26
#define	T_LT					27
#define	T_LE					28
#define	T_EQ					29
#define	T_NE					30

#define	S_LOGICAL				31
#define	T_OR					32
#define	T_AND					33

#define	S_ARITH					34
#define	T_ADD					35
#define	T_SUB					36
#define	T_MOD					37
#define	T_DIV					38
#define	T_MUL					39

#define	S_TERNARY				40
#define	T_QMARK					41
#define	T_COLON					42

#define	S_LVAL					43
#define	T_PLUSPLUS				44
#define	T_MINUSMINUS			45
#define	T_NOT					46
#define	T_BAR					47
#define	S_CALL					48
#define	S_AARGS					49
#define	T_STATIC				50
#define	T_LOCAL					51
#define	T_GLOBAL				52
#define	T_LAMBDA				54

#define	T_DOT					55
#define	T_LB					56
#define	T_RB					57
#define	T_DLB					58
#define	T_DRB					59
#define	T_DDOT					60
#define	S_DOTINDEX				61
#define	T_OPINDEX				62
#define	T_SELF					63
#define	T_ARGUMENTS				64
#define	S_TABLE					65
#define	S_TABLIST				66
#define	S_INDEXEXPR				67
#define	S_INDICES				68
#define	T_LBC					69
#define	T_RBC					70
#define	S_ELIST					71
#define	N_EMPTY					72

#define	S_WHILE					73
#define	S_FOR					74
#define	S_IF					75
#define	S_ELSE					76
#define	T_WHILE					77
#define	T_FOR					78
#define	T_IF					79
#define	T_ELSE					80
#define	T_RET					81
#define	S_RET					82

#define	S_TRY					83
#define	S_THROW					84
#define	T_TRY					85
#define	T_TRAP					86
#define	T_THROW					87
#define	T_STRINGCONST			88
#define	T_SET					89
#define	T_GET					90
#define	T_STRINGIFY				91
#define	T_NEWSELF				92
#define	T_ATTRIBUTEID			93
#define	S_METHOD				94
#define	T_METHOD				95
#define	T_TDOT					96
#define	S_FUNC_ERROR			97
#define	T_FOREACH				98
#define	S_FOREACH				99
#define	S_NAMESPACE				100
#define	T_LPAR_OR_TDOT			101
#define	S_BRACKET_INDEX			102
#define	T_LAMBDA_REF			103
#define	T_LBC_OR_COLON			104
#define	T_KWDIDENT				105
#define	T_IDENT_OR_KWDIDENT		106
#define	T_ATTRIBUTE				107
#define	T_SEMI_OR_COMMA			108
#define	T_LOCAL_OR_FUNCNAME		109
#define	T_TABLE_ELEM			110

inline util_ui32 S_FUNC_(util_ui8 c) 
	{ return c == DELTA_FUNCCLASS_METHOD ? S_METHOD : S_FUNC; }

inline	util_ui32 S_FUNC_(DeltaSymbol* f)
	{ return f ? S_FUNC_(DPTR(f)->funcClass) : S_FUNC_ERROR; }

extern util_ui32 S_FUNC_(const std::string& funcClass);

inline util_ui32 T_FUNC_(util_ui8 c) 
	{ return c == DELTA_FUNCCLASS_METHOD ? T_METHOD : T_FUNC; }

inline util_ui32 T_FUNC_(DeltaSymbol* f)	
	{ return f ? T_FUNC_(DPTR(f)->funcClass) : S_FUNC_ERROR; }

inline util_ui32 T_FUNC_(const std::string& funcClass)	
	{ return S_FUNC_(funcClass); }

extern util_ui32 S_DOTINDEX_ (const std::string& tag);
extern util_ui32 S_BRACKET_INDEX_ (const std::string& tag);

//////////////////////////////////////////////////////

class DescriptiveParseErrorHandler {

	USE_COMPILER_COMPONENT_DIRECTORY();

	public:
	typedef util_ui32 grammar_symbol_t;

	enum	symbol_role_t { 
			role_main_symbol				= 0, 
			role_group_symbol				= 1,
			role_any_expected_symbol		= 2,
			role_any_expected_temp_symbol	= 3,
			role_undefined					= 4
	};

	//***************************

	private:
	struct StackItem {

		grammar_symbol_t	symCode;
		symbol_role_t		symRole;
		
		bool IsMain (void) const			{ return symRole == role_main_symbol; }
		bool IsGroup (void) const			{ return symRole == role_group_symbol; }
		bool IsAnyExpected (void) const		{ return symRole == role_any_expected_symbol; }
		bool IsAnyExpectedTemp (void) const	{ return symRole == role_any_expected_temp_symbol; }

		UOVERLOADED_VOID_ASSIGN_VIA_COPY_CONSTRUCTOR(StackItem)

		StackItem (void) : symCode(0), symRole(role_undefined){}
		StackItem (grammar_symbol_t	code, symbol_role_t		role) : symCode(code), symRole(role){}
		StackItem (const StackItem& i) : symCode(i.symCode), symRole(i.symRole){}
	};

	//***************************

	std::stack<StackItem>*					parseStack;
	typedef std::map<grammar_symbol_t, std::string>	SymbolMessageMap;
	SymbolMessageMap*						symbolMessages;
	bool									isMainAddedAutomatically;

	ucallbackwithclosure<const std::string (*)(void *)> getTokenText;

	bool				IsInitialised (void) { return parseStack && symbolMessages; }
	const std::string	GetMessage (grammar_symbol_t code);
	void				Pop (void);

	//***************************

	public:
	void				Push (grammar_symbol_t code, symbol_role_t role);
	void				PopTill (grammar_symbol_t code, symbol_role_t role);
	void				PopIf (grammar_symbol_t code, symbol_role_t role);

	const std::string	GetReport (void);
	void				Clear (void);
	void				HandleSyntaxError (void);
	void				SetGetTokenText (ucallbackwithclosure<const std::string (*)(void *)> f)	// TODO: remove when AST prevails
							{ getTokenText = f; }
	void				Initialise (void);
	void				CleanUp (void);

	DescriptiveParseErrorHandler (void);
	~DescriptiveParseErrorHandler () {}
};

//////////////////////////////////////////////////////

#define DESCRIPTIVE_ERROR_HANDLER_EX(component_directory)	\
	(*DNULLCHECK(UCOMPONENT_DIRECTORY_GET(*(component_directory), DescriptiveParseErrorHandler)))

#define DESCRIPTIVE_ERROR_HANDLER	DESCRIPTIVE_ERROR_HANDLER_EX(COMPONENT_DIRECTORY())

//////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.


