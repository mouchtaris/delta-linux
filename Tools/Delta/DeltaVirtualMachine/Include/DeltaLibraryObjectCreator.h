// DeltaLibraryObjectCreator.h
// A utility to generate an object (table) from an array
// of library functions, so that an object-based API of library functions
// can be made possible, requiring no compile-time awareness of the
// the functions per se.
// A. Savidis, December 2007.
//

#ifndef	DELTA_LIBRARYOBJECTCREATOR_H
#define	DELTA_LIBRARYOBJECTCREATOR_H

#include "DeltaValue.h"
#include "DeltaObject.h"
#include "uerrorclass.h"
#include <functional>

////////////////////////////////////////////////////////////////////

class DVM_CLASS DeltaLibraryObjectCreator {

	public:
	struct FuncEntry {	// Intentionally a POD.
		const char*			name;
		DeltaLibraryFunc	func;
	};

	struct APIEntry {
		const char*			name;
		DeltaObject*		api;
	};

	typedef std::pair<FuncEntry*, util_ui32>	Functions;
	typedef std::pair<APIEntry*, util_ui32>		APIs;

	////////////////////////////////////////////////////////////////////

	private:	
	template <class T> struct ContentMakeTraits {};


	template <class T> 
	static DeltaObject* CreateTemplate (
								const T&		info, 
								const char*		type, 
								DeltaObject* obj
							) {
	
		typename T::first_type	p	= info.first;
		typename T::second_type	n	= info.second;
		if (!obj) {
			obj = DeltaObject::NewObject();
			obj->IncRefCounter((DeltaValue*) 0);
		}

		while (n--) {

			DeltaValue	index;
			index.FromString(p->name);
			DeltaValue content;
			ContentMakeTraits<typename T::first_type>::Make(content, *p);

			if (!DPTR(obj)->Set(index, content)) {
				uerror::GetSingleton().postdomino(
					"Library creation (%ss): failed to add %s '%s'!",
					type,
					type,
					p->name
				);

				DPTR(obj)->DecRefCounter((DeltaValue*) 0);	// To commit suicide.
				return (DeltaObject*) 0;
			}
			++p;
		}
		return obj;
	}

	static void				Handle (
								const Functions&	info,
								const char*			prefix,
								bool				(*f)(const std::string&, DeltaLibraryFunc)
							);	

	////////////////////////////////////////////////////////////////////

	public:
	static DeltaObject*	Create (		// Make an API.
								const Functions& info, 
								DeltaObject* obj = (DeltaObject*) 0
							);

	static DeltaObject*	Create (		// Populate many APIs.
								const APIs& info, 
								DeltaObject* obj = (DeltaObject*) 0
							);

	static void				Add (	
								DeltaObject* mainAPI, 
								DeltaObject*	subAPI,
								const char*		name
							);

	static void				Install (
								const Functions&	info,
								const char*			prefix = (char*) 0
							);	
	static void				Uninstall (
								const Functions&	info,
								const char*			prefix = (char*) 0
							);	

	template <const util_ui32 N>
	static const Functions	GetFunctions (FuncEntry (&P)[N]) {
								DASSERT(sizeof(P) / sizeof(FuncEntry) == N);
								return	DeltaLibraryObjectCreator::Functions(P, N);
							}

	template <const util_ui32 N>
	static DeltaObject* CreateAPI (
								FuncEntry		(&P)[N],
								DeltaObject*	mainAPI,
								const char*		apiName,
								const char*		prefix
							) {
								Functions		funcs	= GetFunctions(P);
								DeltaObject* api		= Create(funcs);
								DeltaLibraryObjectCreator::Install(funcs, prefix);
								if (mainAPI)
									DeltaLibraryObjectCreator::Add(mainAPI, api, apiName);
								return api;
							}

	template <const util_ui32 N>
	static void				DestroyAPI (
								FuncEntry		(&P)[N],
								DeltaObject*	api,
								const char*		prefix
							) {
								Functions funcs	= GetFunctions(P);
								DeltaLibraryObjectCreator::Uninstall(funcs, prefix);
								DestroyAPI(api);
							}

