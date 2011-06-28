/**
 *	Singleton.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef UTIL_SINGLETON_H
#define UTIL_SINGLETON_H

#include "Portability.h"

#include "Common.h"
#include "LibraryAPIDecls.h"

#include <boost/checked_delete.hpp>

namespace util {

////////////////////////////////////////////////////////////////////////

class _BASE_API Singleton {
protected:
	Singleton (void);
	~Singleton (void);

	void OnInitialize (void) {}
	void OnCleanUp (void) {}

private:
	DISABLE_COPY(Singleton);
};

////////////////////////////////////////////////////////////////////////
// Utility macros
//
#define INLINE_SINGLETON_WITH_ARGS(CLASSNAME, FORMALS, ARGS)				\
	public:																	\
		static void Initialize FORMALS {									\
			s_instance = new CLASSNAME ARGS;								\
			s_instance->OnInitialize();										\
		}																	\
		static void	CleanUp (void) {										\
			s_instance->OnCleanUp();										\
			boost::checked_delete<CLASSNAME>(s_instance);					\
			s_instance = 0;													\
		}																	\
		static CLASSNAME& Instance (void)									\
			{ assert(s_instance); return *s_instance; }						\
	private:																\
		friend void boost::checked_delete<CLASSNAME> (CLASSNAME*);			\
		static CLASSNAME* s_instance

#define INLINE_SINGLETON(CLASSNAME)	\
	INLINE_SINGLETON_WITH_ARGS(CLASSNAME, (void), ())

//**********************************************************************

#define DEFINE_SINGLETON_INSTANCE(CLASSNAME)	\
	CLASSNAME* CLASSNAME::s_instance = 0

//**********************************************************************

#define DECLARE_SINGLETON_WITH_ARGS(CLASSNAME, FORMALS)							\
	public:																		\
		static void			Initialize FORMALS;									\
		static void			CleanUp (void);										\
		static CLASSNAME&	Instance (void);									\
	private:																	\
		friend void portability::boost::checked_delete<CLASSNAME> (CLASSNAME*);	\
		static CLASSNAME* s_instance

#define DECLARE_SINGLETON(CLASSNAME)	\
	DECLARE_SINGLETON_WITH_ARGS(CLASSNAME, (void))

//**********************************************************************

#define IMPLEMENT_SINGLETON_WITHOUT_INITIALIZE_AND_CLEANUP(CLASSNAME)					\
	CLASSNAME& CLASSNAME::Instance (void) { assert(s_instance); return *s_instance; }	\
	DEFINE_SINGLETON_INSTANCE(CLASSNAME)

//**********************************************************************

#define IMPLEMENT_SINGLETON_WITH_ARGS(CLASSNAME, FORMALS, ARGS)				\
	void CLASSNAME::Initialize FORMALS {									\
		s_instance = new CLASSNAME ARGS;									\
		s_instance->OnInitialize();											\
	}																		\
	void CLASSNAME::CleanUp (void) {										\
		s_instance->OnCleanUp();											\
		portability::boost::checked_delete<CLASSNAME>(s_instance);			\
		s_instance = 0;														\
	}																		\
	IMPLEMENT_SINGLETON_WITHOUT_INITIALIZE_AND_CLEANUP(CLASSNAME)

#define IMPLEMENT_SINGLETON(CLASSNAME)	\
	IMPLEMENT_SINGLETON_WITH_ARGS(CLASSNAME, (void), ())

////////////////////////////////////////////////////////////////////////

} // namespace util

#endif // UTIL_SINGLETON_H
