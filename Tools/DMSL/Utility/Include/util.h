/**
 *	util.h
 *
 *	-- DMSL Project --
 *
 *	Namespace with utility functions and classes.
 *
 *	Giannis Lilis <lilis@ics.forth.gr>
 *	August 2006 (Version 1) - July 2007 (Version 2)
 */

#ifndef UTIL_H
#define UTIL_H

#include <algorithm>
#include <list>
#include <string>
#include <time.h>

#if defined(UTILITY_EXPORT)
#	define UTILITY_DLL_API __declspec(dllexport)
#elif defined(UTILITY_IMPORT)
#	define UTILITY_DLL_API __declspec(dllimport)
#else
#	define UTILITY_DLL_API
#endif

namespace dmsl {
	namespace util {
		UTILITY_DLL_API const char* MakeString (const std::string format, ...);
		
		UTILITY_DLL_API char* strdup (const char *s);
		UTILITY_DLL_API char* strdup (const char *s, unsigned int len);

		/////////////////////////////////////////////////////////
		// Concats the strings given in the in the argument list
		//
		UTILITY_DLL_API const char* concat (unsigned int numOfArgs, ...);

		//**********************************************************************************************

		UTILITY_DLL_API const std::string toString (int num);
		UTILITY_DLL_API const std::string toString (double num);
		UTILITY_DLL_API const std::string toString (bool b);
		UTILITY_DLL_API const std::string toString (const std::string& s);

		UTILITY_DLL_API time_t time(time_t *t);

		//**********************************************************************************************
		
		template<class Container, typename Func> const std::string toString(
			const Container& container,
			Func toStringFunc,
			const char braces[2] = "{}"
		)
		{
			std::string str(1, braces[0]);
			for(typename Container::const_iterator i = container.begin(); i != container.end(); ++i) {
				if(i != container.begin())
					str += ", ";
				str += toStringFunc(*i);
			}
			str += braces[1];
			return str;
		}

		//**********************************************************************************************

		template <class T> struct deleter : public std::unary_function<T, void> {
			void operator() (T pointer) const { delete pointer; }
		};

		template <class T> struct arraydeleter : public std::unary_function<T*, void> {
			void operator() (T* pointer) const { delete[] pointer; }
		};

		//**********************************************************************************************

		template <class Pair, class Func> struct applyer1st : public std::unary_function<Pair&, void> {
			applyer1st (Func func) : m_func(func) {}
			void operator() (Pair& entry) const { m_func(entry.first); }

			private:
				Func m_func;
		};

		template <class Pair, class Func> struct applyer2nd : public std::unary_function<Pair&, void> {
			applyer2nd (Func func) : m_func(func) {}
			void operator() (Pair& entry) const { m_func(entry.second); }

			private:
				Func m_func;
		};

		template <class Pair, class Func> inline applyer1st<Pair, Func>
			apply1st (Func func) { return applyer1st<Pair, Func>(func); }

		template <class Pair, class Func> inline applyer2nd<Pair, Func>
			apply2nd (Func func) { return applyer2nd<Pair, Func>(func); }

		//**********************************************************************************************

		template<class T, typename Func> T* cloneContainer(T const *container, Func cloner)
			{ return cloneContainer(*container, cloner); }

		template<class T, typename Func> T* cloneContainer(const T& container, Func cloner) {
			T *t = new T;
			for(typename T::const_iterator i = container.begin(); i != container.end(); ++i)
				t->push_back((typename T::value_type)cloner(*i));
			return t;
		}

		template<class T> class Clonable {
			public:
				virtual T* Clone() const = 0;
		};

		//**********************************************************************************************

		template<class T> void destroyContainer(T& container) {
			typedef typename T::value_type ContainerType;
			std::for_each(container.begin(), container.end(), deleter<ContainerType>());
			container.clear();
		}

		//**********************************************************************************************

		template<class T> void destroyContainer(T* container) {
			if(container) {
				destroyContainer(*container);
				delete container;
			}
		}

		//**********************************************************************************************

		template<class T> class ListCopyFunctor : public std::unary_function<typename T::value_type, void> {
			T& objects;
			public:
				ListCopyFunctor (T& l): objects(l) {}
				void operator()(const typename T::value_type& p) const { objects.push_back(p); }
		};
	}
}

#endif