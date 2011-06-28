// ParseTableConstruction.h
// Translation rules for table construction.
// ScriptFighter Project.
// A. Savidis, May 2009(refactoring of table ctor handling).
//

#ifndef	PARSETABLECONSTRUCTION_H
#define	PARSETABLECONSTRUCTION_H

#include "DDebug.h"
#include "Expr.h"
#include "utypes.h"
#include "AutoCollection.h"
#include "ParseParms.h"
#include "Unparsable.h"
#include <functional>

//////////////////////////////////////////////////////

class NamedMethodEmitter  {
	private:
	DeltaExpr* method;

	public:
	void Emit (DeltaExpr* table) const;
	NamedMethodEmitter (DeltaExpr* _method) : 
		method(_method){}
};

//////////////////////////////////////////////////////

class IndexedElemEmitter  {
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

	ExprList*	unindexedValues;
	ExprList*	indexedValues;
	ExprList*	indices;

	TableElements (void) {
		unindexedValues		= DNEW(ExprList);
		indexedValues		= DNEW(ExprList);
		indices				= DNEW(ExprList);
	}

	~TableElements() {
		if (!UnderAutoCollection()) {
			DDELETE(unindexedValues);
			DDELETE(indexedValues);
			DDELETE(indices);
		}
	}
};

#define	NIL_ELEMS (TableElements*) 0

//////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
