// DeltaConsoleDebugger.cpp
// Disco.
// A console version of the Delta debugger using
// remote / local connection with the debugged Delta program.
// This is a new version of the original compact console debugger (1999).
// ScriptFighter Project.
// A. Savidis, September 2005.
// Revised and enhanced September 2007, debug loader was automatically 
// spawned (cancelled now).
// Revised June 2008, supporting connect to / execute programs already 
// encompassing the debugger kerner.
// Extended August 2008, added suport for aggregate value browsing via the
// content encoding facility of the debugger backend.
// Extended August 2009, support debugger (server) port and breakpoints file as
// command line arguments (needed to integrate the console debugger in Sparrow).
// Extended May 2010, added support for configuration parms when getting object graph.
//

#include <stdio.h>
#include <stdlib.h>

#include "DDebug.h"
#include "DebugClient.h"
#include "DebugBreakPointHolder.h"
#include "DeltaDebugClientBreakPoints.h"
#include "uinit.h"
#include "usystem.h"
#include "ulexutil.h"
#include "VMInit.h"
#include "uerrorclass.h"
#include "DebugWatchValueInfo.h"
#include "DeltaTable.h"
#include "RcLoaderAPI.h"

#include <list>
#include <string>
#include <vector>

//////////////////////////////////////////////////////////////////////////

#define	ABOUT							"\nDelta console debugger (Disco), version 4.0 (May 2010).\n"	\
										"(c) Institute of Computer Science - FORTH, Anthony Savidis <as@ics.forth.gr>.\n\n"
#define	DEBUGGER_DEFAULT_PORT			0
#define	BREAKPOINTS_DEFAULT_FILE		"break.dat"
#define	CURRENT_FILE					"$$"
#define	CONDITION_EMPTY					"$$"
#define	LOCAL_HOST						"$$"
#define	REQUIRES_IN_BREAKPOINT()		if (!inBreakPoint) return; else
#define	KLOCS(n)						((n) << 10)
#define	CALL_FUNCARG_MAX_LEN			70
#define	FilterCondition(cond)			((cond) == CONDITION_EMPTY ? "" : (cond).c_str())
#define	OPERATOR_METHODS_PREFIX			"operator "
#define	OBJECT_OPERATOR_KEY				"OBJECT_OPERATOR_KEY"
#define	BINARY_DATA_FILE				"bin.bin"

static const std::string FilterHost (const std::string& host)
	{ return host == LOCAL_HOST ? "localhost" : host; }

static bool inGlobal				= true;
static bool inBreakPoint			= false;
static bool	quit					= false;
static bool	connected				= false;
static bool waitingBptValidation	= false;
static bool justConnected			= false;

static std::string											stopContext;
static std::list< std::pair<std::string, std::string> >		returnValuesAutoList;
static std::vector<std::string> 							sourceCode;
static std::string											sourceFile;
static std::string											breakpointsFile = BREAKPOINTS_DEFAULT_FILE;
static util_ui32											debuggerPort	= DEBUGGER_DEFAULT_PORT;

//*****************************

static const std::string	MakeContext (const std::string& name, util_ui32 defLine)
								{ return uconstructstr("%s line %d", name.c_str(), defLine); }

static const std::string	GetCurrentContext (void) 
								{ return stopContext; }

static void 				SetContext (const std::string& name, util_ui32 defLine)
								{ stopContext = MakeContext(name, defLine); }

static void 				UpdateReturnValue (const std::string& func, const std::string& value);

static const std::string	ProduceReturnValuesDisplay (void);

static void					ClearReturnValuesAutoList (void) 
								{ returnValuesAutoList.clear(); }

//*****************************

void UpdateReturnValue (const std::string& func, const std::string& value) {

	std::list< std::pair<std::string, std::string> >::iterator i;
	i = std::find_if(
		returnValuesAutoList.begin(),
		returnValuesAutoList.end(),
		std::bind2nd(uequal_first< std::pair<std::string, std::string> >(), func)
	);

	if (i != returnValuesAutoList.end())
		returnValuesAutoList.erase(i);
	
	returnValuesAutoList.push_back(
		std::pair<std::string, std::string>(func, value)
	);
}

//*****************************

const std::string  ProduceReturnValuesDisplay (void) {
	std::string s;
	std::list< std::pair<std::string, std::string> >::iterator i;
	for (i = returnValuesAutoList.begin(); i != returnValuesAutoList.end(); ++i)
		s += i->first + "() returned: '" + i->second + "'.\n";
	return s;
}

//*****************************

void CheckHandleMostRecentFunctionResults (void) {

	returnValuesAutoList.clear();
	bool haveAny = false;	

	DeltaDebugClient::WaitAnyMessage();
	DASSERT(DeltaDebugClient::GetResponseType() == Debug_InfoMostRecentFuncResults);

	std::list< std::pair<std::string, std::string> > results;
	if (!DeltaDebugClient::GetMostRecentFuncResults(results))
		DASSERT(false);

	DeltaDebugClient::ResponseProcessed();

	for (	std::list< std::pair<std::string, std::string> >::iterator i = results.begin();
			i  != results.end();
			++i		)

			UpdateReturnValue(i->first, i->second);

	if (!results.empty())
		fprintf(stdout, "%s\n", ProduceReturnValuesDisplay().c_str());
}

///////////////////////////////////////////////////////////////////

static void LoadSourceFromString (const std::string& code) {

	sourceCode.clear();
	std::string line;

	for (std::string::const_iterator i = code.begin(); i != code.end(); ++i)
		if (*i == '\n')
			{ sourceCode.push_back(line); line.clear(); }
		else
			line.push_back(*i);

	if (!line.empty())
		sourceCode.push_back(line);
}

///////////////////////////////////////////////////////////////////

static void LoadSource (const char* path) {

	if (sourceFile == path)	// Already loaded.
		return;
	sourceFile = path;

	FILE* fp = fopen(path, "rt");
	if (!fp)
		return;

	sourceCode.clear();
	sourceCode.push_back("");	// Line 0

	while (!feof(fp)) {
		static char s[1024];
		if (!fgets(s, sizeof(s) - 1, fp))
			continue;
		sourceCode.push_back(udeleoln(s));
	}

	fclose(fp);
}