	template <const util_ui32 N>
	static void				ExtendAPI (
								FuncEntry		(&P)[N],
								DeltaObject*	api,
								const char*		prefix = (char*) 0
							) {
								DASSERT(api);
								Functions funcs	= GetFunctions(P);
								Create(funcs, api);
								DeltaLibraryObjectCreator::Install(funcs, prefix);
							}

	static DeltaObject* CreateAPI (void) {
								DeltaObject* api = DeltaObject::NewObject();
								DPTR(api)->IncRefCounter((DeltaValue*) 0);
								return api;
							}

	static void				DestroyAPI (DeltaObject*& api) {
								DPTR(api)->DecRefCounter((DeltaValue*) 0); 
								api = (DeltaObject*) 0; 
							}
};

template<>
struct DeltaLibraryObjectCreator::ContentMakeTraits<DeltaLibraryObjectCreator::FuncEntry*> {
	static void Make (DeltaValue& o, const DeltaLibraryObjectCreator::FuncEntry& p)
		{ o.FromLibraryFunc(p.func); }
};

template<>
struct DeltaLibraryObjectCreator::ContentMakeTraits<DeltaLibraryObjectCreator::APIEntry*> {
	static void Make (DeltaValue& o, const DeltaLibraryObjectCreator::APIEntry& p)
		{ o.FromTable(p.api); }
};

////////////////////////////////////////////////////////////////////

#define	DELTA_LIBSUBAPI_INITCLEANUP_EMPTY

#define	DELTA_LIBRARY_RETAINNAME_PREFIX	"$"

#define	DELTA_LIBRARY_INSTALL(funcs)										\
	DeltaLibraryObjectCreator::Install(										\
		DeltaLibraryObjectCreator::Functions(funcs, uarraysize(funcs))		\
	)

#define	DELTA_LIBRARY_UNINSTALL(funcs)										\
	DeltaLibraryObjectCreator::Uninstall(									\
		DeltaLibraryObjectCreator::Functions(funcs, uarraysize(funcs))		\
	)

#define	DELTA_LIBRARY_INSTALL_EX(funcs, prefix)								\
	DeltaLibraryObjectCreator::Install(										\
		DeltaLibraryObjectCreator::Functions(funcs, uarraysize(funcs)),		\
		prefix																\
	)

#define	DELTA_LIBRARY_UNINSTALL_EX(funcs, prefix)							\
	DeltaLibraryObjectCreator::Uninstall(									\
		DeltaLibraryObjectCreator::Functions(funcs, uarraysize(funcs)),		\
		prefix																\
	)

#define	DELTA_LIBRARY_SUBAPI_INSTALLER_EX(lib, apiName, prefix, init, cleanup)				\
	static DeltaObject* api = (DeltaObject*) 0;												\
	void Install_##lib##_Lib (DeltaObject* mainAPI)											\
	{ init; api = DeltaLibraryObjectCreator::CreateAPI(funcs, mainAPI, apiName, prefix); }	\
	void CleanUp_##lib##_Lib (void)															\
	{ cleanup; DeltaLibraryObjectCreator::DestroyAPI(funcs, api, prefix); }

#define	DELTA_LIBRARY_SUBAPI_INSTALLER(lib, apiName, prefix)								\
	DELTA_LIBRARY_SUBAPI_INSTALLER_EX(														\
		lib,																				\
		apiName,																			\
		prefix,																				\
		DELTA_LIBSUBAPI_INITCLEANUP_EMPTY,													\
		DELTA_LIBSUBAPI_INITCLEANUP_EMPTY													\
	)

#define	DELTA_LIBRARY_GETAPI_IMPL(getapi, api)					\
	static void getapi##_LibFunc (DeltaVirtualMachine* vm) {	\
	ISSUE_FUNC(#getapi);										\
	DeltaTotalArgsCheck(0, CURR_FUNC, RESET_NIL_RETURNVALUE);	\
	DPTR(vm)->GetReturnValue().FromTable(api);					\
}

//------------------------------------------------------------------

#endif	// Do not add stuff beyond this point.