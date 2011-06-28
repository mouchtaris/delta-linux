// LibraryDefsParser.h
// Parser for library deifnitions.
// ScriptFighter Project.
// A. Savidis, September 2009.
//

#ifndef	LIBRARYDEFSPARSER_H
#define	LIBRARYDEFSPARSER_H

#include "LibraryDefsScanner.h"
#include "LibraryTypes.h"

///////////////////////////////////////////////////////////

class DeltaLibraryDefsParser : public DeltaLibraryDefsErrorReporting {

	private:
	typedef DeltaLibraryDefsScanner::Token	Token;
	typedef std::list<std::string>			StringList;

	DeltaLibraryDefsScanner	scanner;
	bool					useLookAhead;
	Token					lookAhead;
	std::string				idTokenContent;
	
	// Signatures.
	ucallbackwithclosure<void(*)(void*)>													onNewSig;
	ucallbackwithclosure<void(*)(const DeltaLibraryTypeInfo&, const std::string&, void*)>	onNewArg;
	ucallbackwithclosure<void(*)(const DeltaLibraryTypeInfo&, void*)>						onDoneSig;
	ucallbackwithclosure<void(*)(bool, void*)>												onDoneSigs;

	// User-defined types
	ucallbackwithclosure<void(*)(const std::string&, const StringList&, void*)>	onNewBase;

	void				NotifyNewSig (void)										
							{ onNewSig(); }
	void				NotifyNewArg (const DeltaLibraryTypeInfo& argType, const std::string& id)	
							{ onNewArg(argType, id); }
	void				NotifyDoneSig (const DeltaLibraryTypeInfo& retType)						
							{ onDoneSig(retType); }
	void				NotifyDoneSigs (bool postError)			
							{ onDoneSigs(postError); }

	void				Reset (void)
							{ useLookAhead = false, lookAhead = DeltaLibraryDefsScanner::Error; }
	bool				Parse_Sig (util_ui32 n);
	bool				Parse_Arg (void);
	bool				Parse_Args (void);
	bool				Parse_ArgType (DeltaLibraryTypeInfo* type);
	bool				Parse_ArgTypeBuiltinOrUserDefined (DeltaLibraryTypeInfo* type);
	bool				Parse_ArgTypeList (DeltaLibraryTypeInfo* type, bool inclusive);
	bool				Parse_PostError (void);

	// User-defined types.
	bool				Parse_TypeSpec (
							std::string*		typeName, 
							StringList*			namespacePath, 
							const std::string&	context
						);
	void				NotifyNewBase (const std::string& name, const StringList& namespacePath)	
							{ onNewBase(name, namespacePath); }

	Token				CurrToken (void) const 
							{ return lookAhead; }
	Token				NextToken (void);
	void				RetractToken (void) 
							{ DASSERT(!useLookAhead); useLookAhead = true; }

	///////////////////////////////////////////////////////////

	public:
	static bool			TagStringToTypeTag (const std::string& tagString, DeltaTypeTag* tag);
	static const std::string	
						TypeTagToTagString (DeltaTypeTag tag);
	static bool			IsIdent (const std::string& id);

	// Signatures.
	void				SetOnNewSig	(void(*f)(void*), void* c)	
							{ onNewSig.set(f,c); }
	void				SetOnNewArg (void(*f)(const DeltaLibraryTypeInfo&, const std::string&, void*),	void* c)	
							{ onNewArg.set(f,c); }
	void				SetOnDoneSig (void(*f)(const DeltaLibraryTypeInfo&, void*), void* c)	
							{ onDoneSig.set(f,c); }
	void				SetOnDoneSigs (void(*f)(bool, void*), void* c)	
							{ onDoneSigs.set(f,c); }
	void				ParseSigDefinition (const std::string& input);
	static bool			SplitSigDefinition (const std::string& def, std::string* name, std::string* sigsDef);

	enum ConstType { ConstString = 1, ConstNumber = 2, ConstError = 0 };
	static ConstType	ParseConstValue  (const std::string& val, double* num, std::string* str, std::string* error);
	static bool			SplitConstDefinition (const std::string& def, std::string* name, std::string* value);

	// User-defined types.
	void				SetOnNewBase (void(*f)(const std::string&, const StringList&, void*), void* c)	
							{ onNewBase.set(f,c); }
	bool				ParseBaseTypes (const std::string& input);
	static bool			SplitTypeDefinition (const std::string& def, std::string* name, std::string* basesDef);

	DeltaLibraryDefsParser (void) : 
		useLookAhead(false), 
		lookAhead(DeltaLibraryDefsScanner::Error) {}
	~DeltaLibraryDefsParser(){}
};

///////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.

