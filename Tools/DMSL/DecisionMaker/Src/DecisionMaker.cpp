/**
 *	DecisionMaker.cpp
 *
 *	-- DMSL Project --
 *
 *	Implementation of the DecisionMaker class.
 *
 *  Version 2 update: Implementation of NotiFyProfileLoaded,
 *  NotifyAttributeChanged and NotifyActivationsChanged functions.
 *
 *  Version 3 update: Implementation of AppendRules function.
 *
 *	Giannis Lilis <lilis@ics.forth.gr>
 *	August 2006 (Version 1) - July 2007 (Version 2) - January 2008 (Version 3)
 */

#include "DecisionMaker.h"
#include "Statements.h"
#include "DecisionMakerScanner.h"
#include "util.h"

extern int DMSL_yyparse(dmsl::DecisionMaker *dm, dmsl::Statement **logic);
extern int DMSL_yylineno;

namespace dmsl {
	Profile *DecisionMaker::configuration;

	std::string DecisionMaker::globalError;

	//***********************************************

	#define FAIL_ON_NO_CONFIGURATION_WITH_ERROR_FUNC(func)																	\
		do {																												\
			if(!configuration) {																							\
				func("Invalid configuration. Expected initialization of DecisionMaker with a valid configuration file.");	\
				return false;																								\
			}																												\
		}																													\
		while(false)

	#define FAIL_ON_NO_CONFIGURATION		FAIL_ON_NO_CONFIGURATION_WITH_ERROR_FUNC(SetError)
	#define FAIL_ON_NO_CONFIGURATION_STATIC	FAIL_ON_NO_CONFIGURATION_WITH_ERROR_FUNC(SetGlobalError)

	//***********************************************

	ComponentStatement * DecisionMaker::CastToComponent(Statement *stmt) {
		assert(stmt->GetType() == Statement::StmtTypeComponent);
		return static_cast<ComponentStatement *>(stmt);
	}

	ComponentStatement * DecisionMaker::GetComponent(const std::string& component) const {
		const SymbolTable::StatementMap& components = table.GetComponents();
		for(SymbolTable::StatementMap::const_iterator i = components.begin(); i != components.end(); ++i) {
			ComponentStatement *comp = CastToComponent(i->second);
			if(comp->GetName() == component)
				return comp;
		}
		return (ComponentStatement *) 0;
	}

	//***********************************************
	
	void DecisionMaker::NotifyProfileLoaded(void) {
		SymbolTable::StatementMap& components = table.GetComponents();
		for(SymbolTable::StatementMap::iterator i = components.begin(); i != components.end(); ++i) {
			ComponentStatement *comp = CastToComponent(i->second);
			comp->SetAllProfileDependencies(true);
			comp->SetNeverEvaluated(true);
		}
	}

	void DecisionMaker::NotifyAttributeChanged(const std::string& attribute) {
		SymbolTable::StatementMap& components = table.GetComponents();
		for(SymbolTable::StatementMap::iterator i = components.begin(); i != components.end(); ++i) {
			ComponentStatement *comp = CastToComponent(i->second);
			if(comp->DependsOnProfile(attribute))
				comp->SetProfileDependency(attribute, true);
		}
	}

	void DecisionMaker::NotifyActivationsChanged(void) {
		SymbolTable::StatementMap& components = table.GetComponents();
		for(StringVector::iterator iter = activationsChanged.begin(); iter != activationsChanged.end(); ++iter)
			for(SymbolTable::StatementMap::iterator i = components.begin(); i != components.end(); ++i) {
				ComponentStatement *comp = CastToComponent(i->second);
				if(comp->DependsOnActivation(*iter))
					comp->SetActivationDependency(*iter, true);
			}
		activationsChanged.clear();
	}

	void DecisionMaker::NotifyAllProfileAttributesChanged(const Profile *profile) {
		const PairMap *pairs = profile->GetPairs();
		if(pairs)
			for(PairMap::const_iterator i = pairs->begin(); i != pairs->end(); ++i)
				NotifyAttributeChanged(i->first);
	}

	void DecisionMaker::OnProfileAttributeChange(const std::string& attribute, void *instance)
		{ ((DecisionMaker *) instance)->NotifyAttributeChanged(attribute); }

	//***********************************************

