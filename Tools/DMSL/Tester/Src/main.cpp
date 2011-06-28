/**
 *	main.cpp
 *
 *	-- DMSL Project --
 *
 *  A test program using the DecisionMaker class.
 *
 *	Giannis Lilis <lilis@ics.forth.gr>
 *	August 2006 (Version 1) - July 2007 (Version 2)
 */
#if defined(WIN32)
#include <vld.h>
#endif
#include "DecisionMaker.h"
#include <iostream>
#include <iterator>

#define TEST 6

#if TEST == 1
#	define COMPONENT	"ErrorReporting"
#	define TEST_FILE	"../testfiles/example1.txt"
#elif TEST == 2
#	define COMPONENT	"ScreenCalibration"
#	define TEST_FILE	"../testfiles/example2.txt"
#elif TEST == 3
#	define COMPONENT	"Toolbar"
#	define TEST_FILE	"../testfiles/example3.txt"
#elif TEST == 4
#	define COMPONENT	"MemoryManager"
#	define TEST_FILE	"../testfiles/example4.txt"
#elif TEST == 5
#	define COMPONENT	"Shell"
#	define TEST_FILE	"../testfiles/deltaide.txt"
#else
#	define COMPONENT	"main"
#	define TEST_FILE	"../testfiles/test1.txt"
#endif

#define PARAMS_FILE	"../testfiles/params_examples.txt"
#define OUTPUT_FILE	"../testfiles/output.txt"
using namespace dmsl;

void libfunc_test(const std::string& str) { printf("Libfunc(%s) called\n", str.c_str()); }

void test_dependencies(DecisionMaker& dm) {
	const std::string comp1 = "case_main", attribute = "user.web expertise";
	if(dm.ComponentDependsOnProfileAttribute(comp1, attribute))
		std::cout << comp1 << " depends on attribute '" << attribute << "'" << std::endl;
	else if(!dm.GetError().empty()) {
		std::cout << dm.GetError() << std::endl;
		dm.ClearError();
	}

	const std::string comp2 = "main1", activation = "comp2";
	if(dm.ComponentDependsOnActivation(comp2, activation))
		std::cout << comp2 << " depends on '" << activation << "' activation" << std::endl;
	else if(!dm.GetError().empty()) {
		std::cout << dm.GetError() << std::endl;
		dm.ClearError();
	}
}

bool evaluate(DecisionMaker& dm, const std::string& component) {
	if(!dm.Evaluate(COMPONENT)) {
		std::cout << dm.GetError() << std::endl;
		return false;
	}
	std::cout << "Evaluation done..." << std::endl;
	DecisionMaker::StringVector list;
	//std::cout << "Stereotypes:" << std::endl;
	//list = dm.GetStereotypes();
	//for(DecisionMaker::StringVector::const_iterator i = list.begin(); i != list.end(); ++i)
	//	std::cout << "\t" << *i <<" with definition : " << dm.GetStereotypeDefinition(*i) << std::endl;
	//
	//list = dm.GetComponents();
	//std::cout << "Components:" << std::endl << "\t";
	//std::copy(list.begin(), list.end(), std::ostream_iterator<std::string>(std::cout, "\n\t"));
	
	list = dm.GetDecisions();
	std::cout << "Decisions:" << std::endl << "\t";
	std::copy(list.begin(), list.end(), std::ostream_iterator<std::string>(std::cout, "\n\t"));

	std::cout << "Total decisions: " << list.size() << std::endl;
	std::string str = dm.GetSingleActivationDecision();
	if(str != "")
		std::cout << "Single Decision:" << str << std::endl;
	else
		std::cout << dm.GetError() << std::endl;
	return true;
}

int main(int argc, char **argv) {
	const char *file = TEST_FILE;
	const char *configurationFile = "../conf.ini";

	DecisionMaker::Initialize(configurationFile);

	DecisionMaker::ProfileOpaqueType p1 = DecisionMaker::NewProfile(), p2 = DecisionMaker::NewProfile();
	DecisionMaker::SetProfileAttribute(p1, "abilities.pointing", "accurate");

	do {
		DecisionMaker dm;
		dm.RegisterLibraryFunction("test", libfunc_test);
		if(!dm.Compile(file)) {
			std::cout << dm.GetError() << std::endl;
			break;
		}
		std::cout << "Compilation done..." << std::endl;

		dm.WriteText(OUTPUT_FILE);

		const DecisionMaker::StringVector& list = dm.GetWarnings();
		std::copy(list.begin(), list.end(), std::ostream_iterator<std::string>(std::cout, "\n"));

		test_dependencies(dm);
		std::cout << dm.GetComponentDefinition("main") << std::endl;

		if(!dm.LoadProfile(PARAMS_FILE)) {
			std::cout << dm.GetError() << std::endl;
			break;
		}
		std::cout << "Profile loading done..." << std::endl;
		//dm.SetAttribute("context.installation", "kiosk");
		//dm.SetAttribute("user.\"web expertise\"", "expert");
		//dm.SetAttribute("user.\"visual ability\"", "sighted");

		if(!evaluate(dm, COMPONENT))
			break;
		
		//second evaluation
		dm.SetAttribute("abilities.pointing", "foo");

		if(!evaluate(dm, COMPONENT))
			break;

		//third evaluation
		if(!dm.AddProfile(p1))
			std::cout << dm.GetError() << std::endl;

		//dm.RemoveProfile(p1);
		if(!evaluate(dm, COMPONENT))
			break;

	} while(false);

	DecisionMaker::Cleanup();
	return 0;
}
