// DebugExprParserAPI.h
// The API for the use of the debug expression evaluator for Delta.
// ScriptFighter Project.
// A. Savidis, February 2002.
//

#ifndef	DEBUGEXPRPARSERAPI_H
#define	DEBUGEXPRPARSERAPI_H

#include "DeltaVirtualMachine.h"
#include "DeltaTable.h"
#include "DeltaDebugCommands.h"
#include "ufunctors.h"
#include <string>
#include <functional>

///////////////////////////////////////////////////////////////

class DebugExprEvaluator {
	
	public:
	typedef DeltaDebugger::ObjectSlot				ObjectSlot;
	typedef DeltaDebugger::ObjectNode				ObjectNode;
	typedef DeltaDebugger::ObjectGraph				ObjectGraph;
	typedef std::list<DeltaValue>					ObjectList;
	typedef std::map<std::string, DeltaValue>		ObjectValues;

	///////////////////////////////////////////////////////////////

	private:

	// For extracting directed / undirected object graphs.

	struct DepthCounter {

		util_ui32	counter;
		bool		unlimited;

		const DepthCounter	Decrease (void) const {
								if (unlimited)
									return DepthCounter();
								else 
									{ DASSERT(counter); return DepthCounter(counter - 1); }
							}
		bool				More (void) const 
								{ return unlimited ? true : counter != 0; }

		DepthCounter (void)						: counter(0), unlimited(true)					{}
		DepthCounter (util_ui32 n)				: counter(n), unlimited(false)					{}
		DepthCounter (const DepthCounter& c)	: counter(c.counter), unlimited(c.unlimited)	{}
	};

	static bool expandLibraryFuncs;
	static bool expandMethodFuncs;
	static bool expandProgramFuncs;
	static bool expandFuncVMs;

	//*******************************

	static const std::pair<std::string, bool> 
								ExpandValueSlot		(ObjectGraph& graph, ObjectValues* values, const DeltaValue* val, const DepthCounter& depth);
	static const std::string	ExpandVMValue		(ObjectGraph& graph, ObjectValues* values, const DeltaVirtualMachine* vm, const DepthCounter& depth);
	static const std::string	ExpandSelfValue		(ObjectGraph& graph, ObjectValues* values, const DeltaTable* self, const DepthCounter& depth);
	static void					ExpandTable			(ObjectGraph& graph, ObjectValues* values, const DeltaValue* val, const DepthCounter& depth);
	static void					ExpandExternId		(ObjectGraph& graph, ObjectValues* values, const DeltaValue* val, const DepthCounter& depth);
	static void					ExpandLibraryFunc	(ObjectGraph& graph, ObjectValues* values, const DeltaValue* val, const DepthCounter& depth);
	static void					ExpandProgramFunc	(ObjectGraph& graph, ObjectValues* values, const DeltaValue* val, const DepthCounter& depth);
	static void					ExpandMethodFunc	(ObjectGraph& graph, ObjectValues* values, const DeltaValue* val, const DepthCounter& depth);
	static void					ExpandAttribute		(ObjectGraph& graph, ObjectNode& node, ObjectValues* values, const DeltaTable::Attribute* attr, const DepthCounter& depth);
	static void					ExpandClosure		(ObjectGraph& graph, ObjectNode& node, ObjectValues* values, const DeltaClosure* closure, const DeltaStdFuncInfo* func, const DepthCounter& depth);	
	static void					ExpandBackClosure	(ObjectGraph& graph, ObjectValues* values, CollectableContainer* cont);
	static void					ExternIdExcludeKeys	(std::list<std::string>* keys, const std::string& typeString);

	static bool					IsMethodOrProgramFuncVMNotYetExpanded (ObjectGraph& graph, const DeltaValue& m);
	static bool					IsMethodSelfNotYetExpanded (ObjectGraph& graph, const DeltaValue& m);
	static bool					IsMethodOrProgramFuncContainerNotYetExpanded (ObjectGraph& graph, const DeltaValue& m);
	static bool					IsUsefulToExpandMethodFunc (ObjectGraph& graph, const DeltaValue& m);
	static bool					IsUsefulToExpandProgramFunc (ObjectGraph& graph, const DeltaValue& f);

	static void					AddFunctionSlot (ObjectNode& node, const DeltaValue* val);
	static void					ExpandUndirectedNeighbours (ObjectGraph& graph, ObjectList& neighbours, const DepthCounter& depth);
	static void					IdentifyAndCreateNodesForUndirectedNeighbours (ObjectGraph& graph, ObjectList& neighbours, const DeltaValue* node);
	static void					AddNewNode (ObjectGraph& graph, const DeltaValue* val);

	//*******************************

	static void					CleanUpEvaluation (void);
	
	///////////////////////////////////////////////////////////////

	public:
	static void					SingletonCreate (void);
	static void					SingletonDestroy (void);

	static const std::string	ToString (const DeltaValue* obj);

	static const	std::pair<const std::string, bool>			
								Evaluate (
									DeltaVirtualMachine*	vm,
									const char*				expr
								);

	static const	std::pair<const std::string, bool>
								EvaluateToTypeData (
									DeltaVirtualMachine*	vm,
									const char*				formatId,
									const char*				expr
								);

	static void					SetObjectGraphConfiguration(
									bool expandLibraryFuncs,
									bool expandMethodFuncs,
									bool expandProgramFuncs,
									bool expandFuncVMs
								);

	static bool					EvaluateToObjectGraph (
									DeltaVirtualMachine*	vm,
									const char*				expr,
									ObjectGraph&			graph,
									util_ui32				depth,
									std::string&			simpleOrError
								);

	static bool					EvaluateToUndirectedObjectGraph (
									DeltaVirtualMachine*	vm,
									const char*				expr,
									ObjectGraph&			graph,
									util_ui32				depth,
									std::string&			simpleOrError
								);

	static DeltaValue*			Evaluate (
									DeltaVirtualMachine*	vm,
									const char*				expr,
									std::string&			putErrorString
								);

	static const std::string	Assign (
									DeltaVirtualMachine*	vm,
									const char*				lvalue,
									const char*				rvalue
								);

	static bool					HasError (void);
};

///////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
