// VMCompLib.h
// Virtual machine compiler related functions.
// ScriptFigher Project.
// A. Savidis, December 1999.
// Moved here from VMRegLib. 
//

#ifndef	VMCOMPLIB_H
#define	VMCOMPLIB_H

#include "DeltaString.h"
#include "DeltaLibraryObjectCreator.h"
#include "TreeNode.h"
#include "ucallbacks.h"

///////////////////////////////////////////////////////////////////////////

extern void		Install_DeltaVMComp_Lib (DeltaObject* mainAPI);
extern void		CleanUp_DeltaVMComp_Lib (void);

///////////////////////////////////////////////////////////////////////////
// BuildDependenciesIFace
//
class DVM_CLASS BuildDependenciesIFace {
	private:
	typedef BuildDependenciesIFace* (*ConstructorFunc)(void);
	typedef void (*DestructorFunc)(BuildDependenciesIFace*);

	static ConstructorFunc	ctor;
	static DestructorFunc	dtor;

	protected:
	static void Install (ConstructorFunc create, DestructorFunc destroy);

	public:
	enum ResolutionType {
		NotFound	=	1,
		OneFound	=	2,
		ManyFound	=	3
	};
	typedef std::pair<std::string, ResolutionType>	Dependency;
	typedef std::list<Dependency>					Dependencies;

	virtual bool				Extract(const std::string&	byteCodePath, const std::string& sourceFile) = 0;
	virtual const Dependencies	GetDependencies (void) const = 0;

	static BuildDependenciesIFace*	New (void);
	static void						Delete (BuildDependenciesIFace* instance);

	virtual ~BuildDependenciesIFace() {}
};

//-----------------------------------------------------------------------

template<class T> class DefaultBuildDependenciesIFace : public BuildDependenciesIFace {
	private:
	T* impl;
	Dependencies dependencies;

	static BuildDependenciesIFace*	New (void) { return DNEWCLASS(DefaultBuildDependenciesIFace, (DNEW(T))); }
	static void						Delete (BuildDependenciesIFace* instance) { DDELETE(instance); }

	DefaultBuildDependenciesIFace (T* impl) : impl(impl) {}
	~DefaultBuildDependenciesIFace () { DDELETE(impl); }

	public:
	bool Extract(const std::string&	byteCodePath, const std::string& sourceFile) {
		typename T::Dependencies deps;
		bool result = DPTR(impl)->Extract(byteCodePath, sourceFile, &deps);
		if (result) {
			dependencies.clear();
			for (typename T::Dependencies::const_iterator i = deps.begin(); i != deps.end(); ++i) {
			ResolutionType type;
				switch (i->second) {
					case T::NotFound:	type = NotFound;	break;
					case T::OneFound:	type = OneFound;	break;
					case T::ManyFound:	type = ManyFound;	break;
					default : DASSERT(false);
				}
				dependencies.push_back(Dependency(i->first, type));
			}
		}
		return result;
	}
	const BuildDependenciesIFace::Dependencies	GetDependencies (void) const { return dependencies; }
	static void	Install (void) { BuildDependenciesIFace::Install(New, Delete); }
};

#define INSTALL_DEFAULT_BUILDDEPENDENCIESIFACE(class)	DefaultBuildDependenciesIFace<class>::Install()

///////////////////////////////////////////////////////////////////////////
// CompilerIFace
//
class DVM_CLASS CompilerIFace {
	private:
	typedef CompilerIFace*	(*ConstructorFunc)	(void);
	typedef void			(*DestructorFunc)	(CompilerIFace*);
	typedef void			(*InitialiseFunc)	(void);
	typedef void			(*CleanUpFunc)		(void);

	static ConstructorFunc	ctor;
	static DestructorFunc	dtor;
	static InitialiseFunc	initialiser;
	static CleanUpFunc		cleaner;

	protected:
	static void					Install (
									ConstructorFunc create,
									DestructorFunc destroy,
									InitialiseFunc init = (InitialiseFunc) 0,
									CleanUpFunc cleanUp = (CleanUpFunc) 0
								);

	public:
	virtual void 				SetErrorCallback (ucallbackwithclosure<void (*)(const char*, void*)> callback) = 0;
	virtual void 				ResetErrorCallback (void) = 0;

	virtual void				SetByteCodePath (const std::string& path) = 0;
	virtual const std::string&	GetByteCodePath (void) = 0;

	// Compilation.
	virtual void							AddExternFuncs (UPTR(const char*) funcs) = 0;
	virtual std::list<UPTR(const char*)>*	GetExternFuncs (void) = 0;
	virtual void							ClearExternFuncs (void) = 0;