///////////////////////////////////////////////////////////////////
// Debugger UI pannels.
//
#define	CD_NOTRUN_COMMANDS \
	"| [1] Connect      | [2] Execute           |\n"
	
#define	CD_RUNBREAKPOINT_COMMANDS \
	"| [S]tep over      | Step [i]n             | Selecti[v]e step in   |\n" \
	"| Step [o]ut       | [R]un to              | Callstac[k]           |\n" \
	"| Stack [d]own     | Stack [u]p            | Print loc[a]l         |\n" \
	"| Print ac[t]ual   | Clear [f]unc results  | Print g[l]obal        |\n" \
	"| Print E[x]pr     | Bro[w]se expr         | Get object grap[h]    |\n" \
	"| Binar[y] data	|\n"

#define	CD_RUNSTANDARD_COMMANDS \
	"| [G]o             | Set [m]ax len         | Co[n]figure graph     |\n" \
	"| [3] Stop         | 4. Break              |\n"

#define	CD_ALWAYS_COMMANDS \
	"|(B)reak at        | D(e)lete bpt          | (C)lear bpts          |\n" \
	"| [5] About        | [6] Quit              |\n"

//*******************************

static void ShowCommands (void) {

	if (!connected)
		printf("%s", CD_NOTRUN_COMMANDS);
	else {
		if (inBreakPoint)
			printf("%s", CD_RUNBREAKPOINT_COMMANDS);
		printf("%s", CD_RUNSTANDARD_COMMANDS);
	}

	printf("%s", CD_ALWAYS_COMMANDS);
	printf("Command?");
}

static void ShowMessage (const char* msg) 
	{ printf("\n>>>%s", msg); }

static void KeyToContinue (void) {
	printf("\nPress any key to continue...\n");
	ugetch();
}

//*******************************

static const std::string GetString (const char* msg) {
	while (true) {
		printf("%s", msg);
		static char s[1024];
		fgets(s, sizeof(s) - 1, stdin);
		udeleoln(s);
		if (s[0])
			return s;
	}
}

//*******************************

static bool GetYesNo (const char* msg, bool* val) {
	std::string s = GetString(msg);
	ustrlower(s);
	if (s == "yes" || s == "y")
		{ *val = true; return true; }
	else
	if (s == "no" || s == "n")
		{ *val = false; return true; }
	else
		{ printf("Invalid input '%s' (yes / no expected)!\n", s.c_str()); return false; }
}

//*******************************

template <class T>
bool GetNumber (const char* msg, T* num) {
	DASSERT(sizeof(T) == sizeof(util_ui32)); // Only integers
	std::string val = GetString(msg);
	if (sscanf(val.c_str(), "%d", num) != 1)
		{ printf("Invalid input '%s' (not a number)!\n", val.c_str()); return false; }
	else
		return true;
}

//*******************************

static const std::string GetFile (void) {
	std::string source =	GetString(
								uconstructstr(
									"File%s:", 
									connected && inBreakPoint?  "(use $$ for current)" : "").c_str()
							);
	if (connected && inBreakPoint && source == CURRENT_FILE)
		source = sourceFile;
	return source;
}

//*******************************

static const std::string GetName (const char* msg) 
	{ return GetString(msg); }

///////////////////////////////////////////////////////////////////

static void CDAbout (void) {
	ShowMessage("About\n");
	fprintf(stdout, "%s", ABOUT);
	fflush(stdout);
} 

///////////////////////////////////////////////////////////////////

static void CDGo (void) {
	ShowMessage("Go\n");
	if (justConnected) {
		DeltaDebugClient::DoStart();
		justConnected = false;
	}
	DeltaDebugClient::DoGo();
	inBreakPoint = false;
} 

//////////////////////////////////////////////////////////////

static void CDStepOver (void){
	REQUIRES_IN_BREAKPOINT();
	ShowMessage("Step\n");
	DeltaDebugClient::DoStepOver();
	inBreakPoint = false;
}

//////////////////////////////////////////////////////////////

static void CDStepIn (void){
	REQUIRES_IN_BREAKPOINT();
	ShowMessage("StepIn\n");
	DeltaDebugClient::DoStepIn();
	inBreakPoint = false;
}

//////////////////////////////////////////////////////////////

typedef std::list< std::pair<std::string, std::string> > CallList;

static void ShowCallOptions (CallList& calls) {
	util_ui32 n = 1;

	for (CallList::iterator i = calls.begin(); i != calls.end(); ++i, ++n) {
		std::string call(i->first + "\t(" + i->second + ")");
		fprintf(
			stdout, 
			"%d:\t%s\n",
			n, 
			ucropstr(call, CALL_FUNCARG_MAX_LEN, "..").c_str()
		);
	}
	fprintf(stdout, "0:\tCancel\n"); 
}

//*************************

static void CDSelectiveStepIn (void){

	REQUIRES_IN_BREAKPOINT();
	ShowMessage("SelectiveStepIn\n");

	DeltaDebugClient::DoGetAllPossibleCalls();

	DeltaDebugClient::WaitAnyMessage();
	DASSERT(DeltaDebugClient::GetResponseType() == Debug_InfoAllPossibleCalls);

	CallList calls;
	DeltaDebugClient::GetInfoAllPossibleCalls(calls);
	DASSERT(calls.empty() || calls.size() > 1);
	DeltaDebugClient::ResponseProcessed();

	if (calls.empty())
		ShowMessage("Not applicable at present point\n");
	else
		while (true) {
			ShowCallOptions(calls);
			util_i32 n;
			if (sscanf(GetString("Option:").c_str(), "%d", &n) != 1 || n < 0 || n > (util_i32) calls.size())
				ShowMessage("Invalid option!\n");
			else
			if (n == 0)
				return; // Cancelled.
			else {
				DeltaDebugClient::DoSelectiveStepIn((util_ui32) n - 1);
				inBreakPoint = false;
				return;
			}
		}
}

