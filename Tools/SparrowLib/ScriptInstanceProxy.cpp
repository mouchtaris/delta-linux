/**
 *	ScriptInstanceProxy.cpp
 *
 *	-- IDE Delta Script Component Loader --
 *
 *	ScriptInstanceProxy is used for representing an
 *	instance of a Delta component in the environment.
 *
 *	Themistoklis Bourdenas <tbourden@doc.ic.ac.uk>
 *	December 2007
 */
#include "ScriptInstanceProxy.h"
#include "Call.h"
#include "ComponentFunctionCallerSafe.h"
#include "ComponentRegistry.h"
#include "TypeConverter.h"
#include "DeltaScriptProxy.h"

#include "DDebug.h"
#include "DeltaVirtualMachine.h"
#include "DeltaStdLib.h"
#include "VMInit.h"
#include "uinit.h"
#include "uerrorclass.h"

#define DELTA_CALL(F, ERROR_RETVAL)															\
	do {																					\
		if (!DPTR(vm)->HasProducedError() && DPTR(vm)->GlobalFuncExists(STRINGIFY(F))) {		\
			DPTR(vm)->ExtCallGlobalFunc(STRINGIFY(F));										\
			if (UERROR_ISRAISED()) {														\
				Call<void (const String&), SafeCall>(DPTR(vm)->Id(), "Output", "Append")		\
					(util::std2str(uerror::GetSingleton().getreport() + "\n"));				\
				uerror::GetSingleton().clear();												\
				DELTA_VM_CALL_AND_RESET_ERRORS(vm, UEMPTY, DELTA_NO_VM_CALLER_FAILED);		\
				if (!GetInDestruction())													\
					this->Destroy();															\
				return ERROR_RETVAL;														\
			}																				\
		}																					\
	} while(false)

#define PUSH_ARG(TYPE, ARG)						\
	do {										\
		DeltaValue obj;						\
		to_delta<TYPE>().convert(&obj, ARG);	\
		DPTR(vm)->PushActualArg(obj);			\
	} while (false)

#define DELTA_CALL_0_STUB(F)				\
	void ScriptInstanceProxy::F(void) {		\
		DELTA_CALL(F, UEMPTY);				\
	}

#define DELTA_CALL_1_STUB(F, TYPE, ARG)			\
	void ScriptInstanceProxy::F(TYPE ARG) {		\
		PUSH_ARG(TYPE, ARG);					\
		DELTA_CALL(F, UEMPTY);					\
	}

#define DELTA_CALL_2_STUB(F, TYPE1, ARG1, TYPE2, ARG2)		\
	void ScriptInstanceProxy::F(TYPE1 ARG1, TYPE2 ARG2) {	\
		PUSH_ARG(TYPE2, ARG2);								\
		PUSH_ARG(TYPE1, ARG1);								\
		DELTA_CALL(F, UEMPTY);								\
	}

namespace ide
{
	//-------------------------------------------------------//
	//---- class ScriptInstanceProxy ------------------------//

	ScriptInstanceProxy::ScriptInstanceProxy(const std::string& classId, DeltaVirtualMachine* vm, Component* base)
		: classId(classId), vm(vm), base(base), baseSerial(0)
	{
		if (base) {
			baseSerial = base->GetSerial();
			base->SetDerivedClassId(classId);
		}
	}

	//-----------------------------------------------------------------------

	ScriptInstanceProxy::~ScriptInstanceProxy()
	{
		if (base && IsBaseValid())
			base->Destroy();
		if (vm)
			DDELETE(vm);
	}

	//-----------------------------------------------------------------------

	wxWindow* ScriptInstanceProxy::GenerateWindow(wxWindow* parent)
	{
		wxWindow *ret = 0;
		if (base)
		{
			ret = base->GenerateWindow(parent);
			DELTA_CALL(GenerateWindow, (wxWindow *) 0);
		}
		return ret;
	}

