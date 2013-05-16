/**
 *	MenuParser.h
 *
 *	-- IDE Component User Command --
 *
 *	MenuParser class is a parser that builds
 *	UserCommands based on a string format.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	May 2007
 */
#ifndef MENU_PARSER_H_
#define MENU_PARSER_H_

#include "Portability.h"

#include "Common.h"
#include "IDECommon.h"

#ifdef BOOST_1_34
#	include <boost/spirit/core.hpp>
#	include <boost/spirit/utility.hpp>
#	include <boost/spirit/actor.hpp>
#else
#	include <boost/spirit/include/classic_core.hpp>
#	include <boost/spirit/include/classic_utility.hpp>
#	include <boost/spirit/include/classic_actor.hpp>
#endif

#include <string>
#include <cstdio>

namespace ide
{
	class UserCommand;
	struct MenuParser;

	//------------------------------
	//-- Semantic Actions

	///--- Action print
	struct print :
		public std::binary_function<const tchar*, const tchar*, void>
	{
		result_type operator ()(first_argument_type first_, second_argument_type last_) const {
			String symbol(first_, last_);
			fprintf(stderr, "Symbol '%s' recognized\n", symbol.c_str());
		}

		void operator ()(const tchar* string) const {
			fprintf(stderr, "Symbol '%s' recognized\n", string);
		}

		void operator ()(int priority) const {
			fprintf(stderr, "Priority '%d' found\n", priority);
		}
	};
	///------------------------------------


	///--- Action InsertCommand
	struct InsertCommand :
		public std::binary_function<const tchar*, const tchar*, void>
	{
		InsertCommand(const MenuParser& parser)
			: parser(parser) {}

		result_type operator ()(first_argument_type first_, second_argument_type last_) const;

	private:
		const MenuParser& parser;
	};
	///------------------------------------


	///--- Action MakeLeaf
	struct MakeLeaf :
		public std::binary_function<const tchar*, const tchar*, void>
	{
		MakeLeaf(const MenuParser& parser)
			: parser(parser) {}

		result_type operator ()(first_argument_type first_, second_argument_type last_) const;

	private:
		const MenuParser& parser;
	};
	///------------------------------------


	//------------------------------
	//-- struct MenuParser

	struct MenuParser :
		public BOOST_SPIRIT_CLASSIC_GRAMMAR(MenuParser)
	{
		///--- ParserState
		struct State
		{
			String label;
			int priority;
			bool sep_before;
			bool sep_after;
			bool check;

			State(void) { Reset(); }

			void Reset(void)
			{
				label.clear();//=0;
				priority=0;
				sep_before = sep_after = check = false;
			}
		};
		///------------------------------------


		///--- constructors / destructor
		MenuParser(const UserCommand& command, UserCommand* parent, State& state=State())
			: command(command), parent(parent), grandpa(0), state(state) {}

		///--- public members
		const UserCommand& command;
		mutable UserCommand* parent;
		mutable UserCommand* grandpa;
		State& state;

		///--- Grammar Definition
		template <typename ScannerT>
		struct definition
		{
			definition(const MenuParser& self)
			{
				using namespace BOOST_SPIRIT_CLASSIC_NAMESPACE;

				//-- Grammar Rules
				init
					= content_list
					;

				content_list
					= SLASH >> content	[InsertCommand(self)]
						>> content_list
					| SLASH >> content	[MakeLeaf(self)]
					;

				content
					= priority >> option
					| option
					;

				priority
					= L_CURLY >> int_p			[assign_a(self.state.priority)]
						>> R_CURLY
					;

				option
					= SEPARATOR					[assign_a(self.state.sep_before, true)]
						>> label >> SEPARATOR	[assign_a(self.state.sep_after, true)]
					| SEPARATOR					[assign_a(self.state.sep_before, true)]
						>> label
					| label >> SEPARATOR		[assign_a(self.state.sep_after, true)]
					| label
					;

				label
					= CHECK		[assign_a(self.state.check, true)]
						>> ID	[assign_a(self.state.label)]
					| ID		[assign_a(self.state.label)]
					;

				ID			= *(anychar_p - tchar('/') - _T("--"));
				SLASH		= str_p(_T("/"));
				CHECK		= str_p(_T("@"));
				L_CURLY		= str_p(_T("{"));
				R_CURLY		= str_p(_T("}"));
				SEPARATOR	= str_p(_T("--"));
			}

			//-- terminals
			BOOST_SPIRIT_CLASSIC_RULE(ScannerT)
				ID, SLASH, CHECK, L_CURLY, R_CURLY, SEPARATOR;

			//-- nonterminals
			BOOST_SPIRIT_CLASSIC_RULE(ScannerT)
				init, content_list, content, priority, option, label;

			//-- start symbol
			const BOOST_SPIRIT_CLASSIC_RULE(ScannerT)& start() const {
				return init;
			}
		};
	};

	///--- Skip Grammar
	struct SkipGrammar :
		public BOOST_SPIRIT_CLASSIC_GRAMMAR(SkipGrammar)
	{
		template <typename ScannerT>
		struct definition
		{
			definition(const SkipGrammar&)
			{
				using namespace BOOST_SPIRIT_CLASSIC_NAMESPACE;

#if defined(BOOST_MSVC) && (BOOST_MSVC <= 1200)
				skip
					=   space_p
					|   _T("//") >> *(anychar_p - tchar('\n')) >> tchar('\n')
					|   _T("/*") >> *(anychar_p - _T("*/")) >> _T("*/")
					;
#else
				skip
					=   space_p
					|   comment_p(_T("//"))
					|   comment_p(_T("/*"), _T("*/"))
					;
#endif
			}

			const BOOST_SPIRIT_CLASSIC_RULE(ScannerT)& start(void) const {
				return skip;
			}

			BOOST_SPIRIT_CLASSIC_RULE(ScannerT) skip;
		};
	};


	//------------------------------
	//-- function parse

	template<typename GrammarT>
	bool parse(const GrammarT& g, const String& str, bool phrase=true)
	{
		SkipGrammar skip;
		BOOST_SPIRIT_CLASS_PARSE_INFO(const tchar*) result;
		result.stop = str.c_str();
		const tchar* end = str.c_str() + str.length();

		do {
			result = (phrase)
				? BOOST_SPIRIT_CLASSIC_PARSE(result.stop, g, skip)
				: BOOST_SPIRIT_CLASSIC_PARSE(result.stop, g);
			++result.stop;
		} while (result.stop < end);

		return true;
	}
}

#endif	//MENU_PARSER_H_
