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
		typedef std::list<std::string>				StringList;
	private:
		ExpressionMap stereotypes;
		ExpressionMap defines;
		StatementMap  components;

		template<class T> typename T::mapped_type GetElementFromMap(const typename T::key_type& name, const T& map) const {
			typename T::const_iterator i = map.find(name);
			return i == map.end() ? (typename T::mapped_type) 0 : i->second;
		}

		template<class T> bool AddElementToMap(const typename T::key_type& name, typename T::mapped_type& val, T& map) {
			if(SymbolExists(name))
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
		/////////////////
		// Stereotypes
		//
		bool AddStereotype(const std::string& name, Expression *expr)
			{ return AddElementToMap<ExpressionMap>(name, expr, stereotypes); }
		
		bool HasStereotype(const std::string& name) const { return stereotypes.find(name) != stereotypes.end(); }		

		Expression * GetStereotype(const std::string& name) const
			{ return GetElementFromMap<ExpressionMap>(name, stereotypes); }
		
		const ExpressionMap& GetStereotypes(void) const { return stereotypes; }
		
		////////////////
		// Defines
		//
		bool AddDefine(const std::string& name, Expression *expr)
			{ return AddElementToMap<ExpressionMap>(name, expr, defines); }
		
		Expression * GetDefine(const std::string& name) const
			{ return GetElementFromMap<ExpressionMap>(name, defines);  }

		bool HasDefine(const std::string& name) const { return defines.find(name) != defines.end(); }
		
		const ExpressionMap& GetDefines(void) const { return defines; }

		////////////////
		// Components
		//
		bool AddComponent(const std::string& name, Statement *stmt) 
			{ return AddElementToMap<StatementMap>(name, stmt, components); }
		
		Statement * GetComponent(const std::string& name) const
			{ return GetElementFromMap<StatementMap>(name, components); }

		bool HasComponent(const std::string& name) const { return components.find(name) != components.end(); }

		StatementMap&		GetComponents(void)			{ return components; }
		const StatementMap& GetComponents(void) const	{ return components; }

		//***************************

		bool AddSymbols(const SymbolTable& table, StringList& overwrites, std::string *error) {

	#define CHECK_REDECLARATION(type, map, func1, func2)						\
		do {																	\
			for(type::const_iterator i = map.begin(); i != map.end(); ++i) {	\
				if(func1(i->first) || func2(i->first)) {						\
					*error = i->first;											\
					return false;												\
				}																\
			}																	\
		}	while(false)

			CHECK_REDECLARATION(ExpressionMap,	table.stereotypes, HasDefine, HasComponent);
			CHECK_REDECLARATION(ExpressionMap,	table.defines, HasStereotype, HasComponent);
			CHECK_REDECLARATION(StatementMap,	table.components, HasStereotype, HasDefine);

	#undef CHECK_REDECLARATION

	#define ADD_ITEMS(type, map, func)																			\
		do {																									\
			for(type::const_iterator i = table.map.begin(); i != table.map.end(); ++i) {						\
				if(GetElementFromMap(i->first, map)) {	/* Symboltable redefinition entries maintain values. */	\
					overwrites.push_back(i->first);		/* We also switch thei Stmt inner data at AddLogic. */	\
					map[i->first] = i->second;																	\
				}																								\
				else {																							\
					bool added = func(i->first, i->second);														\
					assert(added);																				\
				}																								\
			}																									\
		}	while(false)

			ADD_ITEMS(ExpressionMap,	stereotypes,	AddStereotype);
			ADD_ITEMS(ExpressionMap,	defines,		AddDefine);
			ADD_ITEMS(StatementMap,		components,		AddComponent);

	#undef ADD_ITEMS

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