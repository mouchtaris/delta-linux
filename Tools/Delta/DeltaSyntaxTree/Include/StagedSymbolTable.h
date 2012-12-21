// StagedSymbolTable.h
// SymbolTable with staging and pure function information.
// ScriptFighter Project.
// Y. Lilis, May 2011.
//

#ifndef	STAGEDSYMBOLTABLE_H
#define	STAGEDSYMBOLTABLE_H

#include "utypes.h"
#include "ucallbacks.h"
#include "DDebug.h"
#include "TreeNode.h"

#include <map>
#include <vector>
#include <set>

/////////////////////////////////////////////////////////

enum SymbolType {
	SymbolType_Variable		= 0,
	SymbolType_Function		= 1,
	SymbolType_Const		= 2,
};

class SYNTAXTREELIB_CLASS Symbol {
public:
	enum DependencyType {
		Inline			= 0,
		GlobalVariable	= 1,
		Closure			= 2,
		NonLocal		= 3,
	};

	typedef ucallbackwithclosure<void (*)(Symbol*, util_ui32, void*)> ImpureCrossStageAccessCallback;
private:
	std::string name;
	std::string newName;
	SymbolType	type;
	util_ui32	stage;
	util_ui32	scope;
	util_ui32	line;
	bool		active;
	bool		nonLocal;

	Symbol*		myFunction;

	//**************************
	// Functions
	enum Pureness {
		Pure	= 0,
		Impure	= 1,
		Pending	= 2,
	};
	Pureness pureness;

	enum Globalization {
		NoGlobalization			= 0,
		GlobalizationPending	= 1,
		Globalized				= 2
	};
	Globalization globalization;

	bool completed;
	TreeNode* node;

	typedef std::set<Symbol*> SymbolSet;
	SymbolSet referTo, referrers;

	typedef std::pair<util_ui32, util_ui32> UsagePair;
	typedef std::list<UsagePair> UsageList;
	UsageList crossStageUsages;

	typedef std::pair<DependencyType, Symbol*> DependencyPair;
	typedef std::list<DependencyPair> DependencyList;
	DependencyList dependencies;

	ImpureCrossStageAccessCallback onImpureCrossStageAccess;

	bool GetSymbolsRecursively		(SymbolSet& functions, SymbolSet& symbols);
	void CheckCyclicDependencies	(void);
	void DetermineReferrerPureness	(void);

public:
	const std::string&	GetName		(void) const	{ return name;		}
	const std::string&	GetNewName	(void) const	{ return newName;	}
	SymbolType			GetType		(void) const	{ return type;		}
	util_ui32			GetStage	(void) const	{ return stage;		}
	util_ui32			GetScope	(void) const	{ return scope;		}
	util_ui32			GetLine		(void) const	{ return line;		}

	void				SetNewName	(const std::string& name) { newName = name; }

	//******************************************************************

	bool IsActive	(void) const	{ return active;	}
	void SetActive	(bool val)		{ active = val;		}

	//******************************************************************

	bool IsNonLocal	(void) const	{ return nonLocal;	}
	void SetNonLocal(bool val)		{ nonLocal = val;	}

	//******************************************************************

	bool IsVariable	(void) const { return type == SymbolType_Variable;	}
	bool IsFunction	(void) const { return type == SymbolType_Function;	}
	bool IsConst	(void) const { return type == SymbolType_Const;		}

	Symbol* MyFunction		(void) const { return myFunction; }
	void	SetMyFunction	(Symbol* f) { DASSERT(f && f->IsFunction()); myFunction = f; }

	//******************************************************************
	// Functions
	//
	bool		IsCompleted		(void) const	{ return completed;	}
	void		SetCompleted	(void)			{ completed = true;	}

	TreeNode*	GetNode			(void) const	{ return node; }

	Pureness	GetPureness		(void) const	{ return pureness; }
	bool		IsPure			(void) const	{ return pureness == Pure;		}
	bool		IsImpure		(void) const	{ return pureness == Impure;	}
	bool		PurenessPending	(void) const	{ return pureness == Pending;	}
	void		SetPure			(bool status);

