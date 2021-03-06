/**
 *	SymbolTable.h
 *
 *	-- DMSL Project --
 *
 *  The SymbolTable class containing information
 *  about stereotypes, defines and components of code.
 *
 *  Version 3 update: Added functionality to merge symbol tables
 *  in order to support incremental rule compilation.
 *
 *	Giannis Lilis <lilis@ics.forth.gr>
 *	August 2006 (Version 1) - July 2007 (Version 2) - January 2008 (Version 3)
 */

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <map>
#include <list>
#include <string>

#include <assert.h>

namespace dmsl {
	///////////////////////////
	// Forward Declarations
	//
	class Statement;
	class Expression;

	//////////////////////////////////////////////////
	// SymbolTable class
	//
	class SymbolTable {
	public:
		typedef std::map<std::string, Expression *>	ExpressionMap;
		typedef std::map<std::string, Statement *>	StatementMap;
	private:
		template<class T> typename T::mapped_type GetElementFromMap(const typename T::key_type& name, const T& map) const {
			typename T::const_iterator i = map.find(name);
			return i == map.end() ? (typename T::mapped_type) 0 : i->second;
		}

		template<class T> bool AddElementToMap(const typename T::key_type& name, typename T::mapped_type& val, T& map, bool overwrite) {
			if(!overwrite && SymbolExists(name))
				return false;
			else {
				map[name] = val;
				return true;
			}
		}

		template<class T> bool RemoveElementFromMap(const typename T::key_type& name, T& map) {
			typename T::iterator i = map.find(name);
			if(i == map.end())
				return false;
			else {
				map.erase(i);
				return true;
			}
		}
		
	public:

#define DEFINE_ITEM_GROUP(_type, _var, _name)														\
	private:																						\
		_type _var;																					\
	public:																							\
		bool Add##_name(const std::string& name, _type::mapped_type val, bool overwrite = false)	\
			{ return AddElementToMap<_type>(name, val, _var, overwrite); }							\
		bool Has##_name(const std::string& name) const { return _var.find(name) != _var.end(); }	\
		_type::mapped_type Get##_name(const std::string& name) const								\
			{ return GetElementFromMap<_type>(name, _var); }										\
			_type& Get##_name##s(void) { return _var; }												\
			const _type& Get##_name##s(void) const { return _var; }

	DEFINE_ITEM_GROUP(ExpressionMap,	stereotypes,	Stereotype)
	DEFINE_ITEM_GROUP(ExpressionMap,	defines,		Define)
	DEFINE_ITEM_GROUP(StatementMap,		components,		Component)

#undef DEFINE_ITEM_GROUP

		//***************************

		bool CheckForRedeclarations(const SymbolTable& table, std::string *error) {

	#define CHECK_REDECLARATION(type, map, func1, func2)						\
		do {																	\
			for(type::const_iterator i = map.begin(); i != map.end(); ++i) {	\
				if(func1(i->first) || func2(i->first)) {							\
					*error = i->first;											\
					return false;												\
				}																\
			}																	\
		}	while(false)

			CHECK_REDECLARATION(ExpressionMap,	table.stereotypes, HasDefine, HasComponent);
			CHECK_REDECLARATION(ExpressionMap,	table.defines, HasStereotype, HasComponent);
			CHECK_REDECLARATION(StatementMap,	table.components, HasStereotype, HasDefine);

	#undef CHECK_REDECLARATION

			return true;
		}
	
		//***************************

		bool SymbolExists(const std::string& name) const
			{ return HasStereotype(name) || HasDefine(name) || HasComponent(name); }

		//***************************

		bool UndefineSymbol(const std::string& name) {
			return	RemoveElementFromMap(name, stereotypes) ||
					RemoveElementFromMap(name, defines)		||
					RemoveElementFromMap(name, components)	;
		}

		//***************************

		void Clear (void) {
			stereotypes.clear();
			defines.clear();
			components.clear();
		}

		//***************************

		SymbolTable(void) {}
		~SymbolTable() { Clear(); }
	};
}

#endif