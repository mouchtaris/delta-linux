// ParseTableConstruction.cpp.
// Translation rules for table construction.
// ScriptFighter Project.
// A. Savidis, October 1999 (original 1st version).
// Refactored August 2006.
// Changed July 2009, table elements are added exactly after
// their expression is evaluated.
//
#include "ParseTableConstruction.h"
#include "DDebug.h"
#include "ParseActions.h"
#include "DeltaByteCodeTypes.h"
#include "DeltaCompErrorMsg.h"
#include "Symbol.h"

//------------------------------------------------------------------
// EMITTERS

void NamedMethodEmitter::Emit (DeltaExpr* table) const {
	DASSERT(DPTR(method)->IsNamedMethod());
	QUADS.Emit(
		DeltaIC_OBJNEWSET,
		method,
		EXPRFACTORY.MakeConst(DPTR(method)->sym->methodName),
		NIL_EXPR
	);
}

/////////////////////////////////////////////////////////////////////

void IndexedElemEmitter::Emit (DeltaExpr* table) const {
	QUADS.Emit(
		DeltaIC_OBJNEWSET,
		content,
		index,
		NIL_EXPR
	);
}

/////////////////////////////////////////////////////////////////////

void UnindexedElemEmitter::Emit (DeltaExpr* table) const {
	QUADS.Emit(
		DeltaIC_OBJNEWSET,
		content,
		EXPRFACTORY.Make((DeltaNumberValueType) order),
		NIL_EXPR
	);
}

//------------------------------------------------------------------
// TABLE CONSTRUCTOR.

TableElements* Translator::CreateTableElements (void) const { return DNEWCLASS(TableElements, (&AUTOCOLLECTOR)); }

// In case more indices than values exist, the last value is copied
// to the remaining extra indices.

TableElements* Translator::Translate_IndexedValues (DeltaExpr* indices, DeltaExpr* values) {

	if (!indices || !values)
		return NIL_ELEMS;

	TableElements*	p = CreateTableElements();
	DPTR(DPTR(p)->indices)->Append(DPTR(indices));
	DPTR(p->indexedValues)->Append(DPTR(values));

	for (bool copyValue	= false; indices; indices = DPTR(indices)->next) {
		
		DASSERT(values);
		DeltaExpr* value = values;

		if (copyValue)
			p->indexedValues->Append(value = EXPRFACTORY.Copy(values));
		else
		if (values->next)
			{ DASSERT(!copyValue); values = DPTR(values->next); }
		else
			copyValue = true;				// When we run out of values, we copy the last value.

		if (DPTR(value)->IsNamedMethod()) {	// An explicit name is preserved even if the method is indexed.
			NamedMethodEmitter emitter(COMPONENT_CLIENT_CTOR_ARGS_1(value));
			emitter.Emit(PARSEPARMS.GetCurrConstructedTable());
		}
		IndexedElemEmitter emitter(COMPONENT_CLIENT_CTOR_ARGS_2(indices, value));
		emitter.Emit(PARSEPARMS.GetCurrConstructedTable());
	}

	return p;
}

///////////////////////////////////////////////////////////////////

TableElements* Translator::Translate_IdentIndexElement (
		const std::string&	index, 
		DeltaExpr*			expr, 
		DeltaQuadAddress	Mquad, 
		util_ui32			line
	) {
	if (!expr)
		return NIL_ELEMS;
	else {
		expr = DPTR(expr)->AdaptIfBool();
		TableElements* elems = Translate_IndexedValues(EXPRFACTORY.MakeConst(index), expr);
		Unparse_IdentIndexElement(elems, index, expr);
		QUADS.SetQuadLine(Mquad, line);
		return elems;
	}
}

///////////////////////////////////////////////////////////////////

TableElements* Translator::Translate_TableElement (DeltaExpr* expr) {
	if (!expr)
		return NIL_ELEMS;
	else {
		TableElements* elems = CreateTableElements();

		DPTR(elems)->unindexedValues->Append(
			expr = DPTR(expr)->AdaptIfBool()
		);

		if (expr->GetType() != DeltaExprNewAttribute) {
			if (DPTR(expr)->IsNamedMethod()) {
				NamedMethodEmitter emitter(COMPONENT_CLIENT_CTOR_ARGS_1(expr));
				emitter.Emit(PARSEPARMS.GetCurrConstructedTable());
			}
			else {
				UnindexedElemEmitter emitter(COMPONENT_CLIENT_CTOR_ARGS_2(PARSEPARMS.GetUnindexedElementOrder(), expr));
				emitter.Emit(PARSEPARMS.GetCurrConstructedTable());
				PARSEPARMS.IncUnindexedElementOrder();
			}
		}
		return elems;
	}
}

///////////////////////////////////////////////////////////////////

TableElements* Translator::Translate_TableElements (TableElements* t1, TableElements* t2) {

	if (!t1)
		return t2;
	else
	if (!t2)
		return t1;
	else {
		DPTR(DPTR(t1)->indices)->Append(DPTR(t2)->indices);
		DPTR(t1->indexedValues)->Append(t2->indexedValues);
		DPTR(t1->unindexedValues)->Append(t2->unindexedValues);

		DDELETE(t2);

		return t1;
	}
}

///////////////////////////////////////////////////////////////////
// Handles the initial creation of a table. The table creation statement
// should be generated prior to the code for creating the expressions that
// appears inside the table.
//
DeltaExpr* Translator::Translate_TablePrefix (void) {

	DeltaExpr* table	= EXPRFACTORY.New();
	DPTR(table)->type	= DeltaExprTableConstruction;
	table->sym			= DELTASYMBOLS.NewTemp();

	QUADS.Emit(DeltaIC_OBJNEW, NIL_EXPR, NIL_EXPR, table);

	PARSEPARMS.EnteringTableConstructor(table);
	table->SetTypeTag(TagObject);
	table->GetTypeInfo().Set(TagObject);
	return table;
}

///////////////////////////////////////////////////////////////////

DeltaExpr* Translator::Translate_TableConstructor (DeltaExpr* table, TableElements* elems) {

	NULL_EXPR_CHECK(table);
	udeleteunlessnull(elems);

	if (QUADS.GetCurrQuad().GetOpCode() == DeltaIC_OBJNEW)
		QUADS.GetCurrQuad().SetOpCode(DeltaIC_OBJNEWEMPTY);
	else {
		DASSERT(
			!elems ||
			QUADS.GetCurrQuad().GetOpCode() == DeltaIC_OBJNEWSET ||
			QUADS.GetCurrQuad().GetOpCode() == DeltaVM_OBJNEWATTR
		);
		QUADS.Emit(DeltaIC_OBJDONE, NIL_EXPR, NIL_EXPR, NIL_EXPR);
	}

	PARSEPARMS.ExitingTableConstructor();
	return table;
}

///////////////////////////////////////////////////////////////////

TableElements* Translator::Translate_UnindexedValue (
		DeltaExpr*			expr,
		DeltaQuadAddress	startQuad,
		util_ui32			lastLine
	) {

	TableElements* elems = Translate_TableElement(expr->AdaptAsArgumentVariable());
	DASSERT(elems || COMPMESSENGER.ErrorsExist());	// Null possible due to comp errors.
	if (elems) {	
		Unparse_UnindexedValue(elems, expr); 
		QUADS.SetQuadLine(startQuad, lastLine, true);
	}
	return elems;
}

//------------------------------------------------------------------