	bool DecisionMaker::Compile(const std::string& file) {
		FAIL_ON_NO_CONFIGURATION;
		error = "";
		warnings.clear();
		if(logic)
			delete logic, logic = (LogicStatement *) 0;
		table.Clear();
		//keep activations and decisions

		bool status = false;
		if((DMSL_yyin = fopen(file.c_str(), "r"))) {
			YY_BUFFER_STATE buffer = DMSL_yy_create_buffer(DMSL_yyin, YY_READ_BUF_SIZE);
			DMSL_yy_switch_to_buffer(buffer);
			Statement *s = (Statement *) 0;
			status = !DMSL_yyparse(this, &s);
			DMSL_yy_delete_buffer(buffer);
			DMSL_yylineno = 1;	//Reset line counter info for next time
			if(!status || !s) {	//error set accordingly by DMSL_yyparse
				Expression::SetStaticDestruction(this, true);
				Expression::Cleanup(this);
				Statement::Cleanup(this);
				logic = (LogicStatement *) 0;
				Expression::SetStaticDestruction(this, false);
			}
			else {
				assert(s->GetType() == Statement::StmtTypeLogic);
				logic = static_cast<LogicStatement*>(s);
				logic->CreateDependencies(this);
				if(!error.empty())
					delete logic, logic = (LogicStatement *) 0, status = false;
			}
		}
		else
			SET_ERROR_WITH_ONE_ARG(this, "CompilationNoFile", file.c_str());
		return status;
	}

	//***********************************************

	bool DecisionMaker::AppendRules(const std::string& file) {
		FAIL_ON_NO_CONFIGURATION;
		if(logic) {
			DecisionMaker *dm = new DecisionMaker;
			bool status = dm->Compile(file);
			if(status) {
				if(status = table.CheckForRedeclarations(dm->table, &error)) {
					std::list<std::string> overwrites;
					logic->AddLogic(dm->logic, overwrites);
					for(std::list<std::string>::const_iterator i = overwrites.begin(); i != overwrites.end(); ++i)
						APPEND_WARNING_WITH_ONE_ARG(this, "AppendRulesRedeclaration", i->c_str());
				}
				else
					SET_ERROR_WITH_ONE_ARG(this, "AppendRulesRedeclarationTypeMismatch", error.c_str());	//error here will just contain the redeclared name
			}
			else
				SetError(dm->GetError());
			delete dm;
			return status;
		}
		else
			return Compile(file);
	}

	//***********************************************

	bool DecisionMaker::LoadProfile(const std::string& profileFile) {
		FAIL_ON_NO_CONFIGURATION;
		Profile *profile = (Profile *) NewProfile(profileFile);
		if(profile) {
			profiles.clear();
			util::destroyContainer(ownedProfiles);

			profile->AddAttributeChangeCallback(OnProfileAttributeChange, this);
			profiles.push_back(profile);
			ownedProfiles.push_back(profile);
			NotifyProfileLoaded();	//upon loading a totally new profile all previous evaluation data are reset
		}
		else {
			SetError(GetGlobalError());
			ClearGlobalError();
		}
		return !!profile;
	}

	//***********************************************

	bool DecisionMaker::AppendProfile(const std::string& profileFile) {
		FAIL_ON_NO_CONFIGURATION;
		Profile *profile = (Profile *) NewProfile(profileFile);
		if(profile) {
			if(profiles.empty())
				NotifyProfileLoaded();
			profile->AddAttributeChangeCallback(OnProfileAttributeChange, this);
			profiles.push_back(profile);
			ownedProfiles.push_back(profile);
			NotifyAllProfileAttributesChanged(profile);
		}
		else {
			SetError(GetGlobalError());
			ClearGlobalError();
		}
		return !!profile;
	}

	//***********************************************

	bool DecisionMaker::Evaluate(const std::string& component, bool force) {
		FAIL_ON_NO_CONFIGURATION;
		bool status = false;
		ActivationMap active_backup = active;
		decisions.clear();	// decisions should always be cleared at a new evaluation
		if(logic) {
			if(!profiles.empty()) {
				forcingEvaluation = force;
				logic->SetEvaluationTarget(component);
				if(logic->Evaluate(this)) {	//evaluation ok
					status = true;
					NotifyActivationsChanged();
				}
				else {								//error set accordingly by Evaluate so no need to set it here
					active = active_backup;			//restore previous active map
					activationsChanged.clear();
				}
				logic->SetEvaluationTarget();
				forcingEvaluation = false;
			}
			else
				SET_ERROR_WITH_ZERO_ARGS(this, "EvaluationWithoutProfile");
		}
		else
			SET_ERROR_WITH_ZERO_ARGS(this, "EvaluationWithoutCompilation");
		return status;
	}

	//***********************************************

