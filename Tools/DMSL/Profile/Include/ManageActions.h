/**
 *	ManageActions.h
 *
 *	-- DMSL Project --
 *
 *	Manage functions for the Profile bison grammar.
 *
 *	Giannis Lilis <lilis@ics.forth.gr>
 *	August 2006 (Version 1) - July 2007 (Version 2)
 */

#ifndef MANAGE_ACTIONS_H
#define MANAGE_ACTIONS_H

#include "Profile.h"

namespace dmsl {

	/////////////////////////////////////////////////////////////////
	// Manage functions declaration
	//
	void Manage_logic_declList	(Profile *profile, PairMap **expr, PairMap *e);
	void Manage_logic_empty		(Profile *profile, PairMap **expr);

	//****************************

	void Manage_declList_declList	(Profile *profile, PairMap **map, PairMap *l, Pair *p);
	void Manage_declList_decl		(Profile *profile, PairMap **map, Pair *p);

	//****************************

	void Manage_decl(Profile *profile, Pair **p, const char *attribute, ExprValue *value);

	//****************************

	void Manage_attribute_attribute	(Profile *profile, const char **id, const char *id1, const char *id2);
	void Manage_attribute_string	(Profile *profile, const char **id, const char *i);

	//****************************

	void Manage_value_bool		(Profile *profile, ExprValue **value, bool			val	);
	void Manage_value_string	(Profile *profile, ExprValue **value, const char*	val	);
	void Manage_value_number	(Profile *profile, ExprValue **value, double		val	);
	void Manage_value_range		(Profile *profile, ExprValue **value, Range*		val	);
	void Manage_value_valueList	(Profile *profile, ExprValue **value, ExprValueList *l);
	void Manage_value_rangeList	(Profile *profile, ExprValue **value, RangeList		*l);

	//****************************

	void Manage_number_int	(Profile *profile, double *value, int	 val);
	void Manage_number_real	(Profile *profile, double *value, double val);

	//****************************

	void Manage_range(Profile *profile, Range **range, double left, double right);

	//****************************

	void Manage_valueList_valueListNonEmpty	(Profile *profile, ExprValueList **list, ExprValueList *l);
	void Manage_valueList_empty				(Profile *profile, ExprValueList **list);

	//****************************

	void Manage_valueListNonEmpty_valueListNonEmpty	(Profile *profile, ExprValueList **list, ExprValueList *l, ExprValue *value);
	void Manage_valueListNonEmpty_value				(Profile *profile, ExprValueList **list, ExprValue *value);

	//****************************

	void Manage_rangeList_rangeListNonEmpty	(Profile *profile, RangeList **list, RangeList *l);
	void Manage_rangeList_empty				(Profile *profile, RangeList **list);

	//****************************

	void Manage_rangeListNonEmpty_rangeListNonEmpty	(Profile *profile, RangeList **list, RangeList *l, Range *value);
	void Manage_rangeListNonEmpty_range				(Profile *profile, RangeList **list, Range *value);
}

#endif