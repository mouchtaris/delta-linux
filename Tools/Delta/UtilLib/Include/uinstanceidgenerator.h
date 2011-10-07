// uinstanceidgenerator.h
// Class enabling to generate unique resource keys
// (usually for named loadable items in an application)
// supporting three schemes: (i) ids from explicit names; 
// (ii) automatic ids; and (iii) ids from copied items.
// All guarantee unique naming based on distinct prefix
// characters supplied to the class.
// A. Savidis, October 2011.
//

#ifndef	UINSTANCEIDGENERATOR_H
#define	UINSTANCEIDGENERATOR_H

#include "DDebug.h"
#include "utypes.h"
#include "ufunctors.h"
#include "ucallbacks.h"

///////////////////////////////////////////////////////////////////

class UTILLIB_CLASS uinstanceidgenerator {
	public:
	typedef bool						(*FindCallback)(const std::string& id, void* closure); 

	private:
	mutable util_ui32					autoIdSerial;
	char								namedPrefix;
	char								autoPrefix;
	char								copiedPrefix;
	char								copiedSeparator;
	util_ui32							maxCopies;
	std::string							invalidId;
	ucallbackwithclosure<FindCallback>	find;

	bool								IsNamedId (const std::string& id) const 
											{ DASSERT(!id.empty()); return id[0] == namedPrefix; }
	bool								IsAutoId (const std::string& id) const
											{ DASSERT(!id.empty()); return id[0] == autoPrefix; }
	bool								IsCopiedId (const std::string& id) const
											{ DASSERT(!id.empty()); return id[0] == copiedPrefix; }

	public:
	const std::string					NewNamedId (const std::string& name) const;
	const std::string					NewAutoId (void) const;
	const std::string					NewCopiedId (const std::string& id) const;

	uinstanceidgenerator (
		char				_namedPrefix,
		char				_autoPrefix,
		char				_copiedPrefix,
		char				_copiedSeparator,
		util_ui32			_maxCopies,
		const std::string&	_invalidId,
		FindCallback		_f, 
		void*				_c = (void*) 0
	):	autoIdSerial	(0),
		namedPrefix		(_namedPrefix),
		autoPrefix		(_autoPrefix),
		copiedPrefix	(_copiedPrefix),
		copiedSeparator	(_copiedSeparator),
		maxCopies		(_maxCopies),
		invalidId		(_invalidId)
		{ find.set(_f, _c); }
	~uinstanceidgenerator()
		{}
};

///////////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
