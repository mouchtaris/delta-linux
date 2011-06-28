// LibraryNamespace.h
// Namespaces for library functionsAndConsts in Delta.
// ScriptFighter Project.
// A. Savidis, August 2009.
//

#ifndef	LIBRARYNAMESPACE_H
#define	LIBRARYNAMESPACE_H

#include "DeltaStdDefs.h"
#include "LibraryTypes.h"
#include "Symbol.h"
#include <functional>
#include <string>
#include <map>

#define	DELTA_LIBRARYNAMESPACE_GLOBAL	"$global"	// Reserved hidden name for global namespace

///////////////////////////////////////////////////////////

class DeltaLibraryNamespace {

	public:
	typedef std::map<std::string, DeltaSymbol*>							Symbols;
	typedef std::map<std::string, DeltaLibraryNamespace*>				Namespaces;
	typedef std::map<std::string, const DeltaLibraryUserDefinedType*>	Types;
	typedef std::list<std::string>										StringList;

	static void OnNewBase (const std::string& name, const StringList& namespacePath, void* closure);

	private:
	Symbols							functionsAndConsts;		// All functionsAndConsts belonging to this namespace.
	Types							types;			// All user-defined types indexed by name (not path).
	Namespaces						namespaces;		// All inner (directly enclosed) namespaces.

	const DeltaLibraryNamespace*	parent;			// Outer (enclosing) namespace.
	std::string						name;
	std::string						fullyQualifiedName;
	std::string						fullPathWithSeparator;

	public:
	typedef std::list<std::string>	NameList;

	const std::string		GetName (void) const 
								{ return name; }
	const std::string		GetFullyQualifiedName (void) const
								{ return fullyQualifiedName; }
	const std::string		GetFullPathWithSeparator (void) const
								{ return fullPathWithSeparator; }

	DeltaSymbol*			IssueConst (const std::string& constDef);			// Parses and installs in symbol table too.
	DeltaSymbol*			IssueFunction (const std::string& funcProto);		// Parses and installs in symbol table too.
	DeltaSymbol*			LookupFunctionOrConst (const std::string& id);				// Get a function from current namespace.
	const Symbols&			GetFunctionsAndConsts (void) const
								{ return functionsAndConsts; }

	const DeltaLibraryUserDefinedType*
							IssueType (const std::string& typeDef);				// Parses, creates and returns new user-defined type.
	const DeltaLibraryUserDefinedType*
							LookupType (const std::string& typeName) const;		// Get a type from its name in current namespace.

	DeltaLibraryNamespace*	NewNamespace (const std::string& name);				// Added as nested (child).
	DeltaLibraryNamespace*	LookupNamespace (const std::string& _name);			// Get a directly enclosed namespace. 
	DeltaLibraryNamespace*	LookupNamespace (const NameList& namespacePath);	// Get an indirectly enclosed namespace.
	const DeltaLibraryNamespace*	
							LookupNamespace (const std::string& _name) const;
	const DeltaLibraryNamespace*	
							LookupNamespace (const NameList& namespacePath) const;
	const Namespaces&		GetInnerNamespaces (void) const
								{ return namespaces; }

	void					Clear (void);

	DeltaLibraryNamespace (
		const std::string&		_name, 
		const DeltaLibraryNamespace* _parent = (DeltaLibraryNamespace*) 0
	);
	~DeltaLibraryNamespace();
};

///////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.


