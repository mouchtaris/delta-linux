// LibraryDefsParser.cpp
// Parser for library definitions.
// ScriptFighter Project.
// A. Savidis, September 2009.
//

#include "LibraryDefsParser.h"
#include "LibraryNamespaceHolder.h"
#include "DeltaStdDefs.h"
#include "ustrings.h"

#undef	DELTANAMESPACES
#define DELTANAMESPACES (*DNULLCHECK(namespaceHolder))

//---------------------------------------------------------
// COMMON

DeltaLibraryDefsParser::Token DeltaLibraryDefsParser::NextToken (void) {
	if (useLookAhead)
		useLookAhead = false;
	else
	if ((lookAhead = scanner.GetToken()) == DeltaLibraryDefsScanner::TokenID)
		idTokenContent = scanner.GetId();
	if (lookAhead == DeltaLibraryDefsScanner::Error)
		Error(scanner.GetError());
	return lookAhead;
}

///////////////////////////////////////////////////////////

bool DeltaLibraryDefsParser::TagStringToTypeTag (const std::string& tagString, DeltaTypeTag* tag) 
	{ return DeltaFunctionReturnType::GetBasicTypeFromString(tagString, tag); }

const std::string DeltaLibraryDefsParser::TypeTagToTagString (DeltaTypeTag tag) 
	{ return DeltaFunctionReturnType::GetBasicTypeString(tag); }

///////////////////////////////////////////////////////////

bool DeltaLibraryDefsParser::IsIdent (const std::string& id) {
	DeltaLibraryDefsScanner scanner;
	scanner.Start(id);
	return	scanner.GetToken() == DeltaLibraryDefsScanner::TokenID && 
			scanner.GetToken() == DeltaLibraryDefsScanner::TokenEOF;
}

//---------------------------------------------------------
// SIGNATURES

void DeltaLibraryDefsParser::ParseSigDefinition (const std::string& input) {
	Reset();
	scanner.Start(input);
	if (scanner.IsEof()) 
		{ Error("empty signature"); return; }

	util_ui32 n = 0;
	while (Parse_Sig(n++))
		;
	if (!HasError()) {
		bool postError = Parse_PostError();
		if (!HasError())
			NotifyDoneSigs(postError);
	}
}

///////////////////////////////////////////////////////////

bool DeltaLibraryDefsParser::SplitSigDefinition (const std::string& def, std::string* name, std::string* sigsDef) {

	name->clear();
	sigsDef->clear();
	
	DeltaLibraryDefsScanner scanner;
	scanner.Start(def);

	if (scanner.GetToken() == DeltaLibraryDefsScanner::TokenID) {
		*name = def.substr(0, scanner.GetPos());
		if (!scanner.IsEof())
			*sigsDef = def.substr(scanner.GetPos());
		return true;
	}
	else 	// Not a legal libfunc definition, since it does not start with an id.
		return false;
}

///////////////////////////////////////////////////////////

bool DeltaLibraryDefsParser::SplitConstDefinition (const std::string& def, std::string* name, std::string* value) {

	name->clear();
	value->clear();
	
	DeltaLibraryDefsScanner scanner;
	scanner.Start(def);

	if (scanner.GetToken() == DeltaLibraryDefsScanner::TokenID) {
		*name  = scanner.GetId();
		*value = def.substr(scanner.GetPos());
		return true;
	}
	else 	// Not a legal libconst definition, since it does not start with an id.
		return false;
}

///////////////////////////////////////////////////////////

DeltaLibraryDefsParser::ConstType DeltaLibraryDefsParser::ParseConstValue (
		const std::string&	val, 
		double*				num, 
		std::string*		str,
		std::string*		error
	) {

	DeltaLibraryDefsScanner scanner;
	scanner.Start(val);

	DeltaLibraryDefsScanner::Token result = scanner.GetToken();
	scanner.SkipBlanks();

	switch (result) {
		case DeltaLibraryDefsScanner::TokenQSTRING:	{
			*str = scanner.GetQuotedString();
			if (!scanner.IsEof()) {
				*error = uconstructstr("expected end of input after quoted string '%s' in const value", str->c_str());
				return ConstError;
			}
			else
				return ConstString;
		}

		case DeltaLibraryDefsScanner::TokenNUMBER:	{
			*num = scanner.GetNumber();			
			if (!scanner.IsEof()) {
				*error = uconstructstr("expected end of input after number '%f' in const value", *num);
				return ConstError;
			}
			else
				return ConstNumber;
		}

		case DeltaLibraryDefsScanner::Error: 
			*error = scanner.GetError(); return ConstError;
		default:	
			*error = "expected quoted string or number in const value"; return ConstError;
	}
}

