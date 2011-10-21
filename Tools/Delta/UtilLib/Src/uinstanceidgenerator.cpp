// uinstanceidgenerator.h
// Class enabling to generate unique resource keys
// (usually for named loadable items in an application)
// supporting three schemes: (i) ids from explicit names; 
// (ii) automatic ids; and (iii) ids from copied items.
// All guarantee unique naming based on distinct prefix
// characters supplied to the class.
// A. Savidis, October 2011.
//

#include "ustrings.h"
#include "uinstanceidgenerator.h"

///////////////////////////////////////////////////////////////////

const std::string uinstanceidgenerator::NewNamedId (const std::string& name) const 
	{ return std::string() + namedPrefix + name;  }

///////////////////////////////////////////////////////////////////

const std::string uinstanceidgenerator::NewAutoId (void) const {
	std::string id;
	while (find(id = uconstructstr("%c%u", autoPrefix, autoIdSerial++)))
		;
	return id;
}

///////////////////////////////////////////////////////////////////

const std::string uinstanceidgenerator::NewCopiedId (const std::string& _id) const {
	std::string id(_id);
	if (IsNamedId(id) || IsAutoId(id))
		id = uconstructstr("%c%s%c0", copiedPrefix, _id.c_str(), copiedSeparator);
	else {
		DASSERT(IsCopiedId(id));

		util_ui32 separatorPos = (util_ui32) id.length() - 1;				// stops where the separator is met

		for (	std::string::reverse_iterator i = id.rbegin(); 
				i != id.rend() && *i == copiedSeparator; 
				++i, --separatorPos	)
				DASSERT(isdigit(*i));

		DASSERT(separatorPos >= 2);											// should have a correct separator
		id = std::string() + copiedPrefix + id.substr(1, separatorPos - 1);	// drop '<prefix>' and '<separator><i>' suffix
	}

	std::string suffix;
	for (util_ui32 i = 0; i < maxCopies; ++i)								// add a unique '<separator><j>' suffix
		if (!find(id + (suffix = uconstructstr("%c%u", copiedSeparator, i))))
			return id + suffix;

	DASSERT(false);															// too many copies
	return invalidId;														// can't create? invalid id
}

///////////////////////////////////////////////////////////