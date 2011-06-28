// DeltaExternIdFactory.h
// Factory and recycler for extern identifiers.
// ScriptFighter Project.
// A. Savidis, February 2006.
//

#ifndef	DELTAEXTERNIDFACTORY_H
#define	DELTAEXTERNIDFACTORY_H

#include "urecyclefactory.h"
#include "usingleton.h"
#include "DeltaExternId.h"

////////////////////////////////////////////////////////////

struct DVM_CLASS DeltaExternIdConstructorParms {

	typedef DeltaExternId::ToStringFunc ToStringFunc;

	void*							val;
	DeltaExternIdType				type;
	ToStringFunc					toString;
	const std::string				typeStr;
	const DeltaExternIdFieldGetter*	fieldGetter;

	DeltaExternIdConstructorParms (const DeltaExternIdConstructorParms& parms) :
		val(parms.val),
		type(parms.type),
		toString(parms.toString),
		typeStr(parms.typeStr),
		fieldGetter(parms.fieldGetter)
		{}

	DeltaExternIdConstructorParms (
			void*							_val,
			DeltaExternIdType				_type,
			ToStringFunc					_toString,
			const std::string&				_typeStr,
			const DeltaExternIdFieldGetter*	_fieldGetter
		) :
		val(_val),
		type(_type),
		toString(_toString),
		typeStr(_typeStr),
		fieldGetter(_fieldGetter){}

	~DeltaExternIdConstructorParms(){}
};

////////////////////////////////////////////////////////////

class DVM_CLASS DeltaExternIdPlacementConstructor : 
			public urecycleconstructor<
						DeltaExternId, 
						DeltaExternIdConstructorParms
					> {

	public:
	virtual DeltaExternId*	operator()(void* mem, const DeltaExternIdConstructorParms& parms) const {
		return	new (mem) DeltaExternId(					
					parms.val,
					parms.type,
					parms.toString,
					parms.typeStr,
					parms.fieldGetter
				);
	}
	virtual void Destroy (DeltaExternId* obj) const 
		{ obj->~DeltaExternId(); }
};

////////////////////////////////////////////////////////////

#define DELTA_EXTERNID_FACTORYCLASS					\
		usingleton<									\
			urecyclefactory<						\
				DeltaExternId,						\
				DeltaExternIdConstructorParms,		\
				DeltaExternIdPlacementConstructor	\
			>										\
		>
 
class DVM_CLASS DeltaExternIdFactory : public DELTA_EXTERNID_FACTORYCLASS {
	public:
	typedef DeltaExternId::ToStringFunc ToStringFunc;

	static	DeltaExternId* New (
				void*							val,
				DeltaExternIdType				type		= DeltaExternId_NonCollectable,
				ToStringFunc					toString	= (ToStringFunc) 0,
				const std::string&				typeStr		= DELTA_EXTERNID_TYPESTR_UNDEF,
				const DeltaExternIdFieldGetter*	fieldGetter = DELTA_EXTERNID_NO_FIELD_GETTER
			);

	static void Delete (DeltaExternId* id);
};
////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.