	bool DecisionMaker::SetAttribute(const std::string& attribute, const std::string& value) {
		FAIL_ON_NO_CONFIGURATION;
		Profile *profile = new Profile;
		bool status = profile->ParseString(attribute + "=" + value);
		if(!status) {
			SetError(profile->GetError());
			delete profile;
		}
		else {
			if(profiles.empty())
				NotifyProfileLoaded();
			profile->AddAttributeChangeCallback(OnProfileAttributeChange, this);
			profiles.push_back(profile);
			ownedProfiles.push_back(profile);
			NotifyAttributeChanged(attribute);	//only one attribute so call this instead of NotifyAllProfileAttributesChanged
		}
		return status;
	}

	//***********************************************

	bool DecisionMaker::HasAttribute(const std::string& attribute) const {
		for(ProfileList::const_iterator i = profiles.begin(); i != profiles.end(); ++i)
			if((*i)->GetAttribute(attribute))
				return true;
		return false;
	}

	//***********************************************

	DecisionMaker::ProfileOpaqueType DecisionMaker::NewProfile(const std::string& profileFile) {
		Profile *profile = new Profile;
		if(!profileFile.empty() && !profile->ParseFile(profileFile)) {
			SetGlobalError(profile->GetError());
			delete profile, profile = (Profile *) 0;
		}
		return (ProfileOpaqueType) profile;
	}

	void DecisionMaker::DeleteProfile(ProfileOpaqueType id) {
		Profile *profile = (Profile *) id;
		assert(Profile::IsValidInstance(profile));
		delete profile;
	}

	bool DecisionMaker::AppendToProfile(ProfileOpaqueType dest, const ProfileOpaqueType source) {
		FAIL_ON_NO_CONFIGURATION_STATIC;
		Profile *destProfile = (Profile *) dest;
		const Profile *sourceProfile = (const Profile *) source;
		if(Profile::IsValidInstance(destProfile) && Profile::IsValidInstance(sourceProfile)) {
			//all DM instances sharing the destProfile are notified for the changes
			destProfile->AddAttributes(sourceProfile);
			return true;
		}
		else {
			SetGlobalError(configuration->GetStringAttribute("InvalidProfileInstance"));
			return false;
		}
	}

	bool DecisionMaker::SetProfileAttribute(ProfileOpaqueType id, const std::string& attribute, const std::string& value) {
		FAIL_ON_NO_CONFIGURATION_STATIC;
		Profile *profile = (Profile *) id;
		if(Profile::IsValidInstance(profile)) {
			Profile *p = new Profile;
			bool status = p->ParseString(attribute + "=" + value);
			if(!status)
				SetGlobalError(p->GetError());
			else
				profile->AddAttributes(p);	//all DM instances sharing the profile are notified for the changes
			delete p;
			return status;
		}
		else {
			SetGlobalError(configuration->GetStringAttribute("InvalidProfileInstance"));
			return false;
		}
		return true;
	}

	bool DecisionMaker::RemoveProfileAttribute(ProfileOpaqueType id, const std::string& attribute) {
		FAIL_ON_NO_CONFIGURATION_STATIC;
		Profile *profile = (Profile *) id;
		if(Profile::IsValidInstance(profile))
			return profile->RemoveAttribute(attribute);	//all DM instances sharing the profile are notified for the changes
		else {
			SetGlobalError(configuration->GetStringAttribute("InvalidProfileInstance"));
			return false;
		}
	}

	bool DecisionMaker::AddProfile(ProfileOpaqueType id) {
		FAIL_ON_NO_CONFIGURATION;
		Profile *profile = (Profile *) id;
		if(Profile::IsValidInstance(profile)) {
			if(std::find(profiles.begin(), profiles.end(), profile) == profiles.end()) {
				profile->AddAttributeChangeCallback(OnProfileAttributeChange, this);
				profiles.push_back(profile);
				NotifyAllProfileAttributesChanged(profile);
				return true;
			}
			else {
				SET_ERROR_WITH_ZERO_ARGS(this, "DuplicateProfileAddition");
				return false;
			}
		}
		else {
			SET_ERROR_WITH_ZERO_ARGS(this, "InvalidProfileInstance");
			return false;
		}
	}

	bool DecisionMaker::RemoveProfile(ProfileOpaqueType id) {
		FAIL_ON_NO_CONFIGURATION;
		Profile *profile = (Profile *) id;
		if(Profile::IsValidInstance(profile)) {
			if(std::find(profiles.begin(), profiles.end(), profile) != profiles.end()) {
				profile->RemoveAttributeChangeCallback(OnProfileAttributeChange, this);
				profiles.remove(profile);
				NotifyAllProfileAttributesChanged(profile);
				return true;
			}
			else {
				SET_ERROR_WITH_ZERO_ARGS(this, "InvalidProfileRemoval");
				return false;
			}
		}
		else {
			SET_ERROR_WITH_ZERO_ARGS(this, "InvalidProfileInstance");
			return false;
		}
	}

	//***********************************************

