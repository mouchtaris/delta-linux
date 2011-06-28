// DeltaStdLib.h
// Standard library functions for the DeltaVirtualMachine.
// ScriptFighter Project.
// A. Savidis, December 1999.
//

#ifndef	DELTASTDLIB_H
#define	DELTASTDLIB_H

#include "DeltaStdDefs.h"
#include "DeltaTable.h"
#include "DeltaValue.h"
#include <string>
#include <list>
#include <functional>

//////////////////////////////////////////////////////////////////////

class DeltaStdUtilities : public DeltaVirtualMachine::StdUtilities {
	public:
	typedef DeltaVirtualMachine::StdUtilities::ValList	ValList;

	virtual  bool						IsList						(const DeltaValue* val)		const;
	virtual ValList&					GetList						(DeltaValue* val)			const;
	virtual const ValList&				GetList						(const DeltaValue* val)		const;
	virtual void						MakeList					(DeltaValue* at)			const;
	virtual CollectableContainer*		GetListContainer			(DeltaValue* value)			const;
	virtual const CollectableContainer*	GetListContainer			(const DeltaValue* value)	const;
	virtual bool						IsVector					(const DeltaValue* value)	const;
	virtual const ValVector&			GetVector					(const DeltaValue* value)	const;
	virtual ValVector&					GetVector					(DeltaValue* value)			const;
	virtual CollectableContainer*		MakeVector					(DeltaValue* at)			const;

	virtual DeltaExternId*				MakeExternId				(DeltaVirtualMachine* vm)	const;
	virtual void						LinkWithLibraryFunctions	(DeltaVirtualMachine* vm)	const;

};

//////////////////////////////////////////////////////////////////////

#define	DELTATUPLE_FIELD(i) \
			const std::string id##i, const DeltaValue&	val##i

#define	DELTATUPLE			DeltaMakeTupple
#define	DELTATUPLE_ID(id)	#id
#define	DELTATUPLE_VALUE	DeltaValue

typedef std::list< std::pair<std::string, DeltaValue> > DeltaTupleFields;

DVM_FUNC		DeltaTable* DeltaMakeTupple (const DeltaTupleFields& fields);
DVM_FUNC		DeltaTable* DeltaMakeTupple (DELTATUPLE_FIELD(1));
DVM_FUNC		DeltaTable* DeltaMakeTupple (DELTATUPLE_FIELD(1), DELTATUPLE_FIELD(2));
DVM_FUNC		DeltaTable* DeltaMakeTupple (DELTATUPLE_FIELD(1), DELTATUPLE_FIELD(2), DELTATUPLE_FIELD(3));
DVM_FUNC		DeltaTable* DeltaMakeTupple (DELTATUPLE_FIELD(1), DELTATUPLE_FIELD(2), DELTATUPLE_FIELD(3), DELTATUPLE_FIELD(4));

//////////////////////////////////////////////////////////////////////

extern void		Install_StdLib (void);	
extern void		SetByteCodeLoadingPath_StdLib (const std::string& path, bool prioritised);
extern void		SetDllImportPath_StdLib (const std::string& path, bool prioritised);
extern void		CleanUp_StdLib (void);

#endif	// Do not add stuff beyond this point.