//////////////////////////////////////////////////////////////

static void CDStepOut (void){
	REQUIRES_IN_BREAKPOINT();
	ShowMessage("StepOut\n");
	DeltaDebugClient::DoStepOut();
	inBreakPoint = false;
}

//////////////////////////////////////////////////////////////

static void CDRunTo (void){
	REQUIRES_IN_BREAKPOINT();
	ShowMessage("RunTo\n");
	util_ui32 line;
	if (GetNumber("Line:", &line)) {
		DeltaDebugClient::DoRunTo(line);
		inBreakPoint = false;
	}
}

//////////////////////////////////////////////////////////////

static void OnInfoValidBreakPoint (void) {

	DASSERT(waitingBptValidation);

	std::string	source;
	util_ui32	line;
	std::string	cond;

	DeltaDebugClient::GetInfoValidBreakPoint(&source, &line, &cond);

	DeltaDebugClientBreakPoints::Get().Add(	// Deferred addition only by validation.
		source.c_str(), 
		line, 
		FilterCondition(cond)
	);

	waitingBptValidation = false;
}

//*****************************

static void CDBreakAt (void) {

	// Operation temporarily unavailable. This is an unlike scenario in DISCO.
	if (connected && waitingBptValidation)	
		return;

	ShowMessage("BreakAt\n");
	
	std::string source = GetFile();
	util_ui32 line;
	if (!GetNumber("Line:", &line))
		return;

	std::string cond = GetString("Cond (use $$ for empty):");

	if (connected) {
		waitingBptValidation = true;
		DeltaDebugClient::DoAddBreakPoint(
			source.c_str(), 
			line, 
			FilterCondition(cond)
		);
	}
	else
	DeltaDebugClientBreakPoints::Get().Add(
		source.c_str(), 
		line, 
		FilterCondition(cond)
	);
}

//////////////////////////////////////////////////////////////

static void CDSetMaxLength (void){
	ShowMessage("SetMaxLength\n");
	util_ui32 maxLen;
	if (GetNumber("Length:", &maxLen))
		DeltaDebugClient::DoSetToStringMaxLength(maxLen);
}

//////////////////////////////////////////////////////////////

static void CDConfigureObjectGraph (void){

	REQUIRES_IN_BREAKPOINT();
	ShowMessage("ConfigureObjectGraph\n");

	bool expandLibraryFuncs;
	bool expandMethodFuncs;
	bool expandProgramFuncs;
	bool expandFuncVMs;

	printf("Respond with [y]es / [n]o\n");

	if (GetYesNo("Expand library funcs ?",	&expandLibraryFuncs	) &&
		GetYesNo("Expand method funcs  ?",	&expandMethodFuncs	) &&
		GetYesNo("Expand program funcs ?",	&expandProgramFuncs	) &&
		GetYesNo("Expand func vms      ?",	&expandFuncVMs	))

		DeltaDebugClient::DoSetObjectGraphConfiguration(
			expandLibraryFuncs,
			expandMethodFuncs,
			expandProgramFuncs,
			expandFuncVMs
		);
}

//////////////////////////////////////////////////////////////

static void CDDeleteBreakpoint (void){

	ShowMessage("DeleteBreakpoint\n");
	std::string source = GetFile();
	util_ui32 line;
	if (!GetNumber("Line:", &line))
		return;

	DeltaDebugClientBreakPoints::Get().Remove(source.c_str(), line);
	if (connected)
		DeltaDebugClient::DoRemoveBreakPoint(source.c_str(), line);
}

//////////////////////////////////////////////////////////////

static void CDClearBreakpoints (void){
	ShowMessage("ClearBreakpoints\n");
	std::string source = GetFile();

	DeltaDebugClientBreakPoints::Get().RemoveAll(source.c_str());
	if (connected)
		DeltaDebugClient::DoRemoveAllBreakPoints(source.c_str());
}

static void CDClearFunctionResults (void) {
	ShowMessage("ClearFunctionResults\n");
	ClearReturnValuesAutoList();
}

//////////////////////////////////////////////////////////////

static bool ReceiveFunction (bool callStackManage = false) {

	DASSERT(DeltaDebugClient::GetResponseType() == Debug_InfoCurrFunction);

	std::string func, call;
	util_ui32	defLine, callLine, scope;

	DeltaDebugClient::GetInfoCurrFunction(&func, &defLine, &callLine, &scope, &call);

	fprintf(
		stdout, 
		"%s%s%s, called at %d, scope %d, defined at %d.\n", 
		callStackManage ? "\n" : "",
		func.c_str(),
		call.c_str(),
		callLine,
		scope,
		defLine
	);
	fflush(stdout);
	DeltaDebugClient::ResponseProcessed();

	if (!callStackManage) {
		if (MakeContext(func, defLine) != GetCurrentContext()) {
			ClearReturnValuesAutoList();
			SetContext(func, defLine);
		}

		CheckHandleMostRecentFunctionResults();
	}

	return defLine;
}

//////////////////////////////////////////////////////////////

static void ReceiveVariables (void) {

	DeltaDebugClient::DoGetVariables();
	DeltaDebugClient::WaitAnyMessage();

	if (DeltaDebugClient::GetResponseType() == Debug_InfoVariables) {
		std::list< std::pair<std::string, std::string> > vars;
		DeltaDebugClient::GetInfoVariables(vars);
		DeltaDebugClient::ResponseProcessed();

		fprintf(stdout, "Variables at current context.\n");

		for (	std::list< std::pair<std::string, std::string> > ::iterator i = vars.begin();
				i != vars.end();
				++i	)
				if (i->first == DBG_GETVARIABLES_NAME_IS_GROUP)
					fprintf(stdout, "[%s]\n", i->second.c_str());
				else
					fprintf(stdout, "%s = %s\n",  i->first.c_str(), i->second.c_str());
	}
}