	//-----------------------------------------------------------------------

	wxWindow* ScriptInstanceProxy::GetWindow(void)
	{
		return base ? base->GetWindow() : 0;
	}

	//-----------------------------------------------------------------------

	void ScriptInstanceProxy::ComponentCreated(void)
	{
		DELTA_CALL(Constructor, UEMPTY);
		std::string error;
		if (!DeltaScriptProxy::InstanceImplementsRequiredFunctionsCheck(vm, classId, &error)) {
			DPTR(vm)->PrimaryError("Component creation failed: %s", error.c_str());
			Call<void (const String&), SafeCall>(DPTR(vm)->Id(), "Output", "Append")
				(util::std2str(uerror::GetSingleton().getreport() + "\n"));
			DELTA_VM_CALL_AND_RESET_ERRORS(vm, UEMPTY, DELTA_NO_VM_CALLER_FAILED);
			DASSERT(!GetInDestruction());
			this->Destroy();
		}	
	}

	//-----------------------------------------------------------------------

	DELTA_CALL_0_STUB(ComponentAdded);
	DELTA_CALL_0_STUB(ComponentRemoved);
	DELTA_CALL_0_STUB(ComponentDestroyed);

	DELTA_CALL_0_STUB(ComponentAddedAsFirstOfItsKind);
	DELTA_CALL_0_STUB(ComponentRemovedAsLastOfItsKind);
	DELTA_CALL_0_STUB(ComponentDestroyedAsLastOfItsKind);

	DELTA_CALL_0_STUB(ComponentFocused);
	
	//-----------------------------------------------------------------------

	void ScriptInstanceProxy::ComponentAppliedChangedProperties(const conf::PropertyTable& old, const conf::PropertyIdVec& changed)
	{
		StringList changedList;
		BOOST_FOREACH(const std::string& str, changed)
			changedList.push_back(util::std2str(str));
		PUSH_ARG(StringList, changedList);
		PUSH_ARG(const conf::Property*, &old);
		DELTA_CALL(ComponentAppliedChangedProperties, UEMPTY);
	}

	//-----------------------------------------------------------------------

	void ScriptInstanceProxy::ComponentRemovedFunction(const std::string& id)
		{ functions.erase(id); }

	void ScriptInstanceProxy::ComponentRemovedDynamicFunction (const std::string& id)
		{ dynamicFunctions.erase(id); }

	//-----------------------------------------------------------------------

	DELTA_CALL_1_STUB(ComponentAddedSignal, const std::string&, id);
	DELTA_CALL_1_STUB(ComponentRemovedSignal, const std::string&, id);

	//-----------------------------------------------------------------------

	DELTA_CALL_1_STUB(ChildAdded, Component*, component);
	DELTA_CALL_1_STUB(ChildRemoved, Component*, component);
	DELTA_CALL_1_STUB(ChildDestroyed, Component*, component);

	DELTA_CALL_1_STUB(ChildAddedAsFirstOfItsKind, Component*, component);
	DELTA_CALL_1_STUB(ChildRemovedAsLastOfItsKind, Component*, component);
	DELTA_CALL_1_STUB(ChildDestroyedAsLastOfItsKind, Component*, component);

	DELTA_CALL_1_STUB(ChildFocused, Component*, component);

	DELTA_CALL_2_STUB(ChildAddedSignal, Component*, component, const std::string&, id);
	DELTA_CALL_2_STUB(ChildRemovedSignal, Component*, component, const std::string&, id);

	//-----------------------------------------------------------------------

	bool ScriptInstanceProxy::HasFunction(const std::string& function) const
	{
		return functions.find(function) != functions.end();
	}

	//-----------------------------------------------------------------------

	bool ScriptInstanceProxy::IsBaseValid(void) const
	{
		return ComponentRegistry::Instance().IsValidInstance(base, baseSerial);
	}

	//-----------------------------------------------------------------------
}
