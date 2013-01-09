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

class NamedMethodEmitter : public ucomponentdirectoryclient {
	private:
	DeltaExpr* method;

	public:
	void Emit (DeltaExpr* table) const;
	NamedMethodEmitter (ucomponentdirectory* directory, DeltaExpr* _method) : 
		ucomponentdirectoryclient(directory), method(_method) {}
};

//////////////////////////////////////////////////////

class IndexedElemEmitter : public ucomponentdirectoryclient {
	private:
	DeltaExpr*	index;
	DeltaExpr*	content;

	public:
	void Emit(DeltaExpr* table) const;
	IndexedElemEmitter (ucomponentdirectory* directory, DeltaExpr* _index, DeltaExpr* _content) : 
		ucomponentdirectoryclient(directory), index(_index), content(_content) {}
};

//////////////////////////////////////////////////////

class UnindexedElemEmitter : public ucomponentdirectoryclient {
	private:
	util_ui32	order;
	DeltaExpr*	content;

	public:
	void Emit (DeltaExpr* table) const;
	UnindexedElemEmitter (ucomponentdirectory* directory, util_ui32	_order, DeltaExpr* _content) :
		ucomponentdirectoryclient(directory), order(_order), content(_content) {}
};

//////////////////////////////////////////////////////

struct TableElements :	public AutoCollectable,
						public Unparsable {

	DFRIENDDESTRUCTOR()
	friend class Translator;	//Instantiated only from Translator::CreateTableElements
	
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

#define	NIL_ELEMS (TableElements*) 0

//////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