//////////////////////////////////////////////////////////////
// Print call records from bottom to top.
//

void AllTheWayStackDown(void) {

	DS2CR type;

	do {
		DeltaDebugClient::DoStackDown();
		DeltaDebugClient::WaitAnyMessage();

		type = DeltaDebugClient::GetResponseType();
		DeltaDebugClient::ResponseProcessed();

		DASSERT(type == Debug_InfoCurrFunction || type == Debug_InfoGlobalContext);

	} while (type != Debug_InfoGlobalContext);
}

static void CDCallStack (void){

	REQUIRES_IN_BREAKPOINT();
	ShowMessage("CallStack\n");

	AllTheWayStackDown();
	fprintf(stdout,"Bottom of call stack.\n");
	fflush(stdout);

	while (1) {

		DeltaDebugClient::DoStackUp();
		DeltaDebugClient::WaitAnyMessage();

		if (DeltaDebugClient::GetResponseType() == Debug_InfoTopContext) {
			fprintf(stdout,"Top of call stack.\n");
			fflush(stdout);
			DeltaDebugClient::ResponseProcessed();
			break;
		}
		else {
			ReceiveFunction(true);
			inGlobal = false;
		}
	}
}

//////////////////////////////////////////////////////////////

static void CDStackDown (void){

	REQUIRES_IN_BREAKPOINT();
	DeltaDebugClient::DoStackDown();
	DeltaDebugClient::WaitAnyMessage();

	if (DeltaDebugClient::GetResponseType() == Debug_InfoGlobalContext) {
		fprintf(stdout,"\nAt global level.\n");
		fflush(stdout);
		inGlobal = true;
		DeltaDebugClient::ResponseProcessed();
	}
	else {
		ReceiveFunction(true);
		inGlobal = false;
	}	
}

//////////////////////////////////////////////////////////////

static void CDStackUp (void){

	REQUIRES_IN_BREAKPOINT();
	DeltaDebugClient::DoStackUp();
	DeltaDebugClient::WaitAnyMessage();

	if (DeltaDebugClient::GetResponseType() == Debug_InfoTopContext) {
		fprintf(stdout,"\nAt top called function.\n");
		fflush(stdout);
		DeltaDebugClient::ResponseProcessed();
	}
	else {
		ReceiveFunction(true);
		inGlobal = false;
	}
}

//////////////////////////////////////////////////////////////

static const std::string ReceiveValue (void) {

	DeltaDebugClient::WaitAnyMessage();

	DASSERT(
		DeltaDebugClient::GetResponseType() == Debug_InfoValue ||
		DeltaDebugClient::GetResponseType() == Debug_InfoErrorValue
	);

	std::string value;

	if (DeltaDebugClient::GetResponseType() == Debug_InfoValue)
		DeltaDebugClient::GetInfoValue(&value);
	else
		DeltaDebugClient::GetInfoErrorValue(&value);
	return value;
}

//////////////////////////////////////////////////////////////

static void PrintVariable (void (*f)(const std::string&)) {

	std::string name = GetName("Name / expr:");
	(*f)(name);
	fprintf(stdout, "%s\n", ReceiveValue().c_str());

	DeltaDebugClient::ResponseProcessed();
}

//*******************************

static void CDPrintLocal (void) {
	REQUIRES_IN_BREAKPOINT();
	ShowMessage("PrintLocal\n");
	if (inGlobal)
		printf("Currently at global execution level.\n");
	else
		PrintVariable(&DeltaDebugClient::DoGetLocal);
}

//*******************************

static void CDPrintGlobal (void) {
	REQUIRES_IN_BREAKPOINT();
	ShowMessage("PrintGlobal\n");
	PrintVariable(&DeltaDebugClient::DoGetGlobal);
}

//*******************************

static void CDPrintActual (void) {
	REQUIRES_IN_BREAKPOINT();
	ShowMessage("PrintActual\n");
	if (inGlobal)
		printf("Currently at global execution level.\n");
	else
		PrintVariable(&DeltaDebugClient::DoGetActual);
}

//*******************************

static void CDPrintExpr (void) {
	REQUIRES_IN_BREAKPOINT();
	ShowMessage("PrintExpr\n");
	PrintVariable(&DeltaDebugClient::DoGetExpr);
}

///////////////////////////////////////////////////////////////////
// EXPRESSION BROWSER.

extern bool DebugWatchValueRcDecoder (const std::string& rc, DebugWatchValueInfo* at);

static bool ReceiveExprTypeData (std::string* value) {
	DeltaDebugClient::WaitAnyMessage();
	bool result = false;
	if (DeltaDebugClient::GetResponseType() == Debug_InfoExprTypeData)
		{ DeltaDebugClient::GetInfoExprTypeData(value); result = true; }
	else
	if (DeltaDebugClient::GetResponseType() == Debug_InfoErrorValue)
		DeltaDebugClient::GetInfoErrorValue(value);
	else
		DASSERT(false);
	DeltaDebugClient::ResponseProcessed();
	return result;
}

//*******************************

struct Elem {
	std::string index;		// expr + subIndex, or key ref
	std::string	dpy;		// desc + displayedKey, or displayed key
	bool		visible;	// visibility, or true for key ref
	Elem (const std::string i, const std::string& d, bool v) :
		index(i), dpy(d), visible(v){}
	Elem (const Elem& e) :
		index(e.index), dpy(e.dpy), visible(e.visible){}
};

//*******************************