///////////////////////////////////////////////////////////

bool DeltaLibraryDefsParser::Parse_Sig (util_ui32 n) {
	if (NextToken() == DeltaLibraryDefsScanner::TokenEOF)
		return false;
	else
	if (n && CurrToken() == DeltaLibraryDefsScanner::TokenWARN) // Legal only after at least one signature.
		{ RetractToken(); return false; }
	else
	if (CurrToken() == DeltaLibraryDefsScanner::TokenLP) {
		NotifyNewSig();
		if (!Parse_Args())
			return false;
		else
		if (NextToken() == DeltaLibraryDefsScanner::TokenRP) {
			DeltaLibraryTypeInfo retType(TagVoid);
			if (Parse_ArgType(&retType))
				NotifyDoneSig(retType);
			return !HasError();
		}
		else 
			{ Error("expecting ')' after arguments"); return false; }
	}
	else 
		{ Error("expecting '(' to open signature"); return false; }
}

///////////////////////////////////////////////////////////

bool DeltaLibraryDefsParser::Parse_Arg (void) {

	if (NextToken() == DeltaLibraryDefsScanner::TokenID) {
		DeltaLibraryTypeInfo info(TagAnyValue);	// If no type defined, it matches any value.
		std::string id	= idTokenContent;
		if (Parse_ArgType(&info))
			NotifyNewArg(info, id);
		return !HasError();
	}
	else
	if (CurrToken() == DeltaLibraryDefsScanner::TokenDOTS) {
		NotifyNewArg(DeltaLibraryTypeInfo(TagAnyValues), "...");
		return true;
	}
	else {
		Error("argument id expected in signature");
		return false;
	}
}

///////////////////////////////////////////////////////////

bool DeltaLibraryDefsParser::Parse_Args (void) {
	bool hasArgs = false, hadComma = false;
	DASSERT(!HasError());
	while (true) {
		NextToken();
		if (CurrToken() == DeltaLibraryDefsScanner::TokenCOMMA) {
			if (!hasArgs)
				{ Error("unexpected ',' after '(' in args of signature"); return false; }
			else
			if (hadComma)
				{ Error("unexpected ',' after ',' in args of signature"); return false; }
			else
				hadComma = true;
		}
		else
		if (CurrToken() == DeltaLibraryDefsScanner::TokenRP)
			if (hadComma)
				{ Error("unexpected ')' after ',' in args of signature"); return false; }
			else
				{ RetractToken(); return true; }
		else 
		if (hasArgs && !hadComma)
				{ Error("expecting ',' or ')' after args of signature"); return false; }
		else {
			RetractToken();
			if (!Parse_Arg())
				return false;
			hasArgs	 = true;
			hadComma = false;
		}
	}
}

///////////////////////////////////////////////////////////

bool DeltaLibraryDefsParser::Parse_ArgTypeBuiltinOrUserDefined (DeltaLibraryTypeInfo* type) {

	std::string typeName;
	StringList	namespacePath;

	if (!Parse_TypeSpec(&typeName, &namespacePath, "in signature")) 
		{ Error("type specifier expected after ':' in signature"); return false; }

	DeltaTypeTag builtinType;
	if (namespacePath.empty() && TagStringToTypeTag(typeName, &builtinType))
		{ type->Set(builtinType); return true; }
	else {
		const DeltaLibraryUserDefinedType* userType;
		userType =	namespacePath.empty() ? 
						DELTANAMESPACES.LookupType(typeName) :
						DELTANAMESPACES.LookupType(
							namespacePath, 
							typeName
						);
		if (userType)
			{ type->Set(userType); return true; }
		else
			return false;
	}
}

///////////////////////////////////////////////////////////

