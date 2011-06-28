// ulexutil.h
// Some common functions used for lexical analysis.
// ScriptFigher Project.
// A. Savidis, January 2000.
//

#ifndef	ULEXUTIL_H
#define	ULEXUTIL_H

#include <string>
#include "utypes.h"
#include "ufunctors.h"
#include "ulatedestructor.h"
#include "uptr.h"

UTILLIB_FUNC	const std::string	uextendescapesequences (const char* src);
UTILLIB_FUNC	const std::string	uunparseescapesequences (const char* src);
UTILLIB_FUNC	const std::string	uunparseescapesequences (const char* src, const char* from, UPTR(const char*) to);
UTILLIB_FUNC	util_ui32			ustrtohex (const char* s);
UTILLIB_FUNC	double				ustrdectodouble (const char* s);
UTILLIB_FUNC	double				ustrfracttodouble (const char* s);
UTILLIB_FUNC	const char*			usaveidstr (const char* s);

UTILLIB_FUNC	void				uignoreCcomments (
										char		(*input)(void), 
										void		(*unput)(char),
										util_ui32*	line,
										void		(*error)(const char*,...)
									);

UTILLIB_FUNC	bool				ureadquotedstring (
										std::string&	s,
										char			(*input)(void), 
										util_ui32*		line,
										void			(*error)(const char*,...)
									);

UTILLIB_FUNC	void				uignoreCPPcomments (
										char (*input)(void), 
										util_ui32*	line
									);

typedef	udestroyablewrapper<
			char*, 
			uarrdestructorfunctor<char*> 
		> udestroyablestring;

#define	UCLASSQUALIFIER_EMPTY

#define	USTRINGDESTRUCTOR_DECLARE(_qualifier, _class)					\
	class _qualifier _class : public ulatedestructionmanager<			\
							udestroyablestring,							\
							uptrdestructorfunctor<udestroyablestring*>	\
						> {												\
		private:														\
		static _class* singletonPtr;									\
		public:															\
		static _class& GetSingleton (void)								\
			{ DASSERT(singletonPtr); return *DPTR(singletonPtr); }		\
		static _class* GetSingletonPtr (void)							\
			{ DASSERT(singletonPtr); return singletonPtr; }				\
		USINGLETON_APISTYLE_DECLARE_PUBLICSTDMETHODS					\
	};

#define	USTRINGDESTRUCTOR_IMPL(_class)									\
	_class* _class::singletonPtr = (_class*) 0;							\
	void _class::SingletonCreate (void) { unew(singletonPtr); }			\
	void _class::SingletonDestroy (void) { udelete(singletonPtr); }

#endif	// Do not add stuff beyonf this point.