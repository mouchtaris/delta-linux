/**
 *	DeltaVMBreakpoints.cpp
 *
 *	-- IDE Delta Virtual Machine Breakpoint Handling --
 *
 *	Virtual Machine component of the Delta language.
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	Functionality moved here from DeltaVM.cpp. 
 */
#include "DeltaVM.h"

#include "Call.h"
#include "ComponentFunctionCallerSafe.h"
#include "ComponentRegistry.h"

#include "DelayedCaller.h"
#include "StringUtils.h"

#include "DeltaVirtualMachine.h"
#include "DeltaDebugClientBreakPoints.h"
#include "uerrorclass.h"

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#ifndef NO_VLD
#include <vld.h>
#endif

#define CALL_DELAYED(call)	timer::DelayedCaller::Instance().PostDelayedCall(call)

namespace ide
{
	//-------------------------------------------------------//
	//---- static members -----------------------------------//
	
	DeltaVM::BreakpointList DeltaVM::stageGeneratedBreakpoints;
	DeltaVM::BreakpointList DeltaVM::finalGeneratedBreakpoints;

	volatile bool	DeltaVM::waitingBreakpointValidation	= false;

	//-------------------------------------------------------//
	//---- class DeltaVM ------------------------------------//

	EXPORTED_STATIC_SIGNAL(DeltaVM, BreakpointAdded, (const String& uri, const String& symbolic, int line, const String& condition));
	EXPORTED_STATIC_SIGNAL(DeltaVM, BreakpointRemoved, (const String& uri, const String& symbolic, int line, bool enabled));
	EXPORTED_STATIC_SIGNAL(DeltaVM, BreakpointEnabled, (const String& uri, const String& symbolic, int line));
	EXPORTED_STATIC_SIGNAL(DeltaVM, BreakpointDisabled, (const String& uri, const String& symbolic, int line));
	EXPORTED_STATIC_SIGNAL(DeltaVM, BreakpointConditionChanged, (const String& uri, const String& symbolic, int line, const String& condition));
	EXPORTED_STATIC_SIGNAL(DeltaVM, BreakpointLineChanged, (const String& uri, const String& symbolic, int line, int newLine));
	EXPORTED_STATIC_SIGNAL(DeltaVM, BreakpointHit, (const String& uri, const String& symbolic, int line));
	EXPORTED_STATIC_SIGNAL(DeltaVM, FirstBreakpoint, (void));
	EXPORTED_STATIC_SIGNAL(DeltaVM, FirstBreakpointEnabled, (void));
	EXPORTED_STATIC_SIGNAL(DeltaVM, FirstBreakpointDisabled, (void));
	EXPORTED_STATIC_SIGNAL(DeltaVM, NoBreakpoints, (void));
	EXPORTED_STATIC_SIGNAL(DeltaVM, NoBreakpointsEnabled, (void));
	EXPORTED_STATIC_SIGNAL(DeltaVM, NoBreakpointsDisabled, (void));

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, bool, ToggleBreakpoint, (const String& symbolic, int line))
	{
		const std::string csymbolic = util::str2std(symbolic);

		if (!DeltaDebugClientBreakPoints::Get().HasBreakPoint(csymbolic, line))
			AddBreakpoint(symbolic, line, String());
		else if (!DeltaDebugClientBreakPoints::Get().Get(csymbolic, line).IsEnabled())
			EnableBreakpoint(symbolic, line);
		else {
			RemoveBreakpoint(symbolic, line);
			return false;
		}
		return true;
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, bool, ToggleBreakpointStatus, (const String& symbolic, int line))
	{
		if (waitingBreakpointValidation) // Operation suspended temporarily.
			return false;	

		const std::string csymbolic = util::str2std(symbolic);

		if (DeltaDebugClientBreakPoints::Get().HasBreakPoint(csymbolic, line)) {
			if(DeltaDebugClientBreakPoints::Get().Get(csymbolic, line).IsEnabled())
				DisableBreakpoint(symbolic, line);
			else
				EnableBreakpoint(symbolic, line);
			return true;
		}
		else
			return false;
	}

	//-----------------------------------------------------------------------

	void DeltaVM::CommitAddBreakpoint (const String& symbolic, int line, const String& condition) {
	
		bool hadBreakpoints			= !!DeltaDebugClientBreakPoints::Get().Total();
		bool hadEnabledBreakpoints	= DeltaDebugClientBreakPoints::Get().HasEnabledBreakPoints();

		DeltaDebugClientBreakPoints::Get().Add(util::str2std(symbolic), line, util::str2std(condition));

		CALL_DELAYED(boost::bind(&DeltaVM::onBreakpointAdded, GetURIFromSymbolic(symbolic), symbolic, line, condition));

		if (!hadEnabledBreakpoints)	//was this the first breakpoint to be enabled?
			CALL_DELAYED(onFirstBreakpointEnabled);

		if (!hadBreakpoints)	//was this the first breakpoint at all?
			CALL_DELAYED(onFirstBreakpoint);
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, void, AddBreakpoint, (const String& symbolic, int line, const String& condition))
	{
		if (waitingBreakpointValidation) // Operation suspended temporarily.
			return;	

		const std::string csymbolic = util::str2std(symbolic);
		const std::string ccondition = util::str2std(condition);

		if (!DeltaDebugClientBreakPoints::Get().HasBreakPoint(csymbolic, line)) {
			
			if (debugRunningState == DEBUG_RUNNING) {
				waitingBreakpointValidation = true;	// Added only by validation.
				DeltaDebugClient::DoAddBreakPoint(csymbolic, line, ccondition);
			}
			else	// Added instantly.
				CommitAddBreakpoint(symbolic, line, condition);
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, void, RemoveBreakpoint, (const String& symbolic, int line))
	{
		const std::string csymbolic = util::str2std(symbolic);
		
		//Don't assert because commands can be invoked from the introspection with invalid arguments
		if (DeltaDebugClientBreakPoints::Get().HasBreakPoint(csymbolic, line)) {
			bool enabled = DeltaDebugClientBreakPoints::Get().Get(csymbolic, line).IsEnabled();

			DeltaDebugClientBreakPoints::Get().Remove(csymbolic, line);
			if (debugRunningState == DEBUG_RUNNING)
				DeltaDebugClient::DoRemoveBreakPoint(csymbolic, line);
			CALL_DELAYED(boost::bind(&DeltaVM::onBreakpointRemoved, GetURIFromSymbolic(symbolic), symbolic, line, enabled));

			if (enabled && !DeltaDebugClientBreakPoints::Get().HasEnabledBreakPoints())	//was this the last enabled breakpoint
				CALL_DELAYED(onNoBreakpointsEnabled);
			else if (!enabled && !DeltaDebugClientBreakPoints::Get().HasDisabledBreakPoints())	//was this the last disabled breakpoint
				CALL_DELAYED(onNoBreakpointsDisabled);
			if (!DeltaDebugClientBreakPoints::Get().Total())
				CALL_DELAYED(onNoBreakpoints);
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, void, EnableBreakpoint, (const String& symbolic, int line))
	{
		const std::string csymbolic = util::str2std(symbolic);

		//Don't assert because commands can be invoked from the introspection with invalid arguments
		if (DeltaDebugClientBreakPoints::Get().HasBreakPoint(csymbolic, line) &&
			!DeltaDebugClientBreakPoints::Get().Get(csymbolic, line).IsEnabled()
		) {
			bool hadEnabledBreakpoints = DeltaDebugClientBreakPoints::Get().HasEnabledBreakPoints();

			DeltaDebugClientBreakPoints::Get().Enable(csymbolic, line);
			if (debugRunningState == DEBUG_RUNNING)
				DeltaDebugClient::DoEnableBreakPoint(csymbolic, line);
			CALL_DELAYED(boost::bind(&DeltaVM::onBreakpointEnabled, GetURIFromSymbolic(symbolic), symbolic, line));

			if (!hadEnabledBreakpoints)	//was this the first breakpoint to be enabled?
				CALL_DELAYED(onFirstBreakpointEnabled);
			if (!DeltaDebugClientBreakPoints::Get().HasDisabledBreakPoints())	//was this the last disabled breakpoint?
				CALL_DELAYED(onFirstBreakpointDisabled);
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, void, DisableBreakpoint, (const String& symbolic, int line))
	{
		const std::string csymbolic = util::str2std(symbolic);

		//Don't assert because commands can be invoked from the introspection with invalid arguments
		if (DeltaDebugClientBreakPoints::Get().HasBreakPoint(csymbolic, line) &&
			DeltaDebugClientBreakPoints::Get().Get(csymbolic, line).IsEnabled()
		) {
			bool hadDisabledBreakpoints = DeltaDebugClientBreakPoints::Get().HasDisabledBreakPoints();

			DeltaDebugClientBreakPoints::Get().Disable(csymbolic, line);
			if (debugRunningState == DEBUG_RUNNING)
				DeltaDebugClient::DoDisableBreakPoint(csymbolic, line);
			CALL_DELAYED(boost::bind(&DeltaVM::onBreakpointDisabled, GetURIFromSymbolic(symbolic), symbolic, line));

			if (!hadDisabledBreakpoints)	//was this the first breakpoint to be disabled?
				CALL_DELAYED(onFirstBreakpointDisabled);
			if (!DeltaDebugClientBreakPoints::Get().HasEnabledBreakPoints())	//was this the last enabled breakpoint?
				CALL_DELAYED(onFirstBreakpointEnabled);
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, void, ChangeBreakpointCondition, (const String& symbolic, int line, const String& condition))
	{
		const std::string csymbolic = util::str2std(symbolic);
		const std::string ccondition = util::str2std(condition);

		//Don't assert because commands can be invoked from the introspection with invalid arguments
		if (DeltaDebugClientBreakPoints::Get().HasBreakPoint(csymbolic, line)) {
			DeltaDebugClientBreakPoints::Get().Change(csymbolic, line, ccondition.c_str());
			if (debugRunningState == DEBUG_RUNNING)
				DeltaDebugClient::DoChangeBreakPointCondition(csymbolic, line, ccondition);
			CALL_DELAYED(boost::bind(&DeltaVM::onBreakpointConditionChanged, GetURIFromSymbolic(symbolic), symbolic, line, condition));
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, void, ChangeBreakpointLine, (const String& symbolic, int line, int newLine))
	{
		const std::string csymbolic = util::str2std(symbolic);

		//Don't assert because commands can be invoked from the introspection with invalid arguments
		if (DeltaDebugClientBreakPoints::Get().HasBreakPoint(csymbolic, line)) {
			const DeltaBreakPoint bpt = DeltaDebugClientBreakPoints::Get().Get(csymbolic, line);
			DeltaDebugClientBreakPoints::Get().Change(csymbolic, line, newLine);
			if (debugRunningState == DEBUG_RUNNING) {	//when running remove and insert to get up-to-date info
				const String condition = bpt.HasCondition() ? util::std2str(bpt.GetCondition()) : String();
				RemoveBreakpoint(symbolic, line);
				AddBreakpoint(symbolic, line, condition);
			}
			else	//else just change the line of the breakpoint assuming it is valid
				CALL_DELAYED(boost::bind(&DeltaVM::onBreakpointLineChanged, GetURIFromSymbolic(symbolic), symbolic, line, newLine));
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, StringListList, GetBreakpoints, (const String& symbolic))
	{
		const std::string csymbolic = util::str2std(symbolic);
		StringListList ret;
		if (DeltaDebugClientBreakPoints::Get().HasAnyBreakPoints(csymbolic)) {
			BOOST_FOREACH(const DeltaBreakPoint& bpt, DeltaDebugClientBreakPoints::Get().Get(csymbolic)) {
				StringList breakpoint;
				breakpoint.push_back(GetURIFromSymbolic(symbolic));
				breakpoint.push_back(symbolic);
				breakpoint.push_back(boost::lexical_cast<String>(bpt.GetLine()));
				breakpoint.push_back(util::std2str(bpt.GetCondition()));
				breakpoint.push_back(bpt.IsEnabled() ? _T("true") : _T("false"));
				ret.push_back(breakpoint);
			}
		}
		return ret;
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, StringListList, GetAllBreakpoints, (void))
	{
		StringListList ret;
		util_ui16 total = DeltaDebugClientBreakPoints::Get().Total();
		for (util_i16 i = 0; i < total; ++i) {
			const std::string source = DeltaDebugClientBreakPoints::Get().GetSource(i);
			const StringListList l = GetBreakpoints(util::std2str(source));
			ret.insert(ret.end(), l.begin(), l.end());
		}
		return ret;
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, void, DeleteAllSourceBreakpoints, (const String& symbolic))
	{
		const std::string source = util::str2std(symbolic);
		const std::list<DeltaBreakPoint> l = DeltaDebugClientBreakPoints::Get().Get(source);
		DeltaDebugClientBreakPoints::Get().RemoveAll(source);
		if (debugRunningState == DEBUG_RUNNING)
			DeltaDebugClient::DoRemoveAllBreakPoints(source);
		BOOST_FOREACH(const DeltaBreakPoint& bpt, l) 
			sigBreakpointRemoved(GetURIFromSymbolic(symbolic), symbolic, bpt.GetLine(), bpt.IsEnabled());
		RemoveSourceFromBreakpointList(finalGeneratedBreakpoints, symbolic);
		RemoveSourceFromBreakpointList(stageGeneratedBreakpoints, symbolic);
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, void, DeleteAllBreakpoints, (void))
	{
		bool hadEnabledBreakpoints = DeltaDebugClientBreakPoints::Get().HasEnabledBreakPoints();
		bool hadDisabledBreakpoints = DeltaDebugClientBreakPoints::Get().HasDisabledBreakPoints();
		while (DeltaDebugClientBreakPoints::Get().Total())
			DeleteAllSourceBreakpoints(util::std2str(DeltaDebugClientBreakPoints::Get().GetSource(0)));
		finalGeneratedBreakpoints.clear();
		stageGeneratedBreakpoints.clear();
		if (hadEnabledBreakpoints)
			sigNoBreakpointsEnabled();
		if (hadDisabledBreakpoints)
			sigNoBreakpointsDisabled();
		sigNoBreakpoints();
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, void, EnableAllBreakpoints, (void))
	{
		bool hadEnabledBreakpoints = DeltaDebugClientBreakPoints::Get().HasEnabledBreakPoints();
		bool hadDisabledBreakpoints = DeltaDebugClientBreakPoints::Get().HasDisabledBreakPoints();
		util_ui16 total = DeltaDebugClientBreakPoints::Get().Total();
		for (util_i16 i = 0; i < total; ++i) {
			const std::string source = DeltaDebugClientBreakPoints::Get().GetSource(i);
			const std::list<DeltaBreakPoint> l = DeltaDebugClientBreakPoints::Get().Get(source);
			DeltaDebugClientBreakPoints::Get().EnableAll(source);
			if (debugRunningState == DEBUG_RUNNING)
				DeltaDebugClient::DoEnableAllBreakPoints(source);
			BOOST_FOREACH(const DeltaBreakPoint& bpt, l)
				if(!bpt.IsEnabled()) {
					const String symbolic = util::std2str(source);
					sigBreakpointEnabled(GetURIFromSymbolic(symbolic), symbolic, bpt.GetLine());
				}
		}
		if (hadDisabledBreakpoints)
			sigNoBreakpointsDisabled();
		if (!hadEnabledBreakpoints && DeltaDebugClientBreakPoints::Get().HasEnabledBreakPoints())
			sigFirstBreakpointEnabled();
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, void, DisableAllBreakpoints, (void))
	{
		bool hadEnabledBreakpoints = DeltaDebugClientBreakPoints::Get().HasEnabledBreakPoints();
		bool hadDisabledBreakpoints = DeltaDebugClientBreakPoints::Get().HasDisabledBreakPoints();
		util_ui16 total = DeltaDebugClientBreakPoints::Get().Total();
		for (util_i16 i = 0; i < total; ++i) {
			const std::string source = DeltaDebugClientBreakPoints::Get().GetSource(i);
			const std::list<DeltaBreakPoint> l = DeltaDebugClientBreakPoints::Get().Get(source);
			DeltaDebugClientBreakPoints::Get().DisableAll(source);
			if (debugRunningState == DEBUG_RUNNING)
				DeltaDebugClient::DoDisableAllBreakPoints(source);
			BOOST_FOREACH(const DeltaBreakPoint& bpt, l)
				if (bpt.IsEnabled()) {
					const String symbolic = util::std2str(source);
					sigBreakpointDisabled(GetURIFromSymbolic(symbolic), symbolic, bpt.GetLine());
				}
		}
		if (hadEnabledBreakpoints)
			sigNoBreakpointsEnabled();
		if (!hadDisabledBreakpoints && DeltaDebugClientBreakPoints::Get().HasDisabledBreakPoints())
			sigFirstBreakpointDisabled();
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, bool, LoadBreakpoints, (const String& uri))
	{
		const std::string& curi = util::normalizepath(util::str2std(uri));
		bool hadBreakpoints = DeltaDebugClientBreakPoints::Get().Total() > 0;
		bool hadEnabledBreakpoints = DeltaDebugClientBreakPoints::Get().HasEnabledBreakPoints();
		bool hadDisabledBreakpoints = DeltaDebugClientBreakPoints::Get().HasDisabledBreakPoints();
		bool status = DeltaDebugClientBreakPoints::Get().LoadText(curi.c_str());
		if(!status) {
			DASSERT(UERROR_ISRAISED());
			DeltaVirtualMachine::ResetRunTimeErrors();
			UERROR_CLEAR();
		}
		if (!hadEnabledBreakpoints && DeltaDebugClientBreakPoints::Get().HasEnabledBreakPoints())
			sigFirstBreakpointEnabled();
		if (!hadDisabledBreakpoints && DeltaDebugClientBreakPoints::Get().HasDisabledBreakPoints())
			sigFirstBreakpointDisabled();
		if (!hadBreakpoints && DeltaDebugClientBreakPoints::Get().Total() > 0)
			sigFirstBreakpoint();
		return status;
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, bool, SaveBreakpoints, (const String& uri))
	{
		const std::string& curi = util::normalizepath(util::str2std(uri));
		return DeltaDebugClientBreakPoints::Get().StoreText(curi.c_str());
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_STATIC(DeltaVM, void, OnEditorMarginMarksClicked,
		(const Handle& editor, int line), "EditorMarginMarksClicked")
	{
		const String symbolic = GetSymbolicURIFromEditor(editor);
		if (!symbolic.empty())
			ToggleBreakpoint(symbolic, line);
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_STATIC(DeltaVM, void, OnFileNameChanged,
		(const Handle& editor, const String& uri), "FileNameChanged")
	{
		ComponentEntry& entry = ComponentRegistry::Instance().GetComponentEntry("ProjectManager");
		if (entry && entry.GetFocusedInstance() && editor.GetClassId() == "Editor") {	//No breakpoint Handling if no ProjectManager
			const String normalizedUri = util::normalizepath(uri);
			const String symbolic = GetSymbolicURIFromURI(normalizedUri);
			const std::string csymbolic = util::str2std(symbolic);
			if (DeltaDebugClientBreakPoints::Get().HasAnyBreakPoints(csymbolic))
				BOOST_FOREACH(const DeltaBreakPoint& bpt, DeltaDebugClientBreakPoints::Get().Get(csymbolic))
					Call<void (int, bool), SafeCall>(s_classId, editor, "InsertBreakpoint")(
						bpt.GetLine(),
						bpt.IsEnabled()
					);
			if (debugRunningState == DEBUG_RUNNING && stopPoint.first == symbolic)
				Call<void (int), SafeCall>(s_classId, editor, "SetBreakpointArrow")(stopPoint.second);
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_STATIC(DeltaVM, void, OnTreeItemSymbolicURIChanged,
		(const Handle& handle, const String& oldUri, const String& newUri), "TreeItemSymbolicURIChanged")
	{
		if (handle.GetClassId() == "Script" || handle.GetClassId() == "Aspect") {
			const std::string symbolic = util::str2std(oldUri);
			if (DeltaDebugClientBreakPoints::Get().HasAnyBreakPoints(symbolic)) {
				const std::list<DeltaBreakPoint> l = DeltaDebugClientBreakPoints::Get().Get(symbolic);
				BOOST_FOREACH(const DeltaBreakPoint& bpt, l) {
					RemoveBreakpoint(oldUri, bpt.GetLine());
					AddBreakpoint(newUri, bpt.GetLine(), util::std2str(bpt.GetCondition()));
					if (!bpt.IsEnabled())
						DisableBreakpoint(newUri, bpt.GetLine());
				}
			}
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_STATIC(DeltaVM, void, OnEditLinesChangedBy,
		(const Handle& editor, int fromLine, int offset), "EditLinesChangedBy")
	{
		if (debugRunningState != DEBUG_RUNNING) {	//don't bother if the file is edited during debugging
			const String& uri = Call<const String& (void), SafeCall>(s_classId, "Editor", "GetURI")();
			const String symbolic = GetSymbolicURIFromURI(uri);
			const std::string csymbolic = util::str2std(symbolic);
			if (DeltaDebugClientBreakPoints::Get().HasAnyBreakPoints(csymbolic)) {
				const std::list<DeltaBreakPoint> l = DeltaDebugClientBreakPoints::Get().Get(csymbolic);
				BOOST_FOREACH(const DeltaBreakPoint& bpt, l)
					if(bpt.GetLine() > fromLine)
						ChangeBreakpointLine(symbolic, bpt.GetLine(), bpt.GetLine() + offset);
			}
		}
	}

	//-----------------------------------------------------------------------

	typedef std::map< util_ui32, std::set<util_ui32> > LineMappings;
	static const LineMappings StringListToLineMappings(const StringList& encodedMappings) {
		LineMappings lineMappings;
		BOOST_FOREACH(const String& str, encodedMappings) {
			StringVec split;
			util::stringtokenizer(split, str, _T(":"));
			DASSERT(split.size() == 2);
			std::set<util_ui32>& s = lineMappings[atoi(util::str2std(split[0]).c_str())];

			const String mapped = split[1];
			split.clear();
			util::stringtokenizer(split, mapped, _T(";"));
			BOOST_FOREACH(const String& line, split)
				s.insert(atoi(util::str2std(line).c_str()));
		}
		return lineMappings;
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_STATIC(DeltaVM, void, OnScriptSourceAttached, (const Handle& script, const Handle& attachedSource,
		const StringList& encodedLineMappings, uint index), "ScriptSourceAttached")
	{
		if (debugRunningState == DEBUG_RUNNING && !encodedLineMappings.empty()) {
			const std::string classId = attachedSource->GetClassId();
			bool stageSource = classId == "StageSource";
			bool stageResult = classId == "StageResult";
			bool stageTransformation = classId == "AspectResult" && script.GetClassId() == "StageSource";
			bool isScript = script.GetClassId() == "Script" || script.GetClassId() == "Aspect";
			bool originalTransformation = isScript && classId == _T("AspectResult");

			if (stageSource || stageResult || stageTransformation || originalTransformation) {
				Component* originalTarget;
				Component* generatedTarget;
				if (index == 1) {
					originalTarget = script.Resolve();
					generatedTarget = attachedSource.Resolve();
				}
				else {
					DASSERT(index > 1);
					Component::List children = script->GetChildren();
					Component::List::iterator pos = children.begin();
					std::advance(pos, (stageSource || stageResult ? 2 : 1) * index - (stageSource || stageResult ? 3 : 2));
					originalTarget = *pos;
					if (stageResult)
						++pos;
					generatedTarget = *++pos;
				}
					
				const std::string original = util::str2std(Call<const String (void)>(s_classId, originalTarget, "GetSymbolicURI")());			
				const String generated = Call<const String (void)>(s_classId, generatedTarget, "GetSymbolicURI")();
				bool keepOriginal = originalTransformation && index == 1	||	//first aspect transformation
									stageResult && index == 1 && isScript	||	//first result of non aspect original program
									stageSource								;
				GenerateBreakpoints(original, generated, encodedLineMappings, AddBreakpointSynchronously, stageGeneratedBreakpoints, !keepOriginal);
			}
		}
	}

	//-----------------------------------------------------------------------

	void DeltaVM::AddBreakpointSynchronously (const String& symbolic, int line, const String& condition) {
		DASSERT(!waitingBreakpointValidation && debugRunningState == DEBUG_RUNNING);

		const std::string csymbolic = util::str2std(symbolic);
		const std::string ccondition = util::str2std(condition);

		if (!DeltaDebugClientBreakPoints::Get().HasBreakPoint(csymbolic, line)) {
			waitingBreakpointValidation = true;	// Added only by validation.
			DeltaDebugClient::DoAddBreakPoint(csymbolic, line, ccondition);
			while(waitingBreakpointValidation)
				wxMilliSleep(100);
		}
	}

	//-----------------------------------------------------------------------

	void DeltaVM::GenerateBreakpoints(const std::string original, const String& generated, const StringList& encodedLineMappings,
		void (*adderFunc)(const String&, int, const String&), BreakpointList& breakpoints, bool deleteOriginal)
	{
		if (DeltaDebugClientBreakPoints::Get().HasAnyBreakPoints(original)) {
			const LineMappings lineMappings = StringListToLineMappings(encodedLineMappings);
			BOOST_FOREACH(const DeltaBreakPoint& bpt, DeltaDebugClientBreakPoints::Get().Get(original)) {
				LineMappings::const_iterator j = lineMappings.find(bpt.GetLine());
				if (j != lineMappings.end())
					for (std::set<util_ui32>::const_iterator iter = j->second.begin(); iter != j->second.end(); ++iter) {
						const String condition = util::std2str(bpt.GetCondition());
						DASSERT(adderFunc);
						(*adderFunc)(generated, *iter, condition);
						breakpoints.push_back(Breakpoint(generated, *iter, condition));
					}
			}
			if (deleteOriginal)
				DeleteAllSourceBreakpoints(util::std2str(original));
		}
	}
	//-----------------------------------------------------------------------

	void DeltaVM::GeneratedFinalSourceBreakpoints (void) {
		const util_ui16 total = DeltaDebugClientBreakPoints::Get().Total();
		for (util_i16 i = 0; i < total; ++i) {
			const std::string coriginal = DeltaDebugClientBreakPoints::Get().GetSource(i);
			const String original = util::std2str(coriginal);
			if (const Handle& script = Call<const Handle& (const String&)>(s_classId, "ProjectManager", "GetResourceBySymbolicURI")(original)) {
				const StringList encodedMappings = Call<const StringList (void)>(s_classId, script, "GetFinalLineMappings")();
				if (!encodedMappings.empty()) {
					const String finalSourceURI = Call<const String& (void)>(s_classId, script, "GetFinalSourceURI")();
					GenerateBreakpoints(coriginal, GetSymbolicURIFromURI(finalSourceURI), encodedMappings, AddBreakpoint, finalGeneratedBreakpoints);
				}
			}
		}
	}

	//-----------------------------------------------------------------------

	void DeltaVM::ClearBreakpointList (BreakpointList& breakpoints) {
		BOOST_FOREACH(const Breakpoint bpt, breakpoints)
			RemoveBreakpoint(bpt.get<0>(), bpt.get<1>());
		breakpoints.clear();
	}

	//-----------------------------------------------------------------------

	void DeltaVM::RemoveSourceFromBreakpointList (BreakpointList& breakpoints, const String& symbolic) {
		for (BreakpointList::iterator i = breakpoints.begin(); i != breakpoints.end(); /*empty*/)
			if (i->get<0>() == symbolic)
				i = breakpoints.erase(i);
			else
				++i;
	}

	//-----------------------------------------------------------------------

	void DeltaVM::HandleInvalidBreakpoint(void)
	{
		std::string	source, cond;
		util_ui32	line, newLine;
 
		DeltaDebugClient::GetInfoInvalidBreakPoint(&source, &line, &newLine, &cond);
		DeltaDebugClient::ResponseProcessed();
		DeltaDebugClient::InvalidBreakPointReason reason = DeltaDebugClient::GetInvalidBreakPointReason(line, newLine);

		if (DeltaDebugClientBreakPoints::Get().HasBreakPoint(source, line)) {	// Was set before running
			
			DASSERT(!waitingBreakpointValidation);

			bool			wasEnabled	= DeltaDebugClientBreakPoints::Get().Get(source, line).IsEnabled();
			const String	symbolic	= util::std2str(source);
			const String	uri			= GetURIFromSymbolic(symbolic);

			if (reason == DeltaDebugClient::ConditionWasInvalid) {
				DeltaDebugClientBreakPoints::Get().Disable(source, line);
				CALL_DELAYED(boost::bind(&DeltaVM::onBreakpointDisabled, uri, symbolic, line));
				CALL_DELAYED(boost::bind(&DeltaVM::MessageConditionError, util::std2str(cond), line));
			}
			else {
				if (reason == DeltaDebugClient::LineWasInvalidButSuccceededToReposition && 
					!DeltaDebugClientBreakPoints::Get().HasBreakPoint(source, newLine)) { 

					DeltaDebugClientBreakPoints::Get().Change(source, line, newLine);

					CALL_DELAYED(boost::bind(&DeltaVM::onBreakpointRemoved, uri, symbolic, line, wasEnabled));
					CALL_DELAYED(boost::bind(&DeltaVM::onBreakpointAdded, uri, symbolic, newLine, util::std2str(cond)));

					if (!wasEnabled)
						CALL_DELAYED(boost::bind(&DeltaVM::DisableBreakpoint, symbolic, newLine));
				}
				else {
					DeltaDebugClientBreakPoints::Get().Remove(source, line);
					CALL_DELAYED(boost::bind(&DeltaVM::onBreakpointRemoved, uri, symbolic, line, wasEnabled));
				}
			}
		}
		else if (waitingBreakpointValidation){	// Set while running.
			if (reason == DeltaDebugClient::ConditionWasInvalid)
				CALL_DELAYED(boost::bind(&DeltaVM::MessageConditionError, util::std2str(cond), line));
			else if (
				reason == DeltaDebugClient::LineWasInvalidButSuccceededToReposition &&
				!DeltaDebugClientBreakPoints::Get().HasBreakPoint(source, newLine)
			)
				CommitAddBreakpoint(util::std2str(source), newLine, util::std2str(cond));
			waitingBreakpointValidation = false;
		}
		else
			DASSERT(false);
	}

	//-----------------------------------------------------------------------

	void DeltaVM::HandleValidBreakpointAdded(void)
	{
		std::string	source, cond;
		util_ui32	line;

		DeltaDebugClient::GetInfoValidBreakPoint(&source, &line, &cond);
		DeltaDebugClient::ResponseProcessed();

		if (waitingBreakpointValidation) {
			DASSERT(!DeltaDebugClientBreakPoints::Get().HasBreakPoint(source, line));
			CommitAddBreakpoint(util::std2str(source), line, util::std2str(cond));
			waitingBreakpointValidation = false;
		}
		else
			DASSERT(false);
	}

	//-----------------------------------------------------------------------

	void DeltaVM::onBreakpointAdded(const String& uri, const String& symbolic, int line, const String& condition)
	{
		sigBreakpointAdded(uri, symbolic, line, condition);
	}

	//-----------------------------------------------------------------------

	void DeltaVM::onBreakpointRemoved(const String& uri, const String& symbolic, int line, bool enabled)
	{
		sigBreakpointRemoved(uri, symbolic, line, enabled);
	}

	//-----------------------------------------------------------------------

	void DeltaVM::onBreakpointEnabled(const String& uri, const String& symbolic, int line)
	{
		sigBreakpointEnabled(uri, symbolic, line);
	}

	//-----------------------------------------------------------------------

	void DeltaVM::onBreakpointDisabled(const String& uri, const String& symbolic, int line)
	{
		sigBreakpointDisabled(uri, symbolic, line);
	}

	//-----------------------------------------------------------------------

	void DeltaVM::onBreakpointConditionChanged(const String& uri, const String& symbolic, int line, const String& condition)
	{
		sigBreakpointConditionChanged(uri, symbolic, line, condition);
	}

	//-----------------------------------------------------------------------

	void DeltaVM::onBreakpointLineChanged(const String& uri, const String& symbolic, int line, int newLine)
	{
		sigBreakpointLineChanged(uri, symbolic, line, newLine);
	}

	//-----------------------------------------------------------------------

	void DeltaVM::onBreakpointHit(const String& uri, const String& symbolic, int line)
	{
		sigBreakpointHit(uri, symbolic, line);
	}

	//-----------------------------------------------------------------------

	void DeltaVM::onFirstBreakpoint(void)
	{
		sigFirstBreakpoint();
	}

	//-----------------------------------------------------------------------

	void DeltaVM::onFirstBreakpointEnabled(void)
	{
		sigFirstBreakpointEnabled();
	}

	//-----------------------------------------------------------------------

	void DeltaVM::onFirstBreakpointDisabled(void)
	{
		sigFirstBreakpointDisabled();
	}

	//-----------------------------------------------------------------------

	void DeltaVM::onNoBreakpoints(void)
	{
		sigNoBreakpoints();
	}

	//-----------------------------------------------------------------------

	void DeltaVM::onNoBreakpointsEnabled(void)
	{
		sigNoBreakpointsEnabled();
	}

	//-----------------------------------------------------------------------

	void DeltaVM::onNoBreakpointsDisabled(void)
	{
		sigNoBreakpointsDisabled();
	}

	//-----------------------------------------------------------------------

}
