// LibraryNamespaceHolder.h
// Library namespace holder for Delta.
// ScriptFighter Project.
// A. Savidis, August 2009.
//

#ifndef	LIBRARYNAMESPACEHOLDER_H
#define	LIBRARYNAMESPACEHOLDER_H

#include "CompilerComponentDirectory.h"
#include "LibraryNamespace.h"
#include <functional>
#include <string>
#include <map>
#include <list>

///////////////////////////////////////////////////////////

class DeltaLibraryNamespaceHolder : public ucomponentdirectoryclient {

	friend class DeltaLibraryNamespace;

	public:
	typedef std::list<std::string>			NameList;
	typedef std::list<UPTR(const char*)>	LibrarySpecifications;
	typedef DeltaLibraryNamespace::Namespaces	Namespaces;

	private:
	typedef DeltaLibraryNamespace::Symbols		Symbols;
	typedef std::list<DeltaLibraryNamespace*>	NamespaceStack;

	DeltaLibraryNamespace*	globalNamespace;							// Everything belongs to it. It is be default opened.
	Symbols					directlyVisibleFunctionAndConsts;			// Those belonging to opened namespaces (including the global).
	Namespaces				openedNamespaces;							// Those opened (defined via 'using'). Indexed by full name.
	Namespaces				directlyVisibleNamespaces;					// Those enclosed directly in opened namespaces. Indexed by full name.
	NamespaceStack			namespaceStack;								// Creation time.

	const DeltaLibraryNamespace*	
							GetGlobalNamespace (void) const
								{ DASSERT(globalNamespace); return globalNamespace; }
	DeltaLibraryNamespace*	GetCurrentNamespace (void) 
								{ DASSERT(!namespaceStack.empty()); return namespaceStack.front(); }
	const DeltaLibraryNamespace*	
							GetCurrentNamespace (void) const
								{ DASSERT(!namespaceStack.empty()); return namespaceStack.front(); }

	bool					Open (DeltaLibraryNamespace* ns);

	void					EnteringNamespace (const std::string& name);	// New namespace defined and entering.
	void					ExitingNamespace (void);						// Exiting from inner namespace.
	void					IssueConst (const std::string& constDef);		// At current namespace.
	void					IssueFunction (const std::string& funcProto);	// At current namespace.
	void					IssueType (const std::string& funcDef);			// At current namespace.

	///////////////////////////////////////////////////////////

	public:

	static const std::string
							MakeFullyQualifiedName (const NameList& namespacePath);

	// All lookup functions below post a comp error 
	// when the requested item is not found!

	DeltaLibraryNamespace*	LookupNamespace (const std::string& name);							// Must be at global namespace or any opened namespace.
	DeltaSymbol*			LookupFunctionOrConst (const std::string& name);					// Must be at global namespace or any opened namespace.
	DeltaSymbol*			LookupFunctionOrConstAtGlobalNamespace (const std::string& name);	// Must be at the global namespace.
	DeltaSymbol*			LookupFunctionOrConst (												// Must at a namespace somehow visible through namespacePath.
								const NameList&		namespacePath, 
								const std::string&	name
							);
	DeltaSymbol*			LookupFunctionOrConstAtGlobalNamespace (							// Must be at a namespace visible only from the global namespace.
								const NameList&		namespacePath, 
								const std::string&	name
							);
	const DeltaLibraryUserDefinedType*
							LookupType (const std::string& typeName) const;						// Must be in current or any of its enclosing namespaces.
	const DeltaLibraryUserDefinedType*
							LookupType (
								const NameList&		namespacePath,								// Must be in a namespace path visible from global namespace.
								const std::string&	typeName
							) const;					

	//***********************

	DeltaLibraryNamespace*	GetGlobalNamespace (void) 
								{ DASSERT(globalNamespace); return globalNamespace; }

	bool					Open (const std::string& name);										// Must be in an opened namespace.
	bool					Open (const NameList& namespacePath);								// Opens an inner namespace inside an opened namespace.
	const Namespaces&		GetOpened (void) const 
								{ return openedNamespaces; }
	void					Initialise (void);
	void					Install (const LibrarySpecifications& libs);
	void					CleanUp (void);

	DeltaLibraryNamespaceHolder (ucomponentdirectory* direcory);
	~DeltaLibraryNamespaceHolder();
};

///////////////////////////////////////////////////////////

#define DELTANAMESPACES_EX(component_directory)	\
	(*DNULLCHECK(UCOMPONENT_DIRECTORY_GET(*(component_directory), DeltaLibraryNamespaceHolder)))

#define DELTANAMESPACES	DELTANAMESPACES_EX(GET_COMPONENT_DIRECTORY())

///////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.


