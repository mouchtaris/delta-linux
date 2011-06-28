/**
 *	Profile.cpp
 *
 *	-- DMSL Project --
 *
 *  Implementation of the Profile class.
 *
 *	Giannis Lilis <lilis@ics.forth.gr>
 *	August 2006 (Version 1) - July 2007 (Version 2)
 */

#include "Profile.h"
#include "ProfileScanner.h"

extern int Profile_yyparse(dmsl::Profile *profile, dmsl::PairMap **attributeValueMap);
extern int Profile_yylineno;

namespace dmsl {
	Profile::ProfileList Profile::profiles;

	//***********************************************

	void Profile::NotifyAttributeChanged(const std::string& attribute) {
		for(CallbackList::const_iterator i = callbacks.begin(); i != callbacks.end(); ++i) {
			assert(i->first);
			(*i->first)(attribute, i->second);
		}
	}

	//***********************************************

	bool Profile::ParseFile(const std::string& file) {
		if ((Profile_yyin = fopen(file.c_str(), "r"))) {
			YY_BUFFER_STATE buffer = Profile_yy_create_buffer(Profile_yyin, YY_READ_BUF_SIZE);
			Profile_yy_switch_to_buffer(buffer);
			bool ret = !Profile_yyparse(this, &pairs);
			fclose(Profile_yyin);
			Profile_yy_delete_buffer(buffer);
			Profile_yylineno = 1;	//Reset line counter info for next time
			return ret;
		}
		else {
			pairs = (PairMap *) 0;
			fprintf(stderr, "Cannot open file '%s'\n", file.c_str());
			return false;
		}
	}

	//***********************************************

	bool Profile::ParseString(const std::string &str) {
		YY_BUFFER_STATE buffer = Profile_yy_scan_string(str.c_str());
		Profile_yy_switch_to_buffer(buffer);
		bool ret = !Profile_yyparse(this, &pairs);
		Profile_yy_delete_buffer(buffer);
		Profile_yylineno = 1;	//Reset line counter info for next time
		return ret;
	}

	//***********************************************

	bool Profile::WriteText(FILE *fp) const {
		if(fp && pairs) {
			for(PairMap::const_iterator i = pairs->begin(); i!= pairs->end(); ++i)
				fprintf(fp, "%s\t=\t%s\n", i->first.c_str(), i->second->toString().c_str());
			return true;
		}
		else
			return false;
	}

	bool Profile::WriteText(const std::string& file) const {
		FILE *fp = fopen(file.c_str(), "w");
		bool ret = WriteText(fp);
		fclose(fp);
		return ret;
	}

	//***********************************************

	void Profile::SetAttribute(const std::string& name, ExprValue *value) {
		if(!pairs)
			pairs = new PairMap;
		ExprValue *e = GetAttribute(name);
		if(e)
			delete e;
		(*pairs)[name] = value;
		NotifyAttributeChanged(name);
	}
	
	bool Profile::RemoveAttribute(const std::string& name) {
		PairMap::iterator i;
		if(!pairs || (i = pairs->find(name)) == pairs->end())
			return false;
		else {
			delete i->second;
			pairs->erase(i);
			NotifyAttributeChanged(name);
			return true;
		}
	}

	ExprValue* Profile::GetAttribute(const std::string& name) const {
		if(pairs) {
			PairMap::const_iterator i = pairs->find(name);
			return i == pairs->end() ? (ExprValue *) 0 : i->second;
		}
		else
			return (ExprValue *) 0;
	}

	//***********************************************

	const std::string Profile::GetStringAttribute(const std::string& name) const {
		ExprValue *val = GetAttribute(name);
		return val && val->IsString() ? val->GetString() : "";
	}

	double Profile::GetNumberAttribute(const std::string& name) const {
		ExprValue *val = GetAttribute(name);
		return val && val->IsNumber() ? val->GetNumber() : 0.0;
	}

