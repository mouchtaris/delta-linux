/**
 *	PrototypeFactory.h
 *
 *	-- Utility Library --
 *	Header file defining a prototype factory mechanism
 *	that can load static and dynamic elements.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	March 2007.
 */
#ifndef UTIL_PROTOTYPE_FACTORY_H_
#define UTIL_PROTOTYPE_FACTORY_H_

#include "Common.h"
#include "Streams.h"
#include "Singleton.h"

#include <string>
#include <map>

#ifdef WIN32
#	define WIN32_LEAN_AND_MEAN		///< Exclude rarely-used stuff from Windows headers
#	include <windows.h>
#endif	// WIN32

namespace util
{
	//----------------------------
	//-- class PrototypeFactory

	template <class Element>
	class PrototypeFactory :
		public Singleton
	{
	private:
		//----------------------------
		//-- class Prototype

		template <class Element>
		class Prototype
		{
		public:
			virtual Element* GetPrototype(void) =0;
			virtual Element* CreateInstance(void) =0;
			virtual void ReleaseInstance(Element* plugin) =0;

		protected:
			///--- private members
			std::string	name;
			Element*	element;
		};

		typedef std::map<std::string, Prototype<Element>*> ElementMapping;

	public:
		///--- public static API
		static void Initialize(void) {
			instance = new PrototypeFactory;
		}

		static void CleanUp(void) {
			//-- Release all I/O plug-ins
			std::for_each(elements.begin(), elements.end(),
				util::project2nd<PluginMap::value_type>(util::deleter<Prototype<Element>*>()));
			elements.clear();

			delete instance;
			instance=0;
		}

		static PrototypeFactory& Instance(void) {
			assert(instance); return *instance;
		}

		///--- public API
		void Register(const std::string& name, const std::string& source) {
			elements[name] = new DynamicPrototype<Element>(source);
		}

		void Register(const std::string& name, Element* element) {
			elements[name] = new StaticPrototype<Element>(name, element);
		}

		Element* GetPrototype(const std::string& name) {
			PluginMap::iterator it = elements.find(name);
			return (it != elements.end() ? it->second->GetPrototype() : 0);
		}

		Element* CreateInstance(const std::string& name) {
			PluginMap::iterator it = elements.find(name);
			return (it != elements.end() ? it->second->CreateInstance() : 0);
		}

		void ReleaseInstance(const std::string& name) {
			PluginMap::iterator it = elements.find(name);
			if (it != plugins.end()) {
				it->second->ReleaseInstance(element);
				elements.erase(it);
			}
		}

	protected:
		///--- constructors / destructor
		PrototypeFactory(void) {}
		~PrototypeFactory(void) {}

	private:
		///--- private members
		static PrototypeFactory* instance;
		ElementMapping elements;
	};

	template <class Element>
	typename PrototypeFactory<Element>* PrototypeFactory<Element>::instance;


	//----------------------------
	//-- class StaticPrototype

	template <class Element>
	class StaticPrototype :
		public PrototypeFactory<Element>::Prototype
	{
	public:
		///--- constructors / destructor
		StaticPrototype(const std::string& name, Element* element)
			: name(name), element(element) {}
		~StaticPrototype(void) {
			ReleaseInstance(element);
		}

		///--- public API
		Element* GetInstance(void) {
			assert(element);
			return element;
		}

		Element* CreateInstance(void) {
			assert(element);
			return element->Clone();
		}

		void ReleaseInstance(Element* element) {
			assert(element && element != this->element);
			delete element;
		}

	private:
		///--- private API
		void ReleasePrototype(void) {
			if (element)
				delete element;
			element=0;
		}
	};


	//----------------------------
	//-- class DynamicPrototype

	template <class Element>
	class DynamicPrototype :
		public PrototypeFactory<Element>::Prototype
	{
	public:
		///--- constructors / destructor
		DynamicPrototype(const std::string& dll)
			: element(0)
		{
			//-- Load Dynamicly Linked Library
			hDll = LoadLibrary(dll.c_str());
			if (!hDll)
				TRACEOUT << "DLL: '" << dll << "' is missing!" << std::endl;

			//-- Get Constructor/Destructor function pointers from DLL
			pfnConstructor = (PFN_NEW_OBJ)GetProcAddress(hDll, "NewPlugin");
			pfnDestructor  = (PFN_RLS_OBJ)GetProcAddress(hDll, "ReleasePlugin");

			//-- Should I fail to retrive function pointers I should release the dynamic library (note: it won't unload itself with program termination)
			if (pfnConstructor == 0 || pfnDestructor == 0) {
				FreeLibrary(hDll);
				TRACEOUT << "DLL: '" << dll << "' does not implement the expected plug-in interface" << std::endl;
			}

			//-- Create prototype element
			element = (Element*)pfnConstructor();
		}

		~DynamicPrototype(void)
		{
			//-- Release Prototype Object
			ReleasePrototype();

			//-- Release Dynamic Library
			FreeLibrary(hDll);
		}

		///--- public API
		Element* GetInstance(void) {
			assert(element);
			return element;
		}

		Element* CreateInstance(void) {
			assert(element);
			return (Element*)pfnConstructor();
		}

		void ReleaseInstance(Element* element) {
			assert(element && element != this->element);
			pfnDestructor((void*)element);
		}

	private:
		///--- private API
		void ReleasePrototype(void) {
			if (element)
				pfnDestructor((void*)element);
			element=0;
		}

		///--- private type definitions
		typedef void* (*PFN_NEW_OBJ)(void);
		typedef void  (*PFN_RLS_OBJ)(void*);

		///--- private members
		PFN_NEW_OBJ pfnConstructor;		///< function pointer to NewPlugin dll function
		PFN_RLS_OBJ	pfnDestructor;		///< function pointer to ReleasePlugin dll function

		HINSTANCE	hDll;
	};
}

#endif	//UTIL_PROTOTYPE_FACTORY_H_
