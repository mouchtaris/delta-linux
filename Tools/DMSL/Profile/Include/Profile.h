/**
 *	Profile.h
 *
 *	-- DMSL Project --
 *
 *	The profile used for the evaluation of the compiled rules.
 *  Contains information for attribute value pairs.
 *
 *	Giannis Lilis <lilis@ics.forth.gr>
 *	August 2006 (Version 1) - July 2007 (Version 2)
 */

#ifndef PROFILE_H
#define PROFILE_H

#include <map>
#include <list>
#include <string>
#include <assert.h>

#include "ExprValue.h"

namespace dmsl {
	typedef std::pair<const std::string, ExprValue *> Pair;
	typedef std::map <const std::string, ExprValue *> PairMap;

	//////////////////////////////////////////////////
	// Profile class
	//
	class Profile {
	public:
		typedef void (*Callback)(const std::string&, void *);
	private:
		PairMap *pairs;
		std::string error;

		typedef std::list<Profile *> ProfileList;
		static ProfileList profiles;

		typedef std::pair<Callback, void *> CallbackData;
		typedef std::list<CallbackData>		CallbackList;
		CallbackList callbacks;

		void NotifyAttributeChanged(const std::string& attribute);
	public:

		PROFILE_DLL_API bool ParseFile	(const std::string& file);
		PROFILE_DLL_API bool ParseString(const std::string& str);

		//***********************************************

		PROFILE_DLL_API bool WriteText(FILE *fp) const;
		PROFILE_DLL_API bool WriteText(const std::string& file) const;

		////////////////////////////////////////////////
		// Attribute handling members
		//

		PROFILE_DLL_API void		SetAttribute	(const std::string& name, ExprValue *value);
		PROFILE_DLL_API bool		RemoveAttribute	(const std::string& name);
		PROFILE_DLL_API ExprValue*	GetAttribute	(const std::string& name) const;

		///////////////////////////////////////////////////////////////////
		// Specific value type "Get" functions for convenience
		// Return default values uppon non existance or type error
		//

		PROFILE_DLL_API const std::string	GetStringAttribute		(const std::string& name) const;
		PROFILE_DLL_API double				GetNumberAttribute		(const std::string& name) const;
		PROFILE_DLL_API bool				GetBoolAttribute		(const std::string& name) const;
		PROFILE_DLL_API const ExprValueList	GetListAttribute		(const std::string& name) const;
		PROFILE_DLL_API const Range			GetRangeAttribute		(const std::string& name) const;
		PROFILE_DLL_API const RangeList		GetRangeListAttribute	(const std::string& name) const;

		///////////////////////////////////////////////////////////////////
		// Specific value type "Get" functions for convenience
		// Checks to be performed by caller, return success status
		// and set the correct value via reference if able.
		// Use strong typing as well.
		//

		PROFILE_DLL_API bool GetAttribute(const std::string& name, std::string& str)	const;
		PROFILE_DLL_API bool GetAttribute(const std::string& name, const char*& str)	const;
		PROFILE_DLL_API bool GetAttribute(const std::string& name, int& num)			const;
		PROFILE_DLL_API bool GetAttribute(const std::string& name, double& num)			const;
		PROFILE_DLL_API bool GetAttribute(const std::string& name, bool& b)				const;
		PROFILE_DLL_API bool GetAttribute(const std::string& name, ExprValueList& l)	const;
		PROFILE_DLL_API bool GetAttribute(const std::string& name, Range& range)		const;
		PROFILE_DLL_API bool GetAttribute(const std::string& name, RangeList& rangeList)const;

		//***********************************************

		PROFILE_DLL_API void AddAttributes (const Profile *profile);

		//***********************************************

		PROFILE_DLL_API const std::string& GetError(void) const { return error; }
		PROFILE_DLL_API void SetError(const std::string& error) { this->error = error; }

		//***********************************************

		PROFILE_DLL_API PairMap * GetPairs(void) const { return pairs; }

		//***********************************************

		PROFILE_DLL_API void AddAttributeChangeCallback		(Callback cb, void *closure);
		PROFILE_DLL_API void RemoveAttributeChangeCallback	(Callback cb, void *closure);
		PROFILE_DLL_API bool HasAttributeChangeCallback		(Callback cb, void *closure);

		//***********************************************

		PROFILE_DLL_API static bool IsValidInstance(const Profile *profile)
			{ return std::find(profiles.begin(), profiles.end(), profile) != profiles.end(); }

		//***********************************************

		PROFILE_DLL_API Profile(void) : pairs((PairMap *) 0), error("") { profiles.push_back(this); }
		PROFILE_DLL_API ~Profile();
	};
}

#endif