	bool DecisionMaker::WriteText(FILE *fp) {
		FAIL_ON_NO_CONFIGURATION;
		if(fp) {
			if(logic) { 
				logic->WriteText(fp, 0);
				return true;
			}
			else {
				SET_ERROR_WITH_ZERO_ARGS(this, "FileOutputWithoutCompilation");
				return false;
			}
		}
		else {
			SET_ERROR_WITH_ZERO_ARGS(this, "FileOutputNoFile");
			return false;
		}
	}

	bool DecisionMaker::WriteText(const std::string& file) {
		FILE *fp = fopen(file.c_str(), "w");
		bool ret = WriteText(fp);
		fclose(fp);
		return ret;
	}

	//***********************************************

	const DecisionMaker::StringVector DecisionMaker::GetStereotypes(void) const {
		StringVector ret;
		const SymbolTable::ExpressionMap& stereotypes = table.GetStereotypes();
		for(SymbolTable::ExpressionMap::const_iterator i = stereotypes.begin(); i != stereotypes.end(); ++i)
			ret.push_back(i->first);
		return ret;
	}

	const DecisionMaker::StringVector DecisionMaker::GetComponents(void) const {
		StringVector ret;
		const SymbolTable::StatementMap& components = table.GetComponents();
		for(SymbolTable::StatementMap::const_iterator i = components.begin(); i != components.end(); ++i)
			ret.push_back(i->first);
		return ret;
	}

	const std::string DecisionMaker::GetStereotypeDefinition(const std::string& id) const {
		Expression *stereotype = table.GetStereotype(id);
		return stereotype ? stereotype->ConvertToString() : "";
	}

	const std::string DecisionMaker::GetComponentDefinition(const std::string& id) const {
		Statement *component = table.GetComponent(id);
		return component ? component->ConvertToString(0) : "";
	}

	//***********************************************

	const std::string DecisionMaker::GetSingleActivationDecision(void) {
		if(decisions.size() == 1) {
			const std::string activate = "activate";
			if(decisions[0].substr(0, activate.size()) == activate)
				return decisions[0].substr(activate.size() + 1);
			else {
				SET_ERROR_WITH_ZERO_ARGS(this, "SingleActivationDecisionCancel");
				return "";
			}
		}
		else {
			if(!decisions.empty() && configuration)
				SET_ERROR_WITH_ONE_ARG(this, "SingleActivationDecisionMultiples", decisions.size());
			return "";
		}
	}

	//***********************************************

	bool DecisionMaker::ComponentDependsOnProfileAttribute(const std::string& component, const std::string& attribute) {
		if(ComponentStatement *comp = GetComponent(component))
			return comp->DependsOnProfile(attribute);
		else {
			SET_ERROR_WITH_TWO_ARGS(this, "DependencyCheckNoComponent", "attribute", component.c_str());
			return false;
		}
	}

	bool DecisionMaker::ComponentDependsOnActivation(const std::string& component, const std::string& activation) {
		if(ComponentStatement *comp = GetComponent(component))
			return comp->DependsOnActivation(activation);
		else {
			SET_ERROR_WITH_TWO_ARGS(this, "DependencyCheckNoComponent", "activation", component.c_str());
			return false;
		}
	}

	//***********************************************

	bool DecisionMaker::RegisterLibraryFunction(const std::string& name, LibraryFunction func) {
		FunctionMap::iterator i = functions.find(name);
		if(i == functions.end()) {
			functions[name] = func;
			return true;
		}
		else
			return false;
	}

	bool DecisionMaker::CancelLibraryFunction(const std::string& name) {
		FunctionMap::iterator i = functions.find(name);
		if(i != functions.end()) {
			functions.erase(i);
			return true;
		}
		else
			return false;
	}

	DecisionMaker::LibraryFunction DecisionMaker::GetLibraryFunction(const std::string& name) {
		FunctionMap::const_iterator i = functions.find(name);
		return i == functions.end() ? (LibraryFunction) 0 : i->second;
	}

	//***********************************************

	bool DecisionMaker::Initialize(const std::string& configFile) {
		srand((unsigned) util::time(0));
		configuration = new Profile;
		bool status = configuration->ParseFile(configFile);
		if(!status)
			delete configuration, configuration = (Profile *) 0;
		return status;
	}

	//***********************************************

	DecisionMaker::~DecisionMaker() {
		if(logic)
			delete logic, logic = (LogicStatement *) 0;
		Expression::RemoveDecisionMaker(this);
		Statement::RemoveDecisionMaker(this);
		util::destroyContainer(ownedProfiles);
		profiles.clear();
		active.clear();
		warnings.clear();
		decisions.clear();
		functions.clear();
	}
}