	bool Profile::GetBoolAttribute(const std::string& name) const {
		ExprValue *val = GetAttribute(name);
		return val && val->IsBool() ? val->GetBool() : false;
	}

	const ExprValueList Profile::GetListAttribute(const std::string& name) const {
		ExprValue *val = GetAttribute(name);
		return val && val->IsList() ? val->GetList() : ExprValueList();
	}

	const Range Profile::GetRangeAttribute(const std::string& name) const {
		ExprValue *val = GetAttribute(name);
		return val && val->IsRange() ? val->GetRange() : Range(0, 0);
	}

	const RangeList Profile::GetRangeListAttribute(const std::string& name) const {
		ExprValue *val = GetAttribute(name);
		return val && val->IsRangeList() ? val->GetRangeList() : RangeList();
	}

	//***********************************************

	bool Profile::GetAttribute(const std::string& name, std::string& str) const {
		ExprValue *val = GetAttribute(name);
		if(val && val->IsString()) {
			str = val->GetString();
			return true;
		}
		else
			return false;
	}

	bool Profile::GetAttribute(const std::string& name, const char*& str) const {
		ExprValue *val = GetAttribute(name);
		if(val && val->IsString()) {
			str = val->GetString().c_str();
			return true;
		}
		else
			return false;
	}

	bool Profile::GetAttribute(const std::string& name, int& num) const {
		ExprValue *val = GetAttribute(name);
		if(val && val->IsNumber()) {
			num = (int) val->GetNumber();
			return true;
		}
		else
			return false;
	}

	bool Profile::GetAttribute(const std::string& name, double& num) const {
		ExprValue *val = GetAttribute(name);
		if(val && val->IsNumber()) {
			num = val->GetNumber();
			return true;
		}
		else
			return false;
	}

	bool Profile::GetAttribute(const std::string& name, bool& b) const {
		ExprValue *val = GetAttribute(name);
		if(val && val->IsBool()) {
			b = val->GetBool();
			return true;
		}
		else
			return false;
	}

	bool Profile::GetAttribute(const std::string& name, ExprValueList& l) const {
		ExprValue *val = GetAttribute(name);
		if(val && val->IsList()) {
			l = val->GetList();
			return true;
		}
		else
			return false;
	}

	bool Profile::GetAttribute(const std::string& name, Range& range) const {
		ExprValue *val = GetAttribute(name);
		if(val && val->IsRange()) {
			range = val->GetRange();
			return true;
		}
		else
			return false;
	}

	bool Profile::GetAttribute(const std::string& name, RangeList& rangeList) const {
		ExprValue *val = GetAttribute(name);
		if(val && val->IsRangeList()) {
			rangeList = val->GetRangeList();
			return true;
		}
		else
			return false;
	}

	//***********************************************

	void Profile::AddAttributes (const Profile *profile) {
		if(profile->pairs)
			for(PairMap::iterator i = profile->pairs->begin(); i != profile->pairs->end(); ++i)
				SetAttribute(i->first, new ExprValue(*i->second));
	}

	//***********************************************

	void Profile::AddAttributeChangeCallback (Callback cb, void *closure) {
		assert(!HasAttributeChangeCallback(cb, closure));
		callbacks.push_back(std::make_pair(cb, closure));
	}

	void Profile::RemoveAttributeChangeCallback (Callback cb, void *closure) {
		assert(HasAttributeChangeCallback(cb, closure));
		callbacks.remove(std::make_pair(cb, closure));
	}

	bool Profile::HasAttributeChangeCallback (Callback cb, void *closure)
		{ return std::find(callbacks.begin(), callbacks.end(), std::make_pair(cb, closure)) != callbacks.end(); }

	//***********************************************

	Profile::~Profile() {
		profiles.remove(this);
		callbacks.clear();
		if(pairs) {
			std::for_each(
				pairs->begin(), 
				pairs->end(),
				util::apply2nd<PairMap::value_type>(util::deleter<ExprValue *>())
			);
			pairs->clear();
			delete pairs;
		}
	}
}