	virtual bool							Compile (const char* file) = 0;
	virtual bool							CompileText (const char* text) = 0;
	virtual bool							Translate (const TreeNode* ast) = 0;

	// Pure Parsing.
	virtual TreeNode*			Parse (const char* file) = 0;
	virtual TreeNode*			ParseText (const char* text) = 0;
	virtual TreeNode*			ParseQuotedElements (const char* text) = 0;
	virtual const std::string	Unparse (const TreeNode* ast) = 0;

	// Compile flags.
	virtual void				SetProductionMode (bool val) = 0;
	virtual bool				GetProductionMode (void) = 0;

	// Code emission.
	virtual void 				DumpUnparsed (const char* file) = 0;
	virtual void 				DumpAST (const char* file) = 0;
	virtual void 				DumpInterCode (const char* file) = 0;
	virtual void 				DumpTextCode (const char* file) = 0;
	virtual void				DumpBinaryCode (GenericWriter& writer) = 0;
	virtual void 				DumpBinaryCode (const char*	file) = 0;		// To secondary storage directly.
	
	virtual void 				Clear (void) = 0;

	static void					Initialise (void);
	static void					CleanUp (void);
	static CompilerIFace*		New (void);
	static void					Delete (CompilerIFace* instance);

	virtual ~CompilerIFace(){};
};

//-----------------------------------------------------------------------

template<class T> class DefaultCompilerIFace : public CompilerIFace {
	private:
	T* impl;

	static CompilerIFace*			New (void) { return DNEWCLASS(DefaultCompilerIFace, (DNEW(T))); }
	static void						Delete (CompilerIFace* instance) { DDELETE(instance); }
	static void						Initialise (void) { DeltaCompilerInit::Initialise(); }
	static void						CleanUp (void) { DeltaCompilerInit::CleanUp(); }
	
	DefaultCompilerIFace (T* impl) : impl(impl) {}
	~DefaultCompilerIFace () { DDELETE(impl); }
	public:
	void 							SetErrorCallback (ucallbackwithclosure<void (*)(const char*, void*)> callback)
		{ DPTR(impl)->SetErrorCallback(callback); }
	void 							ResetErrorCallback (void) { DPTR(impl)->ResetErrorCallback(); }
	void							SetByteCodePath (const std::string& path)
		{ DPTR(impl)->SetByteCodePath(path); }

	const std::string&				GetByteCodePath (void) { return DPTR(impl)->GetByteCodePath(); }
	void							AddExternFuncs (UPTR(const char*) funcs){ DPTR(impl)->AddExternFuncs(funcs); }
	std::list<UPTR(const char*)>*	GetExternFuncs (void)	{ return DPTR(impl)->GetExternFuncs(); }
	void							ClearExternFuncs (void) { DPTR(impl)->ClearExternFuncs(); }

	bool							Compile (const char* file) { return DPTR(impl)->Compile(file); }
	bool							CompileText (const char* text) { return DPTR(impl)->CompileText(text); }
	bool							Translate (const TreeNode* ast) { return DPTR(impl)->Translate(ast); }

	TreeNode*						Parse (const char* file) { return DPTR(impl)->Parse(file); }
	TreeNode*						ParseText (const char* text) { return DPTR(impl)->ParseText(text); }
	TreeNode*						ParseQuotedElements (const char* text) { return DPTR(impl)->ParseQuotedElements(text); }
	const std::string				Unparse (const TreeNode* ast) { return DPTR(impl)->Unparse(ast); }

	void							SetProductionMode (bool val) { DPTR(impl)->SetProductionMode(val); }
	bool							GetProductionMode (void) { return DPTR(impl)->GetProductionMode(); }

	void 							DumpUnparsed (const char* file) { DPTR(impl)->DumpUnparsed(file); }
	void 							DumpAST (const char* file) { DPTR(impl)->DumpAST(file); }
	void 							DumpInterCode (const char* file) { DPTR(impl)->DumpInterCode(file); }
	void 							DumpTextCode (const char* file) { DPTR(impl)->DumpTextCode(file); }
	void							DumpBinaryCode (GenericWriter& writer) { DPTR(impl)->DumpBinaryCode(writer); }
	void 							DumpBinaryCode (const char*	file) { DPTR(impl)->DumpBinaryCode(file); }

	void 							Clear (void) { DPTR(impl)->CleanUp(); }
	static void						Install (void) { CompilerIFace::Install(New, Delete, Initialise, CleanUp); }
};

#define INSTALL_DEFAULT_COMPILERIFACE(class)	DefaultCompilerIFace<class>::Install()

///////////////////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.