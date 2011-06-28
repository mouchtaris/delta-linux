// RcAttrRDParser.h
// Attribute expression parser to be used from within
// C++ programs for the Resource Loader.
// ScriptFigher Project.
// A. Savidis, January 2007.
//

#ifndef	RCATTRRDPARSER_H
#define	RCATTRRDPARSER_H

#include "DDebug.h"
#include "utypes.h"
#include "RcAttrLexAn.h"
#include "DeltaTable.h"
#include "usingleton.h"
#include "DeltaValue.h"
#include "RcLoaderDefs.h"
#include <string>

//////////////////////////////////////////////////////////////////
//	Original grammar:		
//	E -> A
//	A -> id | A[num] | A.id
//
//	After left factoring:	
//	E -> A
//	A -> id B
//	B -> [num]B	| .id B | empty
//
//////////////////////////////////////////////////////////////////

class RCLOAD_CLASS RcAttrParser {

	//*********************************
	public:
	enum ValueType { Number = 0, String = 1, Boolean = 2, Table = 3, Undef = 4 };

	class RCLOAD_CLASS Value {
		private:
		ValueType				type;
		DeltaNumberValueType	num;
		std::string				str;
		bool					boo;
		DeltaTable*				tab;
		std::string				toString;

		public:
		void 				Set (DeltaNumberValueType x) 
								{ type = Number; num = x; toString = uconstructstr("%f", x); }

		void 				Set (const std::string&  x) 
								{ type = String; str = x; toString = str; }
		
		void 				Set (bool x) 
								{ type = Boolean; boo = x; toString = ubool2conststr(x); }

		void 				Set (DeltaTable*  x);

		ValueType				GetType (void) const { return type; }

		DeltaNumberValueType	GetNumber (void) const
									{ DASSERT(type==Number); return num; }

		const std::string 		GetString (void) const
									{ DASSERT(type==String); return str; }

		bool 					GetBoolean (void) const
									{ DASSERT(type==Boolean); return boo; }

		const DeltaTable*		GetTable (void) const
									{ DASSERT(type==Table); return tab; }
				
		const std::string		ToString (void) const
									{ return toString; }

		void operator=(const Value& v) 
					{ new (this) Value(v); }

		Value (void): type(Undef), num(0), str(""), boo(false), tab((DeltaTable*) 0), toString(""){}
		Value (const Value& v): type(v.type), num(v.num), str(v.str), boo(v.boo), tab(v.tab), toString(v.toString){}
	};

	//*********************************
	private:
	RcAttrLexAn				lexan;
	RcAttrLexAn::TokenType	tokenType;

	bool					hasError;
	std::string				errorMsg;
	std::string				unparsedInput;	// Unparsed form of successfuly parsed input.
	Value					result;			// Parsing outcome.
	DeltaTable*				resources;		// Reference resources.

	// RDP functions following the grammar.
	// We apply predictive RDP (one look-ahead), failure on error.
	//
	void					GetNextToken (void)
								{ tokenType = lexan.GetToken(); }

	RcAttrLexAn::TokenType	CurrTokenType (void) const 
								{ return tokenType; }

	bool					Match (RcAttrLexAn::TokenType token);
	bool					E_parse(DeltaTable* attr, Value& val);
	bool					A_parse(DeltaTable* attr, Value& val);
	bool					B_dispatcher(DeltaValue& attr, Value& val);
	bool					B1_parse(DeltaTable* attr, Value& val);
	bool					B2_parse(DeltaTable* attr, Value& val);

	void					Error (const char* format, ...);

	//*********************************
	public:
	bool				Parse (DeltaTable* resources, const char* expr);

	const Value			GetResult (void) const 
							{ DASSERT(!HasError()); return result; }

	bool				HasError (void) const
							{ return hasError; }

	void				ResetError (void) { hasError = false; }

	const std::string	GetErrorMsg (void) const
							{ DASSERT(HasError()); return errorMsg; }
	
	//*********************************

	RcAttrParser (void);
	~RcAttrParser(){}
};

//////////////////////////////////////////////////////////////////
// Helper macro and traits for extracting configuration variables.
//

#define	RCREAD_CONFIG_VAR(parser, data, var, id)		\
	if (parser.Parse(data, id))							\
		SetRcAttrValue(var, id, parser.GetResult());	\
	else												\
		parser.ResetError()

template <class T> struct RcAttrTraits{};
template<> struct RcAttrTraits<std::string> {
	const std::string operator()(const RcAttrParser::Value& attr) const
		{ return attr.GetString(); }
};

template<> struct RcAttrTraits<bool> {
	bool operator()(const RcAttrParser::Value& attr) const
		{ return attr.GetBoolean(); }
};

template<> struct RcAttrTraits<DeltaNumberValueType> {
	DeltaNumberValueType operator()(const RcAttrParser::Value& attr) const
		{ return attr.GetNumber(); }
};

template <typename T> 
void SetRcAttrValue (T& var, const char* id, const RcAttrParser::Value& attr) 
	{ var = RcAttrTraits<T>()(attr); }

//////////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.


