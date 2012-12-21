// ParseTableConstruction.h
// Translation rules for table construction.
// ScriptFighter Project.
// A. Savidis, May 2009(refactoring of table ctor handling).
//

#ifndef	PARSETABLECONSTRUCTION_H
#define	PARSETABLECONSTRUCTION_H

#include "DDebug.h"
#include "CompilerComponentDirectory.h"
#include "Expr.h"
#include "utypes.h"
#include "AutoCollection.h"
#include "Unparsable.h"
#include <functional>

//////////////////////////////////////////////////////

class NamedMethodEmitter  {
	USE_COMPILER_COMPONENT_DIRECTORY();
	private:
	DeltaExpr* method;

	public:
	void Emit (DeltaExpr* table) const;
	NamedMethodEmitter (DeltaExpr* _method) : 
		method(_method){}
};

//////////////////////////////////////////////////////

class IndexedElemEmitter  {
	USE_COMPILER_COMPONENT_DIRECTORY();
	private:
	DeltaExpr*	index;
	DeltaExpr*	content;

	public:
	void Emit(DeltaExpr* table) const;
	IndexedElemEmitter (DeltaExpr*	_index, DeltaExpr* _content) : 
		index(_index), 
		content(_content){}
};

//////////////////////////////////////////////////////

class UnindexedElemEmitter  {
	USE_COMPILER_COMPONENT_DIRECTORY();
	private:
	util_ui32	order;
	DeltaExpr*	content;

	public:
	void Emit (DeltaExpr* table) const;
	UnindexedElemEmitter (util_ui32	_order, DeltaExpr* _content) : 
		order(_order), 
		content(_content){}
};

//////////////////////////////////////////////////////

struct TableElements :	public AutoCollectable,
						public Unparsable {

	DFRIENDDESTRUCTOR()
	friend class Translator;
	
	ExprList*	unindexedValues;
	ExprList*	indexedValues;
	ExprList*	indices;
	
	private:
	TableElements (AutoCollector* collector) : AutoCollectable(collector) {
		unindexedValues		= DNEWCLASS(ExprList, (collector));
		indexedValues		= DNEWCLASS(ExprList, (collector));
		indices				= DNEWCLASS(ExprList, (collector));
	}

	~TableElements() {
		if (!UnderAutoCollection()) {
			DDELETE(unindexedValues);
			DDELETE(indexedValues);
			DDELETE(indices);
		}
	}
};

typedef AutoCollectableFactory<TableElements> TableElementsFactory;

#define	NIL_ELEMS (TableElements*) 0

//////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