	Globalization	GetGlobalization	(void) const	{ return globalization; }
	bool			IsGlobalized		(void) const	{ return globalization == Globalized;			}
	bool			ShouldGlobalize		(void) const	{ return globalization == GlobalizationPending;	}
	void			SetGlobalized		(void)	{ globalization = Globalized;			}
	void			SetShouldGlobalize	(void)	{ globalization = GlobalizationPending;	}

	void AddCrossStageUsage (util_ui32 stage, util_ui32 line) { crossStageUsages.push_back(UsagePair(stage, line)); }
	void SetImpureCrossStageAccessCallback	(const ImpureCrossStageAccessCallback& callback) { onImpureCrossStageAccess = callback; }

	void ReferTo			(Symbol* function);
	void DeterminePureness	(void);

	void AddDependency					(DependencyType type, Symbol* symbol);
	bool DependsOnlyOnFunctionClosures	(Symbol* function);

	//******************************************************************

	const char* GetTypeStr	(SymbolType type);
	void		Print		(void);

	//******************************************************************

	Symbol (const std::string name, SymbolType type, util_ui32 stage, util_ui32 scope, util_ui32 line, TreeNode* node = (TreeNode *) 0) :
		name			(name),
		newName			(),
		type			(type),
		stage			(stage),
		scope			(scope),
		line			(line),
		active			(true),
		nonLocal		(false),
		myFunction		((Symbol*) 0),
		pureness		(Pending),
		globalization	(NoGlobalization),
		completed		(false),
		node			(node)
	{}
};

/////////////////////////////////////////////////////////

class SYNTAXTREELIB_CLASS StagedSymbolTable {
private:
	std::vector<util_ui32> stageScopeStack;	
	typedef std::pair<util_ui32, Symbol*> StageSymbolPair;
	std::vector<StageSymbolPair> functions;

	typedef std::multimap<std::string, Symbol*> SymbolMap;
	typedef std::map<util_ui32, SymbolMap>		StageSymbolMap;
	StageSymbolMap stages;

	typedef std::pair<SymbolMap::iterator, SymbolMap::iterator> IteratorPair;

	typedef Symbol::ImpureCrossStageAccessCallback ImpureCrossStageAccessCallback;
	ImpureCrossStageAccessCallback onImpureCrossStageAccess;

	typedef ucallbackwithclosure<void (*)(Symbol*, void*)> SymbolHideCallback;
	SymbolHideCallback onHideSymbol;

	void Hide (util_ui32 stage, util_ui32 scope);
public:

	void		EnterStage	(void);
	void		LeaveStage	(void);
	util_ui32	GetStage	(void) const { DASSERT(stageScopeStack.size() >= 1); return (util_ui32) stageScopeStack.size() - 1; }

	void		EnterScope	(void);
	void		ExitScope	(bool shouldHide = true);
	util_ui32	GetScope	(void) const { DASSERT(!stageScopeStack.empty()); return stageScopeStack.back(); }

	//******************************************************************
	
	void	PushFunction				(Symbol* function);
	void	PopFunction					(void);
	Symbol*	CurrFunction				(void) const;
	Symbol*	PrevFunction				(void) const;
	Symbol* PreviousStageCurrFunction	(void) const;

	//******************************************************************
	
	const std::string	NewTempFuncName	(void);
	Symbol *			NewSymbol		(const std::string& name, SymbolType type, util_ui32 line, TreeNode* node = (TreeNode *) 0);
	void				Install			(Symbol* symbol);

	//******************************************************************

	Symbol* Lookup (const std::string& id);
	Symbol* Lookup (const std::string& id, util_ui32 stage);
	Symbol* Lookup (const std::string& id, util_ui32 stage, util_ui32 scope);

	//******************************************************************

	void SetImpureCrossStageAccessCallback	(const ImpureCrossStageAccessCallback& callback) { onImpureCrossStageAccess = callback; }
	void SetSymbolHideCallback (const SymbolHideCallback& callback) { onHideSymbol = callback; }

	//******************************************************************

	void Print (void);

	//******************************************************************

	StagedSymbolTable (void);
	~StagedSymbolTable ();
};

/////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