static void ExtractElements (
		const std::string&			prefix, 
		std::list<Elem>&			elems, 
		const DebugWatchValueInfo&	info,
		std::string*				overview
	) {

	const DebugWatchValueInfo::Composite&			composite	= info.GetComposite();
	const std::list<DebugWatchValueInfo::Content>&	contents	= composite.GetContents();
	*overview = composite.GetOverview();

	for (	std::list<DebugWatchValueInfo::Content>::const_iterator i = contents.begin();
			i != contents.end();
			++i		) {

			std::string dpy;
			const std::list<DebugWatchValueInfo::FieldKey>& fieldKeys = i->GetFieldKeys();
			
			for (	std::list<DebugWatchValueInfo::FieldKey>::const_iterator j = fieldKeys.begin();
					j != fieldKeys.end();
					++j		) {

					if (!dpy.empty())
						dpy.append(",");
					dpy.append(j->GetDisplayedText());

					if (j->HasContentReference()) // We have a composite key value that can be browsed too.
						elems.push_back(
							Elem(
								j->GetContentReference(),
								j->GetDisplayedText() + "#key", // To show it is a key.
								true
							)
						);
			}

			if (i->HasKeyType() && i->GetKeyType() == OBJECT_OPERATOR_KEY)
				dpy = std::string(OPERATOR_METHODS_PREFIX) + dpy;

			elems.push_back(
				Elem(
					prefix + i->GetSubIndex(),
					i->GetDisplayDesc() + dpy,
					i->IsKeyVisible()
				)
			);
	}
}

//*******************************

static void ShowElements (
		const std::list<Elem>&	elems, 
		const std::string&		expr, 
		const std::string&		overview, 
		bool					visible
	) {

	if (visible)
		fprintf(stdout, "Expr: %s\n", expr.c_str()); 
	fprintf(stdout, "%s\n", overview.c_str());

	util_ui16 j = 1;
	for (std::list<Elem>::const_iterator i = elems.begin(); i != elems.end(); ++i, ++j)
		fprintf(stdout, "%d:\t%s\n", j, i->dpy.c_str()); 

	fprintf(stdout, "0:\tBack\n"); 
	fprintf(stdout, "-1:\tMain menu\n"); 
}

//*******************************
// True from a recursive call means stay in the dialogue, false, means
// return from all recursive calls.
//
static bool ExprBrowser (const std::string& expr, bool visible) {
	
	DeltaDebugClient::DoGetExprTypeData("rc", expr);
	std::string result;
	if (!ReceiveExprTypeData(&result))
		{ fprintf(stdout, "%s\n", result.c_str()); return true; }

	DebugWatchValueInfo info;
	if (!DebugWatchValueRcDecoder(result, &info))
		{ fprintf(stdout, "Decoding error!\n"); return true; }

	if (info.GetType() == DebugWatchValueInfo::SimpleType)
		{ fprintf(stdout, "Value: %s\n", info.GetSimple().c_str()); return true; }

	std::list<Elem> elems;
	std::string		overview;
	ExtractElements(expr, elems, info, &overview);
	if (elems.empty())
		{ fprintf(stdout, "Value: %s\n", overview.c_str()); return true; }

	while (true) {
		ShowElements(elems, expr, overview, visible);
		util_i32 n;
		if (sscanf(GetString("Option:").c_str(), "%d", &n) != 1 || n < -1 || n > (util_i32) elems.size())
			ShowMessage("Invalid option!\n");
		else
		if (n == 0)
			return true; // Back one step.
		else
		if (n == -1)
			return false; // Direct exit.
		else {
			Elem& elem = ulistgetbyindex(elems, n - 1);
			if (!ExprBrowser(elem.index, visible & elem.visible)) // False means exit to main menu requested.
				return false;
		}
	}
}

//*******************************

static void CDBrowseExpr (void) {
	REQUIRES_IN_BREAKPOINT();
	ShowMessage("BrowseExpr\n");
	ExprBrowser(GetName("Expr:"), true);
}

///////////////////////////////////////////////////////////////////

static void CDBinaryData (void) {
	REQUIRES_IN_BREAKPOINT();
	ShowMessage("BinaryData\n");

	std::string expr = GetName("Expr:");
	DeltaDebugClient::DoGetBinaryData(expr);
	DeltaDebugClient::WaitAnyMessage();

	if (DeltaDebugClient::GetResponseType() == Debug_InfoErrorValue) {
		std::string error;
		DeltaDebugClient::GetInfoErrorValue(&error);
		DeltaDebugClient::ResponseProcessed();
		printf("%s\n", error.c_str());
	}
	else {
		DASSERT(DeltaDebugClient::GetResponseType() == Debug_InfoBinaryData);
		void* data; util_ui32 size;
		DeltaDebugClient::GetInfoBinaryData(&data, &size);
		if (FILE* fp = fopen(BINARY_DATA_FILE, "w")) {
			fwrite(data, size, 1, fp);
			fclose(fp);
			printf("Binary data written to '%s'\n", BINARY_DATA_FILE);
		}
		else
			printf("Failed to open '%s' to write binary data!\n", BINARY_DATA_FILE);
		DeltaDebugClient::ClearBinaryData(&data, &size);
	}
}

///////////////////////////////////////////////////////////////////

