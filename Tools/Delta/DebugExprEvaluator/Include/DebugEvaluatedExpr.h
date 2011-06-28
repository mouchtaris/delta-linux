// DebugEvaluatedExpr.h
// Parse actions for the debug expression evaluator.
// ScriptFighter Project.
// A. Savidis, February 2002.
//

#ifndef	DEBUGEVALUATEDEXPR_H
#define	DEBUGEVALUATEDEXPR_H

#include "DeltaValue.h"
#include "DDebug.h"
#include "ufunctors.h"
#include "ulatedestructor.h"
#include "DeltaStdDefs.h"

#include <list>
#include <functional>

/////////////////////////////////////////////////////////////

struct DebugEvaluatedExpr {

	DFRIENDDESTRUCTOR()

	private:
	bool								isLvalue, isField;
	DeltaValue*							result;
	std::pair<DeltaValue*, DeltaValue*>	fieldInfo;

	static std::list<DebugEvaluatedExpr*>*	allEvaluatedExprs;

	bool				Invariant (void) const 
							{ return result && (isField == (fieldInfo.first && fieldInfo.second)); }

	DebugEvaluatedExpr (DeltaValue* r, DeltaValue* t, DeltaValue* i) :
		isLvalue	(true),
		isField		(true),
		result		(r),
		fieldInfo	(t, i){ DASSERT(Invariant()); }
	
	DebugEvaluatedExpr (DeltaValue* r, bool lv) :
		isLvalue	(lv),
		isField		(false),
		result		(r),
		fieldInfo	((DeltaValue*) 0, (DeltaValue*) 0)
		{ DASSERT(Invariant()); }

	~DebugEvaluatedExpr(){}

	/////////////////////////////////////////////////////////////

	public:
	DeltaValue*					GetResult (void) 
									{ DASSERT(Invariant()); return result; }
	
	bool						IsLvalue (void) const
									{ DASSERT(Invariant()); return isLvalue; }
	
	bool						IsField (void) const
									{ DASSERT(Invariant()); return isField; }

	std::pair<DeltaValue*, DeltaValue*>&
								GetField (void)
									{ DASSERT(Invariant() && isField); return fieldInfo; }

	static void 				SingletonCreate(void);
	static void 				SingletonDestroy(void);
	static void					CleanUp (void);

	static DebugEvaluatedExpr*	New (DeltaValue* r, DeltaValue* t, DeltaValue* i);
	static DebugEvaluatedExpr*	New (DeltaValue* r, bool lv);
	static char*				StringLateDestroy (char* s);	// Implemented in API
};

typedef std::list<DebugEvaluatedExpr*> DebugExprList;

/////////////////////////////////////////////////////////////

typedef	udestroyablewrapper<
			DebugExprList*, 
			uptrdestructorfunctor<DebugExprList*> 
		> DestroyableExprList;

class ExprListDestructorClass : 
	public	ulatedestructionmanager<
				DestroyableExprList,
				uptrdestructorfunctor<DestroyableExprList*>
			>{};

class ExprListDestructor : public usingleton<ExprListDestructorClass>{};

DebugExprList* EXPRLIST_LATE_DESTROY (DebugExprList* s);

#endif	// Do not add stuff beyond this point.
