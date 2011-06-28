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
#include "MenuParser.h"
#include "ComponentUserCommand.h"

#include "Adaptors.h"

#include <algorithm>

namespace ide
{
	///--- Functor OptionSeeker
	struct OptionSeeker :
		std::unary_function<UserCommand::Option, bool>
	{
		OptionSeeker(const String& label)
			: label(label) {}

		result_type operator ()(argument_type option) {
			return label == option.first;
		}

	private:
		const String& label;
	};
	///------------------------------------


	//-------------------------------------------------------//
	//---- struct InsertCommand -----------------------------//

	InsertCommand::result_type InsertCommand::operator ()(
		InsertCommand::first_argument_type first_,
		InsertCommand::second_argument_type last_) const
	{
		//-- Check for option existance
		bool exists = std::find_if(
			parser.parent->menu.lower_bound(parser.state.priority),
			parser.parent->menu.upper_bound(parser.state.priority),
			util::project2nd<UserCommand::Menu::value_type>(
				OptionSeeker(parser.state.label)
			)
		) != parser.parent->menu.upper_bound(parser.state.priority);

		if (!exists) {
			//-- Append separator before
			if (parser.state.sep_before)
				parser.parent->menu.insert(
					std::make_pair(
						parser.state.priority -1,
						std::make_pair(_T("--"), UserCommand(UserCommand::UCT_SEPARATOR))
					)
				);

			//-- Append intermediate sub-menu
			UserCommand cmd(UserCommand::UCT_SUB_MENU, parser.command.GetFlags());
			parser.parent->menu.insert(
				std::make_pair(
					parser.state.priority,
					std::make_pair(parser.state.label, cmd)
				)
			);

			//-- Append separator after
			if (parser.state.sep_after)
				parser.parent->menu.insert(
					std::make_pair(
						parser.state.priority +1,
						std::make_pair(_T("--"), UserCommand(UserCommand::UCT_SEPARATOR))
					)
				);
		}

		//-- Retrieve new parent
		parser.grandpa = parser.parent;
		parser.parent =
			&(std::find_if(
				parser.parent->menu.lower_bound(parser.state.priority),
				parser.parent->menu.upper_bound(parser.state.priority),
				util::project2nd<UserCommand::Menu::value_type>(
					OptionSeeker(parser.state.label)
				)
			)->second.second);

		//-- Reset parser's state
		parser.state.Reset();
	}

	//-----------------------------------------------------------------------


	//-------------------------------------------------------//
	//---- struct MakeLeaf ----------------------------------//

	MakeLeaf::result_type MakeLeaf::operator ()(
		MakeLeaf::first_argument_type first_,
		MakeLeaf::second_argument_type last_) const
	{
		UserCommand::Menu::iterator it =
			std::find_if(
				parser.grandpa->menu.lower_bound(parser.state.priority),
				parser.grandpa->menu.upper_bound(parser.state.priority),
				util::project2nd<UserCommand::Menu::value_type>(
					OptionSeeker(parser.state.label)
				)
			);

		assert(it != parser.grandpa->menu.upper_bound(parser.state.priority));
		it->second.second = parser.command;
	}

	//-----------------------------------------------------------------------
}