static void CDObjectGraph (void) {

	REQUIRES_IN_BREAKPOINT();
	ShowMessage("ObjectGraph\n");

	std::string expr = GetName("Expr:");
	util_ui32 depth;
	if (!GetNumber("Depth (0 for unlimited):", &depth))
		return;

	bool directed;
	if (!GetYesNo("Directed graph ([y]es / [n]o)?:", &directed))
		return;

	DeltaDebugClient::DoGetObjectGraph(expr, depth, directed);
	DeltaDebugClient::WaitAnyMessage();

	if (DeltaDebugClient::GetResponseType() == Debug_InfoErrorValue) {
		std::string error;
		DeltaDebugClient::GetInfoErrorValue(&error);
		DeltaDebugClient::ResponseProcessed();
		printf("%s\n", error.c_str());
	}
	else
	if (DeltaDebugClient::GetResponseType() == Debug_InfoValue) {
		std::string content;
		DeltaDebugClient::GetInfoValue(&content);
		DeltaDebugClient::ResponseProcessed();
		printf("Expression is simple: %s\n", content.c_str());
	}
	else {
		DASSERT(DeltaDebugClient::GetResponseType() == Debug_InfoObjectGraph);

		typedef DeltaDebugClient::ObjectGraph	Graph;
		typedef DeltaDebugClient::ObjectNode	Node;

		Graph graph;
		DeltaDebugClient::GetInfoObjectGraph(graph);
		DeltaDebugClient::ResponseProcessed();

		typedef std::map<std::string, std::string>				InfoMap;
		typedef std::map<std::string, std::list<std::string> >	EdgeMap;
		typedef	ubag<std::string>								NameBag;
		typedef	std::list<std::string>							NameList;

		InfoMap		names, labels;
		EdgeMap		outEdges, inEdges;
		std::string	startNodeAbsRef, startNodeName;
	
		// Assign logical node names.
		util_ui32 n = 0;

		// Determine start node.
		Graph::iterator i = graph.find(DBG_GRAPH_START_NODE);
		DASSERT(i != graph.end());
		DASSERT(i->second.size() == 1);										// Just a single node,
		DASSERT(i->second.front().first == DBG_GRAPH_START_NODE_ABSREF);	// with the absref
		startNodeAbsRef = i->second.front().third;							// of the starting node,
		DASSERT(graph.find(startNodeAbsRef) != graph.end());				// that is also found in the graph.
		graph.erase(i);														// Remove the artificial node.
		
		for (i = graph.begin(); i != graph.end(); ++i) {
			names[i->first] = uconstructstr("node%u", n++);
			if (startNodeAbsRef == i->first)
				startNodeName = names[i->first];
		}
		
		// Produce outEdges and labels.
		for (i = graph.begin(); i != graph.end(); ++i) {

			std::string label;
			std::string curr = names[i->first];

			#define	HANDLE_ATTR(str, flag)									\
				if (true)													\
					if (j->flag) {											\
						InfoMap::iterator q = names.find(j->str);			\
						if (q != names.end()) {								\
							outEdges[curr].push_back(q->second);			\
							inEdges[q->second].push_back(curr);				\
						}													\
					}														\
				else

			for (Node::iterator j = i->second.begin(); j != i->second.end(); ++j) {
				HANDLE_ATTR(first, second);
				HANDLE_ATTR(third, fourth);
				if (!label.empty())
					label.append("\n");
				label.append(uconstructstr("%s : %s", j->first.c_str(), j->third.c_str()));
			}

			labels[curr] = i->first + "\n" + label;	// Abs ref
		}
		
		#undef	HANDLE_ATTR

		// Make the dot file.
		FILE* fp = fopen("dot.gv", "wt");
		fprintf(fp, "digraph G {\n");

		// Write outEdges, starting from the start node in a bridth-first fashion.
		// Also, we use incoming edges to ensure all nodes are considered in case
		// the objcect graph was requested assuming undirected edges.

		ubag<std::string>		visited;
		NameBag					curr;
		curr.insert(startNodeName);

		while (!curr.empty()) {

			NameBag next;
			for (NameBag::iterator i = curr.begin(); i != curr.end(); ++i) {

				DASSERT(!visited.in(i->first));
				visited.insert(i->first);

				NameList& l1 = outEdges[i->first]; // Outgoing edges.
				for (NameList::iterator j = l1.begin(); j != l1.end(); ++j) {
					fprintf(fp, "\t%s -> %s;\n", i->first.c_str(), j->c_str());
					if (!visited.in(*j) && !next.in(*j))
						next.insert(*j);
				}

				NameList& l2 = inEdges[i->first]; // Use incoming edges to visit all nodes.
				for (NameList::iterator j = l2.begin(); j != l2.end(); ++j)
					if (!visited.in(*j) && !next.in(*j))
						next.insert(*j);
			}

			curr.clear();
			curr.insert(next);
			next.clear();
		}

		// Write nodes.
		for (InfoMap::iterator i = labels.begin(); i != labels.end(); ++i)
			fprintf(
				fp, 
				"\t%s [\n\t\tshape = box,\n\t\tlabel=\"%s\"\n\t];\n", 
				i->first.c_str(), 
				uunparseescapesequences(i->second.c_str()).c_str()
			);

		fprintf(fp, "}");
		fclose(fp);
		printf("Produced dot.gv for GraphViz software.\n");
	}
}

///////////////////////////////////////////////////////////////////

static void CDQuit (void) {
	ShowMessage("Quit\n");
	if (connected) {
		DeltaDebugClient::DoStop();
		DeltaDebugClient::CleanUp();
	}
	quit = true;
}

static void CDBreakExecution (void) {
	ShowMessage("BreakExecution\n");
	if (connected)
		DeltaDebugClient::DoBreakExecution();
}

//////////////////////////////////////////////////////////////

static void ConnectWithDebugger (util_ui32 port, const std::string host) {
	DeltaDebugClient::Initialise();
	if (connected = DeltaDebugClient::Connect(host, port)) {
		justConnected = true;
		fprintf(stderr, "Connected with the program at '%s'.\n", host.c_str());
		fflush(stderr);
	}
	else {
		fprintf(stderr, "Failed to connect with program at '%s'.\n", host.c_str());
		fflush(stderr);
		DeltaDebugClient::CleanUp();
	}
}

//****************************

static void CDConnect  (void) {

	DASSERT(!connected);

	ShowMessage("Connect\n");
	std::string host = FilterHost(GetString("Host (use $$ for localhost):"));
	util_ui32 port;
	if (GetNumber("Port:", &port))
		ConnectWithDebugger(port, host);
}

//////////////////////////////////////////////////////////////

static void CDExecute (void) {

	DASSERT(!connected);

	ShowMessage("Execute\n");
	std::string program = GetString("Exe:");

	DeltaDebugClient::Initialise();

	util_ui32	port	= DeltaDebugClient::ServerPortNegotiation::Start();
	std::string arg		= DeltaDebugClient::ServerPortNegotiation::ToArg(port);

	const char* args[] = { program.c_str(), arg.c_str(), NULL };
	int result = uspawnv(program.c_str(), args, false);

	if (result == -1) {
		fprintf(stderr, "Failed to execute '%s'.\n", program.c_str());
		fflush(stderr);
		return;
	}

	port = DeltaDebugClient::ServerPortNegotiation::Finish();
	if (!port) {
		fprintf(stderr, "Failed to negotiate port with the program.\n");
		fflush(stderr);
		DeltaDebugClient::CleanUp();
	}
	if (connected = DeltaDebugClient::Connect("localhost", port)) {
		justConnected = true;
		fprintf(stderr, "Connected with the program locally.\n");
		fflush(stderr);
	}
	else {
		fprintf(stderr, "Failed to connect with program locally.\n");
		fflush(stderr);
		DeltaDebugClient::CleanUp();
	}
}

