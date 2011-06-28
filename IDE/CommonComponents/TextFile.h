/**
 *	TextFile.h
 *
 *	-- IDE Common Components --
 *
 *	TextFile component is the component representing
 *	a text file in the tree view.
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	July 2009
 */
#ifndef COMMON_COMPONENTS_TEXT_FILE_H_
#define COMMON_COMPONENTS_TEXT_FILE_H_

#include "Common.h"
#include "GenericFile.h"

namespace ide
{
	class _IDE_API TextFile :
		public GenericFile
	{
		DECLARE_IDE_COMPONENT(TextFile);

	public:
		///--- type definitions

		///--- constructors / destructor
		TextFile(void);
		~TextFile(void);

		///--- public API

		///--- exported API
		DECLARE_EXPORTED_STATIC(void, Initialize, (void));
		DECLARE_EXPORTED_STATIC(void, CleanUp, (void));

		DECLARE_EXPORTED_FUNCTION_(void, DefaultAction, (void),
			_("Execute file's default action (open)"));

		DECLARE_EXPORTED_FUNCTION(void, Open, (void));
		DECLARE_EXPORTED_FUNCTION(void, View, (void));

	protected:
		///--- protected API
		virtual void ComponentDestroyed(void);

	private:
		///--- private API

		///--- private members
	};
}

#endif	//COMMON_COMPONENTS_TEXT_FILE_H_
