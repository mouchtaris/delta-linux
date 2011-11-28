/* File : dmsl.i */
%module DMSL

/* Make use of the string, list and map libraries (used in DecisionMaker)*/
%include <std_string.i>
%include <std_vector.i>
%include <std_map.i>

%{
#include "DecisionMaker.h"
%}

namespace std {
   %template(StringVector) vector<std::string>;
};

#include <vector>
#include <string>

/////////////////////////////////////////////////////////////////////////
// The Main Component of this project, the DecisionMaker class. 
// Compiles a given DMSL file and evaluates it according to the
// profile file, returning the Decisions made and any errors and
// warnings that occured in the process.
//

namespace dmsl {
	class DecisionMaker {
	public:
		typedef unsigned long long ProfileOpaqueType;

		//***********************************************

		bool Compile		(const std::string& file);
		bool AppendRules	(const std::string& file);
		bool LoadProfile	(const std::string& profileFile);
		bool AppendProfile	(const std::string& profileFile);
		bool Evaluate		(const std::string& component, bool force = false);
		
		bool SetAttribute	(const std::string& attribute, const std::string& value);
		bool HasAttribute	(const std::string& attribute) const;

		//***********************************************

		static ProfileOpaqueType	NewProfile				(const std::string& profileFile = "");
		static void					DeleteProfile			(ProfileOpaqueType id);
		static bool					AppendToProfile			(ProfileOpaqueType dest, const ProfileOpaqueType source);
		static bool					SetProfileAttribute		(ProfileOpaqueType id, const std::string& attribute, const std::string& value);
		static bool					RemoveProfileAttribute	(ProfileOpaqueType id, const std::string& attribute);
		
		bool AddProfile	(ProfileOpaqueType id);
		bool RemoveProfile	(ProfileOpaqueType id);

		//***********************************************

		bool WriteText(const std::string& file);

		//***********************************************

		const std::vector<std::string> GetStereotypes(void) const;
		const std::vector<std::string> GetComponents (void) const;

		const std::string GetStereotypeDefinition	(const std::string& id) const;
		const std::string GetComponentDefinition	(const std::string& id) const;

		//***********************************************

		const std::vector<std::string>& GetDecisions				(void) const;
		const std::string				GetSingleActivationDecision	(void);

		//***********************************************

		bool ComponentDependsOnProfileAttribute	(const std::string& component, const std::string& attribute);
		bool ComponentDependsOnActivation		(const std::string& component, const std::string& activation);
		
		//***********************************************

		const std::vector<std::string>& GetWarnings(void) const;
		void ClearWarnings(void);
		
		const std::string& GetError(void) const;
		void ClearError(void);
		
		static const std::string& GetGlobalError(void);
		static void ClearGlobalError(void);

		//***********************************************
		
		void ClearActivations(void);
		bool IsActiveComponent(const std::string& name) const;

		//////////////////////////////////////////////////////////////////
		// Static member functions used for DMSL configuration messages
		//
		static bool Initialize	(const std::string& configFile);
		static void Cleanup		(void);
	};
}