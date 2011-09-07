/**
 *	IViewsTextSourceSelectionManager.cpp
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr>
 *	Octomber 2010
 */

#include <assert.h>
#include "IViewsTextSourceSelectionManager.h"

namespace iviews {

//-----------------------------------------------------------------------

IViewsTextSourceSelectionManager::IViewsTextSourceSelectionManager() 
: table() 
	{ lastFocusedTextSource = table.end(); }

//-----------------------------------------------------------------------

void IViewsTextSourceSelectionManager::Clear(void) 
	{ table.clear(); }

//-----------------------------------------------------------------------

void IViewsTextSourceSelectionManager::RegisterTextSource(const std::string & path) 
	{ table[path] = false; }

//-----------------------------------------------------------------------

bool IViewsTextSourceSelectionManager::IsFocused(const std::string & path) const {
	FocusedTextSources::const_iterator i = table.find(path);
	assert(i != table.end());

	if (i!= table.end())
		return i->second;
	return false;
}

//-----------------------------------------------------------------------

void IViewsTextSourceSelectionManager::Focus(const std::string & path) {
	//Ignore last focused text source
	if (lastFocusedTextSource != table.end())
		lastFocusedTextSource->second = false;

	lastFocusedTextSource = table.find(path);
	assert(lastFocusedTextSource != table.end());

	lastFocusedTextSource->second = true;
}

}	//namespace iviews