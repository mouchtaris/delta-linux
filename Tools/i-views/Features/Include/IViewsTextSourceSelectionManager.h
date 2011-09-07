/**
 *	IViewsTextSourceSelectionManager.h
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr>
 *	Octomber 2010
 */

#ifndef	IVIEWS_TEXT_SOURCE_SELECTION_MANAGER_H
#define	IVIEWS_TEXT_SOURCE_SELECTION_MANAGER_H

#include <map>
#include <string>

namespace iviews {
	
	class IViewsTextSourceSelectionManager {
	public:
		IViewsTextSourceSelectionManager();

		void Clear(void);
		void Focus(const std::string & path);
		void RegisterTextSource(const std::string & path);
		bool IsFocused(const std::string & path) const;
		

	private:
		typedef std::map<std::string, bool>	FocusedTextSources;

		FocusedTextSources				table;
		FocusedTextSources::iterator	lastFocusedTextSource;
	};

}

#endif	//IVIEWS_TEXT_SOURCE_SELECTION_MANAGER_H