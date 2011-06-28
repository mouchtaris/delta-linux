// DeltaExternIdFactory.cpp
// ScriptFigher Project.
// A. Savidis, February 2006.
//

#include "DeltaExternIdFactory.h"

USINGLETON_INSTANCE(DELTA_EXTERNID_FACTORYCLASS)

////////////////////////////////////////////////////////////

DeltaExternId* DeltaExternIdFactory::New (
		void*							val,
		DeltaExternIdType				type,
		ToStringFunc					toString,
		const std::string&				typeStr,
		const DeltaExternIdFieldGetter*	fieldGetter
	) {
	return	GetSingleton().Get(
				DeltaExternIdConstructorParms(
					val,
					type,
					toString,
					typeStr,
					fieldGetter
				)
			);
}

////////////////////////////////////////////////////////////

void DeltaExternIdFactory::Delete (DeltaExternId* id) 
	{ GetSingleton().UnUse(id); }

////////////////////////////////////////////////////////////