//////////////////////////////////////////////////////////////

static void DisconnectWithProgram (void) {
	try {
		DeltaDebugClient::CleanUp();
	} catch (DeltaDebugClient::BrokenConnection){}

	connected				= false;
	inGlobal				= true;
	inBreakPoint			= false;
	connected				= false;
	waitingBptValidation	= false;
}

static void CDStop  (void) {
	ShowMessage("Stop\n");
	DASSERT(connected);
	DeltaDebugClient::DoStop();
	DisconnectWithProgram();
}

//////////////////////////////////////////////////////////////
// Dynamic menu based on handler functions and preconditions
// (predicates).
//
typedef void (*Handler)(void);
static std::map<char, Handler> handlers;

static void InstalHandlers (void) {

	// Run extra (break points).
	//
	handlers['S'] = handlers['s'] = &CDStepOver;
	handlers['I'] = handlers['i'] = &CDStepIn;
	handlers['V'] = handlers['v'] = &CDSelectiveStepIn;
	handlers['O'] = handlers['o'] = &CDStepOut;
	handlers['R'] = handlers['r'] = &CDRunTo;
	handlers['K'] = handlers['k'] = &CDCallStack;
	handlers['D'] = handlers['d'] = &CDStackDown;
	handlers['U'] = handlers['u'] = &CDStackUp;
	handlers['A'] = handlers['a'] = &CDPrintLocal;
	handlers['T'] = handlers['t'] = &CDPrintActual;
	handlers['L'] = handlers['l'] = &CDPrintGlobal;
	handlers['X'] = handlers['x'] = &CDPrintExpr;
	handlers['W'] = handlers['w'] = &CDBrowseExpr;
	handlers['H'] = handlers['h'] = &CDObjectGraph;
	handlers['F'] = handlers['f'] = &CDClearFunctionResults;
	handlers['Y'] = handlers['y'] = &CDBinaryData;

	// Run standard.
	//
	handlers['G'] = handlers['g'] = &CDGo;
	handlers['M'] = handlers['m'] = &CDSetMaxLength;
	handlers['N'] = handlers['n'] = &CDConfigureObjectGraph;
	handlers['3'] = &CDStop;
	handlers['4'] = &CDBreakExecution;

	// Not run.
	//
	handlers['1'] = &CDConnect;
	handlers['2'] = &CDExecute;

	// Always.
	//
	handlers['B'] = handlers['b'] = &CDBreakAt;
	handlers['E'] = handlers['e'] = &CDDeleteBreakpoint;
	handlers['C'] = handlers['c'] = &CDClearBreakpoints;
	handlers['5'] = &CDAbout;
	handlers['6'] = &CDQuit;
}

//*********************

static const char* runExtraBreakPointCommands	= "SsIiVvOoRrKkDdUuAaTtLlXxWwHhFfYy";
static const char* runStandardCommands			= "GgMmNn34";
static const char* notRunCommands				= "12";
static const char* alwaysCommands				= "BbEeCc56";

static bool RunExtraPrecond (void)		{ return connected && inBreakPoint; }
static bool RunStandardPrecond (void)	{ return connected; }
static bool NotRunPrecond (void)		{ return !connected; }
static bool TruePrecond (void)			{ return true; }

struct CommandPrecond {
	const char*	commands;
	bool		(*precond)(void);
};

static CommandPrecond preconditions[] = {
	{ runExtraBreakPointCommands,	&RunExtraPrecond	},
	{ runStandardCommands,			&RunStandardPrecond },
	{ notRunCommands,				&NotRunPrecond		},
	{ alwaysCommands,				&TruePrecond		}
};

//////////////////////////////////////////////////////////////

static void ManageInteraction (void) {
	if (ukbhit())
		if (char c = (char) ugetche()) {
			for (util_ui16 i = 0; i < uarraysize(preconditions); ++i)
				if ((*preconditions[i].precond)())
					if (strchr(preconditions[i].commands, c))
						{ (*handlers[c])(); break; }
					else
						puts("\n");
			if (!quit)
				ShowCommands();
		}
}

//////////////////////////////////////////////////////////////

typedef void (*DebuggerResponseHandler)(void);

static void OnInfoStopPoint (void) {

	std::string	src;
	util_ui32	line;
	std::string	cond;

	DeltaDebugClient::GetInfoStopPoint(&src, &line, &inGlobal, &cond);
	if (!cond.empty())
		fprintf(stdout, "\nCondition (%s) is true: ", cond.c_str());
	else
		fprintf(stdout, "\n"); 

	fprintf(stdout, "Stopped at line %d, source %s.\n",
		line,
		src.c_str()
	);

	DeltaDebugClient::ResponseProcessed();
	DeltaDebugClient::WaitAnyMessage();

	if (ReceiveFunction())
		ReceiveVariables();

	if (ustrprefix(DBG_DYNAMIC_SOURCE, src.c_str())) {
		if (sourceFile != src) {
			sourceFile = src;
			DeltaDebugClient::DoGetDynamicCode();
			DeltaDebugClient::WaitAnyMessage();
			DeltaDebugClient::GetInfoDynamicCode(&src);
			LoadSourceFromString(src);
		}
	}
	else
		LoadSource(src.c_str());

	if (!sourceCode.empty())	// We do have source code.
		fprintf(stdout,"\"%s\"\n", sourceCode[line].c_str());
	fflush(stdout);
	inBreakPoint = true;
}

//*****************************

