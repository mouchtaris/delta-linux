/**
 *	DecisionMaker.h
 *
 *	-- DMSL Project --
 *
 *	The Main Component of this project, the DecisionMaker class. 
 *  Compiles a given DMSL file and evaluates it according to the
 *  profile file, returning the Decisions made and any errors and
 *  warnings that occured in the process.
 *
 *  Version 2 update: Added support to avoid evaluating non-changed
 *  components. Evaluate now takes a 'force evaluation' flag to 
 *  indicate full reevaluation.
 *
 *  Version 3 update: Added support for incremental rule loading.
 *
 *	Giannis Lilis <lilis@ics.forth.gr>
 *	August 2006 (Version 1) - July 2007 (Version 2) - January 2008 (Version 3)
 */

#ifndef DECISION_MAKER_H
#define DECISION_MAKER_H

#include <vector>
#include <map>
#include <string>

#include <assert.h>
#include <stdio.h>

#include "Profile.h"
#include "SymbolTable.h"

#if defined(DM_EXPORT)
#	define DM_DLL_API __declspec(dllexport)
#elif defined(DM_IMPORT)
#	define DM_DLL_API __declspec(dllimport)
#else
#	define DM_DLL_API
#endif

namespace dmsl {
	///////////////////////////
	// Forward Declarations
	//
	class LogicStatement;
	class ComponentStatement;
	
	//////////////////////////////////////////////////
	// DecisionMaker class
	//
	class DecisionMaker {
	public:
		typedef std::vector<std::string>	StringVector;
		typedef std::map<std::string, bool> ActivationMap;
		typedef std::list<Profile *> ProfileList;
		typedef unsigned long long ProfileOpaqueType;

		typedef void (*LibraryFunction)(const std::string&);
		typedef bool (*PlausibilityFunction)(const std::string&, const std::string&);

	private:
		typedef std::map<std::string, LibraryFunction> FunctionMap;

		bool forcingEvaluation;
		std::string error;

		StringVector warnings;
		StringVector decisions;
		ActivationMap active;

		SymbolTable table;

		ProfileList ownedProfiles;
		ProfileList profiles;
		LogicStatement *logic;

		StringVector activationsChanged;

		FunctionMap functions;
		PlausibilityFunction plausibilityFunc;

		static Profile *configuration;
		static std::string globalError;

		static ComponentStatement * CastToComponent(Statement *stmt);
		ComponentStatement * GetComponent(const std::string& component) const;

		void NotifyProfileLoaded				(void);
		void NotifyAttributeChanged				(const std::string& attribute);
		void NotifyActivationsChanged			(void);
		void NotifyAllProfileAttributesChanged	(const Profile *profile);

		static void OnProfileAttributeChange	(const std::string& attribute, void *instance);
	public:

		//***********************************************

		DM_DLL_API bool Compile			(const std::string& file);
		DM_DLL_API bool AppendRules		(const std::string& file);
		DM_DLL_API bool LoadProfile		(const std::string& profileFile);
		DM_DLL_API bool AppendProfile	(const std::string& profileFile);
		DM_DLL_API bool Evaluate		(const std::string& component, bool force = false);

		DM_DLL_API bool SetAttribute	(const std::string& attribute, const std::string& value);

		//***********************************************

		DM_DLL_API static ProfileOpaqueType	NewProfile				(const std::string& profileFile = "");
		DM_DLL_API static void				DeleteProfile			(ProfileOpaqueType id);
		DM_DLL_API static bool				AppendToProfile			(ProfileOpaqueType dest, const ProfileOpaqueType source);
		DM_DLL_API static bool				SetProfileAttribute		(ProfileOpaqueType id, const std::string& attribute, const std::string& value);
		DM_DLL_API static bool				RemoveProfileAttribute	(ProfileOpaqueType id, const std::string& attribute);

		DM_DLL_API bool AddProfile		(ProfileOpaqueType id);
		DM_DLL_API bool RemoveProfile	(ProfileOpaqueType id);

		//***********************************************

		// The output functions are non-const as they can produce errors
		DM_DLL_API bool WriteText(FILE *fp);
		DM_DLL_API bool WriteText(const std::string& file);

		//***********************************************

		DM_DLL_API const StringVector	GetStereotypes(void) const;
		DM_DLL_API const StringVector	GetComponents (void) const;

		DM_DLL_API const std::string	GetStereotypeDefinition	(const std::string& id) const;
		DM_DLL_API const std::string	GetComponentDefinition	(const std::string& id) const;

		//***********************************************

		DM_DLL_API const StringVector&	GetDecisions				(void) const { return decisions; }
		DM_DLL_API const std::string	GetSingleActivationDecision	(void);	//non const as it may cause an error

		//***********************************************

		//non const as they may cause errors
		DM_DLL_API bool	ComponentDependsOnProfileAttribute	(const std::string& component, const std::string& attribute);
		DM_DLL_API bool	ComponentDependsOnActivation		(const std::string& component, const std::string& activation);

		//***********************************************

		DM_DLL_API const StringVector& GetWarnings(void) const { return warnings; }
		void AppendWarning(const std::string& warning) { warnings.push_back(warning); }
		DM_DLL_API void ClearWarnings(void) { warnings.clear(); }

		DM_DLL_API const std::string& GetError(void) const { return error; }
		void SetError(const std::string& error) { this->error = error; }
		DM_DLL_API void ClearError(void) { error = ""; }

		DM_DLL_API static const std::string& GetGlobalError(void) { return globalError; }
		static void SetGlobalError(const std::string& error) { globalError = error; }
		static void ClearGlobalError(void) { globalError = ""; }

		//***********************************************
		
		DM_DLL_API void ClearActivations(void) { active.clear(); }

		DM_DLL_API bool IsActiveComponent(const std::string& name) const {
			ActivationMap::const_iterator i = active.find(name);
			return i == active.end() ? false : i->second;
		}

		void SetActiveComponent(const std::string& name, bool val) {
			active[name] = val;
			decisions.push_back(std::string(val ? "activate" : "cancel") + " " + name);
			activationsChanged.push_back(name);
		}

		//***********************************************

		DM_DLL_API bool				RegisterLibraryFunction	(const std::string& name, LibraryFunction func);
		DM_DLL_API bool				CancelLibraryFunction	(const std::string& name);
		DM_DLL_API LibraryFunction	GetLibraryFunction		(const std::string& name);

		//***********************************************

		DM_DLL_API PlausibilityFunction	GetPlausibilityFunction	(void)						{ return plausibilityFunc;	}
		DM_DLL_API void					SetPlausibilityFunction	(PlausibilityFunction func)	{ plausibilityFunc = func;	}

		//***********************************************

		SymbolTable& GetSymbolTable(void) { return table; }

		bool ForcingEvaluation(void) const { return forcingEvaluation; }

		//***********************************************

		const ProfileList& GetProfileList(void) const { return profiles; }

		//////////////////////////////////////////////////////////////////
		// Static member functions
		//
		DM_DLL_API static bool Initialize	(const std::string& configFile);
		DM_DLL_API static void Cleanup		(void) { if(configuration) delete configuration, configuration = (Profile *) 0; }
		
		static Profile& GetConfiguration(void) { assert(configuration); return *configuration; }

		//***********************************************

		DM_DLL_API DecisionMaker(void) : forcingEvaluation(false), error(""), logic((LogicStatement *) 0), plausibilityFunc(0) {}
		DM_DLL_API ~DecisionMaker();
	};
}

#endif
