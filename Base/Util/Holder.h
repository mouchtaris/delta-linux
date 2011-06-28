/**
 *	Holder.h
 *
 *	-- Utility Library --
 *	Header file defining a generic Holder class.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	March 2007.
 */
#ifndef UTIL_HOLDER_H_
#define UTIL_HOLDER_H_

#include "Common.h"
#include "Streams.h"
#include "Singleton.h"

#include <string>
#include <map>

namespace util
{
	//----------------------------
	//-- policy PtrStoragePolicy

	template <class Element>
	struct PtrStoragePolicy
	{
		typedef Element* value_type;

		typedef PARAM(value_type)	param_type;
		typedef REF(value_type)		reference;
		typedef CREF(value_type)	const_reference;

		static const value_type null_entity;
	};

	template <class Element>
	const typename PtrStoragePolicy<Element>::value_type
	PtrStoragePolicy<Element>::null_entity =0;


	//----------------------------
	//-- policy InstStoragePolicy

	template <class Element>
	struct InstStoragePolicy
	{
		typedef Element value_type;

		typedef PARAM(value_type)	param_type;
		typedef REF(value_type)		reference;
		typedef CREF(value_type)	const_reference;

		static const value_type null_entity;
	};

	template <class Element>
	const typename InstStoragePolicy<Element>::value_type
	InstStoragePolicy<Element>::null_entity = value_type();


	//----------------------------
	//-- class Holder

	template
	<
		class Key,
		class Element,
		template <class> class StoragePolicy = PtrStoragePolicy
	>
	class Holder :
		public Singleton,
		public StoragePolicy<Element>
	{
		INLINE_SINGLETON(Holder);

	public:
		///--- type definitions
		typedef typename StoragePolicy<Element>::value_type value_type;
		typedef typename StoragePolicy<Element>::param_type param_type;
		typedef typename StoragePolicy<Element>::reference reference;
		typedef typename StoragePolicy<Element>::const_reference const_reference;

		///--- public API
		void Insert(PARAM(Key) id, param_type e) {
			elements[id] = e;
		}
		void Remove(PARAM(Key) id) {
			elements.erase(id);
		}

		param_type Get(PARAM(Key) id) {
			ElementMapping::iterator it = elements.find(id);
			return (it == elements.end())
				? StoragePolicy<Element>::null_entity
				: it->second;
		}

		bool Exists(PARAM(Key) id) {
			ElementMapping::iterator it = elements.find(id);
			return (it == elements.end()) ? false : true;
		}

	private:
		///--- private members
		typedef std::map<Key, value_type> ElementMapping;
		ElementMapping elements;
	};

	template
	<
		class Key,
		class Element,
		template <class> class StoragePolicy
	>
	Holder<Key, Element, StoragePolicy>* Holder<Key, Element, StoragePolicy>::s_instance =0;
}

#endif	//UTIL_HOLDER_H_