static void OnInfoInvalidBreakPoint (void) {

	std::string	src;
	util_ui32	line, newLine;
	std::string	cond;

	DeltaDebugClient::GetInfoInvalidBreakPoint(&src, &line, &newLine, &cond);
	fprintf(stdout, "\nInvalid break point, src %s, line %d%s.\n",
		src.c_str(),
		line, 
		cond.empty() ? "" : uconstructstr(", cond %s", cond.c_str()).c_str()
	);

	if (line != newLine)
		if (newLine)
			fprintf(stdout, "\tMoved to next valid line %d\n", newLine);
		else
			fprintf(stdout, "\tCould not find next valid line (ingnored).\n");
	else
		fprintf(stdout, "\tCondition error, breakpoint disabled!\n");
	fflush(stdout);
}

//*****************************

static void OnInfoWarning (void) {
	std::string warning;
	DeltaDebugClient::GetInfoWarning(&warning);
	fprintf(stdout, "\nWarning %s!\n", warning.c_str());
	fflush(stdout);
}

//*****************************

static void OnAnyError (bool (*f) (std::string* error)) {
	std::string error;
	(*f)(&error);
	fprintf(stdout, "\n%s\n", error.c_str());
	fflush(stdout);
	ShowCommands();
}

static void OnInfoError (void) 
	{ OnAnyError(&DeltaDebugClient::GetInfoError); }

static void OnInfoBreakPointError (void)
	{ OnAnyError(&DeltaDebugClient::GetInfoBreakPointError); }

//*****************************

static void OnInfoStop (void) {
	fprintf(stdout, "\nExecution finished gracefully.\n");
	fflush(stdout);
	DisconnectWithProgram();
	ShowCommands();
}

//*****************************

static void OnInfoDynamicCode (void) {
	// Simply ignore the async message.
}

//*****************************

static std::map<DebugServerToClientResponse, DebuggerResponseHandler> responseHandlers;

static void InitialiseResponseHandlers (void) {

	responseHandlers[Debug_InfoStopPoint]					= &OnInfoStopPoint;
	responseHandlers[Debug_InfoInvalidBreakPoint]			= &OnInfoInvalidBreakPoint;
	responseHandlers[Debug_InfoBreakPointConditionError]	= &OnInfoInvalidBreakPoint;
	responseHandlers[Debug_InfoValidBreakPoint]				= &OnInfoValidBreakPoint;
	responseHandlers[Debug_InfoWarning]						= &OnInfoWarning;
	responseHandlers[Debug_InfoError]						= &OnInfoError;
	responseHandlers[Debug_InfoBreakPointError]				= &OnInfoBreakPointError;
	responseHandlers[Debug_InfoStop]						= &OnInfoStop;
	responseHandlers[Debug_InfoDynamicCode]					= &OnInfoDynamicCode;
}

static void CallResponseHandler (DebugServerToClientResponse response) {

	std::map<DebugServerToClientResponse, DebuggerResponseHandler>::iterator i;
	i = responseHandlers.find(response);

	if (i != responseHandlers.end())
		(*i->second)();
	else {
		fprintf(stderr,"\nUnexpected message at this point!\n");
		fflush(stdout);
	}
}

//////////////////////////////////////////////////////////////

static void MainLoop (void) {

	InitialiseResponseHandlers();
	ShowCommands();

	while (!quit) {
		
		try {
			ManageInteraction();
			if (!connected || quit)
				continue;

			if (DeltaDebugClient::IsConnectionBroken()) {
				fprintf(stdout, "Program was terminated.\n");
				DisconnectWithProgram();
				ShowCommands();
			}
			else
			if (DeltaDebugClient::IsResponsePending()) {
				DeltaDebugClient::ReceiveResponse();
				CallResponseHandler(DeltaDebugClient::GetResponseType());
				if (!DeltaDebugClient::IsConnectionBroken() && 
					DeltaDebugClient::GetResponseType() != Debug_ResponseInvalid)
					DeltaDebugClient::ResponseProcessed();
			}
		}
		catch (DeltaDebugClient::BrokenConnection) {
			DisconnectWithProgram();
		}
	}
}

//////////////////////////////////////////////////////////////

static void DerrorReport (const char* error) {
	fprintf(stderr, "DD: %s", error);
	fflush(stderr);
}

//////////////////////////////////////////////////////////////

#define	ARG_PORT_NUM			std::string("--port")
#define	ARG_BREAKPOINTS_FILE	std::string("--breakpoints")

static void ReadArguments (int argc, char** argv) {
	for (--argc, ++argv; argc >= 2; argc -= 2, ++argv)
		if (*argv == ARG_PORT_NUM)
			debuggerPort = atoi(*++argv);
		else
		if (*argv == ARG_BREAKPOINTS_FILE)
			breakpointsFile = *++argv;
		else {
			fprintf(stderr, "Unknown argument '%s'; ingored.\n", *argv);
			fflush(stderr);
		}
}

//////////////////////////////////////////////////////////////

int main (int argc, char** argv) {

	dinit(DerrorReport);

	UtilPackage::Initialise();
	VirtualMachinePackage::Initialise();
	ResourceLoader::SingletonCreate();

	fprintf(stdout, "%s", ABOUT);
	fflush(stdout);

	sourceCode.reserve(KLOCS(2));
	DeltaDebugClientBreakPoints::Create();
		
	ReadArguments(argc, argv);
	if (!DeltaDebugClientBreakPoints::Get().LoadText(breakpointsFile.c_str()))
		UERROR_CLEAR();
	else
		fprintf(stdout, "Has loaded successfully breakpoints from '%s'.\n", breakpointsFile.c_str());
	InstalHandlers();

	if (debuggerPort != DEBUGGER_DEFAULT_PORT)
		ConnectWithDebugger(debuggerPort, "localhost");

	MainLoop();

	DeltaDebugClientBreakPoints::Destroy();
	VirtualMachinePackage::CleanUp();
	UtilPackage::CleanUp();
	ResourceLoader::SingletonDestroy();

	dclose();
	return 0;
}

//////////////////////////////////////////////////////////////