bool DeltaLibraryDefsParser::Parse_ArgTypeList (DeltaLibraryTypeInfo* type, bool inclusive) {
	
	if (NextToken() != DeltaLibraryDefsScanner::TokenLBRACE) {
		Error(
			uconstructstr(
				"'{' expected after %s in signature",
				inclusive ? "'+' (inclusive type list)" : "'-' (exclusive type list)"
			)
		); 
		return false; 
	}

	DeltaLibraryTypeInfo::TypeList typeList;
	while (true) {
		DeltaLibraryTypeInfo altType(TagAnyValue);
		if (!Parse_ArgTypeBuiltinOrUserDefined(&altType))
			return false;
		typeList.push_back(altType);
		if (NextToken() == DeltaLibraryDefsScanner::TokenCOMMA)
			continue;
		else
		if (CurrToken() != DeltaLibraryDefsScanner::TokenRBRACE)
			{ Error("'}' expected after type list in signature"); return false; }
		else
			{ type->Set(typeList, inclusive); return true; }
	}
}

///////////////////////////////////////////////////////////

bool DeltaLibraryDefsParser::Parse_ArgType (DeltaLibraryTypeInfo* type) {
	if (NextToken() == DeltaLibraryDefsScanner::TokenCOLON)
		switch (NextToken()) {
			case DeltaLibraryDefsScanner::TokenID:
				RetractToken(); return Parse_ArgTypeBuiltinOrUserDefined(type);
			case DeltaLibraryDefsScanner::TokenPLUS:
				return Parse_ArgTypeList(type, true);
			case DeltaLibraryDefsScanner::TokenMINUS:
				return Parse_ArgTypeList(type, false);
			default:
				{ Error("type expected after ':' in signature"); return false; }
		}
	else 
		{ RetractToken(); return true; }		
}

///////////////////////////////////////////////////////////

bool DeltaLibraryDefsParser::Parse_PostError (void) {
	if (NextToken() == DeltaLibraryDefsScanner::TokenWARN)	// This causes a warning, not an error, when the sig is not matched.
		return false;
	else {
		RetractToken();
		return true;
	}
}

//---------------------------------------------------------
// USER-DEFINED TYPES

bool DeltaLibraryDefsParser::Parse_TypeSpec (
		std::string*		typeName, 
		StringList*			namespacePath, 
		const std::string&	context
	) {
	
	namespacePath->clear();
	if (NextToken() == DeltaLibraryDefsScanner::TokenID)
		namespacePath->push_back(idTokenContent);
	else 
		{ Error("type name expected in " + context); return false; }

	while (true) {
		NextToken();
		if (CurrToken() == DeltaLibraryDefsScanner::TokenSCOPE)
			if (NextToken() == DeltaLibraryDefsScanner::TokenID) 
				namespacePath->push_back(idTokenContent);
			else 
				{ Error("name expected after '::' in " + context); return false; }
		else {
			RetractToken();
			DASSERT(!namespacePath->empty());
			*typeName = namespacePath->back();
			namespacePath->pop_back();
			return true;
		}
	}
}

///////////////////////////////////////////////////////////

bool DeltaLibraryDefsParser::ParseBaseTypes (const std::string& input) {

	scanner.Start(input);
	NextToken();
	if (CurrToken() == DeltaLibraryDefsScanner::TokenEOF)
		return true;	// No base types
	else
	if (CurrToken() != DeltaLibraryDefsScanner::TokenCOLON)
		{ Error("expecting ':' after name"); return false; }

	bool hasBases = false, hadComma = false;
	DASSERT(!HasError());

	while (true) {
		NextToken();
		if (CurrToken() == DeltaLibraryDefsScanner::TokenCOMMA) {
			if (!hasBases)
				{ Error("unexpected ',' after ':' in base types"); return false; }
			else
			if (hadComma)
				{ Error("unexpected ',' after ','in base types"); return false; }
			else
				hadComma = true;
		}
		else
		if (CurrToken() == DeltaLibraryDefsScanner::TokenEOF)
			if (hadComma)
				{ Error("unexpected EOF after ',' in base types"); return false; }
			else
			if (!hasBases)
				{ Error("unexpected EOF after ':' with no base types"); return false; }
			else
				{ RetractToken(); return true; }
		else
		if (hasBases && !hadComma)
			{ Error("expecting ',' or EOF after base typesn"); return false; }
		else {
			RetractToken();
			StringList	namespacePath;
			std::string typeName;
			if (!Parse_TypeSpec(&typeName, &namespacePath, "base types"))
				return false;
			NotifyNewBase(typeName, namespacePath);
			hasBases = true;
			hadComma = false;
		}
	}
}

///////////////////////////////////////////////////////////

bool DeltaLibraryDefsParser::SplitTypeDefinition (const std::string& def, std::string* name, std::string* basesDef) 
	{ return SplitSigDefinition(def, name, basesDef); }

//---------------------------------------------------------
