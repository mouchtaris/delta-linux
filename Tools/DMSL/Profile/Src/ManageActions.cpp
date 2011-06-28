/**
 *	ManageActions.cpp
 *
 *	-- DMSL Project --
 *
 *	Implementation of the manage functions for the Profile bison grammar.
 *
 *	Giannis Lilis <lilis@ics.forth.gr>
 *	August 2006 (Version 1) - July 2007 (Version 2)
 */

#include "ManageActions.h"

#include <assert.h>

#define RETURN_ON_ERROR_UNARY(ret, test)	\
	do {									\
		if(!test) {							\
			*ret = 0;						\
			return;							\
		}									\
	} while(false)

extern int Profile_yylineno;

namespace dmsl {

	/////////////////////////////////////////////////////////////////
	// Manage functions implementation
	//

	void Manage_logic_declList (Profile *, PairMap **map, PairMap *m) {
		RETURN_ON_ERROR_UNARY(map, m);
		*map = m;
	}
	void Manage_logic_empty	(Profile *, PairMap **map)	{ *map = new PairMap; }

	//****************************

	void Manage_declList_declList (Profile *, PairMap **map, PairMap *m, Pair *p) {
		if(!m || !p) {
			if(m)
				std::for_each(
					m->begin(), 
					m->end(),
					util::apply2nd<PairMap::value_type>(util::deleter<ExprValue *>())
				);
			*map = (PairMap *) 0;
		}
		else
			(*map = m)->insert(*p);
		delete p;
	}

	void Manage_declList_decl (Profile *, PairMap **map, Pair *p) {
		RETURN_ON_ERROR_UNARY(map, p);
		(*map = new PairMap)->insert(*p);
		delete p;
	}

	//****************************

	void Manage_decl (Profile *, Pair **p, const char *attribute, ExprValue *value) {
		*p = value ? new Pair(attribute, value) : (Pair *) 0;
		delete[] attribute;
	}

	//****************************

	void Manage_attribute_attribute	(Profile *, const char **id, const char *id1, const char *id2) {
		*id = dmsl::util::concat(3, id1, ".", id2);
		delete[] id1;
		delete[] id2;
	}

	void Manage_attribute_string	(Profile *, const char **id, const char *i)	{ *id = i; }

	//****************************

	void Manage_value_bool (Profile *, ExprValue **value, bool val )	{ *value = new ExprValue(val);	}
	
	void Manage_value_string (Profile *, ExprValue **value, const char* val) {
		*value = new ExprValue(val);
		delete[] val;
	}
	
	void Manage_value_number (Profile *, ExprValue **value, double val) { *value = new ExprValue(val);	}

	void Manage_value_range	(Profile *, ExprValue **value, Range* val)	{
		RETURN_ON_ERROR_UNARY(value, val);
		*value = new ExprValue(val);
	}

	void Manage_value_valueList	(Profile *, ExprValue **value, ExprValueList *l) {
		RETURN_ON_ERROR_UNARY(value, l);
		*value = new ExprValue(l);
	}

	void Manage_value_rangeList	(Profile *, ExprValue **value, RangeList *l) {
		RETURN_ON_ERROR_UNARY(value, l);
		l->Normalize();
		*value = new ExprValue(l);
	}

	//****************************

	void Manage_number_int	(Profile *, double *value, int		val )	{ *value = val;	}
	void Manage_number_real	(Profile *, double *value, double	val )	{ *value = val;	}

	//****************************

	void Manage_range(Profile *profile, Range **range, double left, double right) {
		if(left > right) {
			profile->SetError(
				util::MakeString(
					"Profile: Line %d : Range [%lf .. %lf] is invalid",
					Profile_yylineno,
					left,
					right
				)
			);
			*range = (Range *) 0;
		}
		else
			*range = new Range(left, right);
		
	}

	//****************************

	void Manage_valueList_valueListNonEmpty	(Profile *, ExprValueList **list, ExprValueList *l) {
		RETURN_ON_ERROR_UNARY(list, l);
		*list = l;
	}

	void Manage_valueList_empty	(Profile *, ExprValueList **list)	{ *list = new ExprValueList; }

	//****************************

	void Manage_valueListNonEmpty_valueListNonEmpty	(Profile *, ExprValueList **list, ExprValueList *l, ExprValue *value) {
		if(!l || !value) {
			util::destroyContainer<ExprValueList>(l);
			delete value;
			*list = (ExprValueList *) 0;
		}
		else 
			(*list = l)->push_back(value);
	}

	void Manage_valueListNonEmpty_value(Profile *, ExprValueList **list, ExprValue *value) {
		RETURN_ON_ERROR_UNARY(list, value);
		*list = new ExprValueList(1, value);
	}

	//****************************

	void Manage_rangeList_rangeListNonEmpty	(Profile *, RangeList **list, RangeList *l) {
		RETURN_ON_ERROR_UNARY(list, l);
		*list = l;
	}
	void Manage_rangeList_empty	(Profile *, RangeList **list) { *list = new RangeList; }

	//****************************

	void Manage_rangeListNonEmpty_rangeListNonEmpty	(Profile *, RangeList **list, RangeList *l, Range *value) {
		if(!l || !value) {
			l->clear();
			delete l;
			*list = (RangeList *) 0;
			return;
		}
		else
			(*list = l)->push_back(*value);
		delete value;
	}

	void Manage_rangeListNonEmpty_range	(Profile *, RangeList **list, Range *value) {
		RETURN_ON_ERROR_UNARY(list, value);
		*list = new RangeList(1, *value);
		delete value;
	}
}