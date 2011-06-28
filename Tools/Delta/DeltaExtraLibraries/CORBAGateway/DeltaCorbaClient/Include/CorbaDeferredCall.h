#ifndef CORBA_DEFERRED_CALL
#define CORBA_DEFERRED_CALL

#include "utypes.h"
#include "uvalidatable.h"
#include "Corba.h"
#include "DeltaValue.h"
#include "ContainerAdapter.h"
#include "CorbaCommonLib.h"

#define CORBA_DEFERRED_CALL_TYPE_STR	CORBA_NAMESPACE_PREFIX "deferredcall"

////////////////////////////////////////////////////////////

inline const char* CorbaDeferredCallNameFun (void) 
	{ return CORBA_DEFERRED_CALL_TYPE_STR; }

class CorbaDeferredCall : public ContainerAdapter<std::list<DeltaValue>, CorbaDeferredCallNameFun> {
private:
	bool finished, isVoid;
	CORBA::Request_ptr request;
public:
	virtual void DeleteMemory (void) { DDELETE(this); }
	virtual void ToValue (DeltaValue* at) {
		at->FromExternId(
			(void*) GetSerialNo(),
			DeltaExternId_CollectableContainer,
			DELTA_EXTERNID_TOSTRINGNULL,
			CORBA_DEFERRED_CALL_TYPE_STR,
			(DeltaExternIdFieldGetter *) 0
		);
	}

	//**********************

	bool IsFinished(void) const { return finished; }
	void SetFinished(bool val = true) { finished = val; }

	CORBA::Request_ptr GetRequest(void) const { return request; }
	const std::list<DeltaValue>& GetOutAndInoutArgs(void) const { return *GetNativeContainerPtr(); }
	
	bool IsVoid(void) const { return isVoid; }

	//**********************

	CorbaDeferredCall (CORBA::Request_ptr request, const std::list<DeltaValue>& outAndInoutArgs, bool isVoid) :
		request(request), isVoid(isVoid), finished(false)
	{
		Original& cont = GetNativeContainer();
		cont.insert(cont.end(), outAndInoutArgs.begin(), outAndInoutArgs.end());
		for(Original::iterator i = cont.begin(); i != cont.end(); ++i)
			i->SetResidencyType(DeltaValue::Contained, this);
	}
	virtual ~CorbaDeferredCall() { CORBA::release(request); }
};

#define	CORBA_DEFERRED_CALL_VAL(val)	\
		VGET_INST(CorbaDeferredCall, val, CORBA_DEFERRED_CALL_TYPE_STR)

#define	DLIB_GET_CORBA_DEFERRED_CALL \
	DLIB_GET_VALIDATABLE_ARG(corbaDeferredCallVal, call, CorbaDeferredCall, CORBA_DEFERRED_CALL_TYPE_STR, CORBA_DEFERRED_CALL_VAL)

////////////////////////////////////////////////////////////

#endif