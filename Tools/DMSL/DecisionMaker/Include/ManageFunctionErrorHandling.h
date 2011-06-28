/**
 *	ManageFunctionErrorHandling.h
 *
 *	-- DMSL Project --
 *
 *	Error Handling macros used in the various manage
 *  functions used for parsing.
 *
 *	Giannis Lilis <lilis@ics.forth.gr>
 *	June 2008
 */

#ifndef MANAGE_FUNCTION_ERROR_HANDLING_H
#define MANAGE_FUNCTION_ERROR_HANDLING_H

///////////////////////////////////////////////////////////////////////
// Macros to handle semantic errors bottom up NULLing the returned value
//

#define RETURN_ON_ERROR_UNARY(ret, test)	\
	do {									\
		if(!test) {							\
			*ret = 0;						\
			return;							\
		}									\
	} while(false)

#define RETURN_ON_ERROR_BINARY(ret, e1, e2)	\
	do {									\
		if(!e1 || !e2) {					\
			delete e1;	delete e2;			\
			*ret = 0;						\
			return;							\
		}									\
	} while (false)

#define	RETURN_ON_ERROR_TRIADIC(ret, e1, e2, e3)	\
	do {											\
		if(!e1 || ! e2 || !e3) {					\
			delete e1;								\
			delete e2;								\
			delete e3;								\
			*ret = 0;								\
			return;									\
		}											\
	} while(false)

#define RETURN_ON_ERROR_LIST(dm, ret, l, s)			\
	do {											\
		if(!l || !s) {								\
			util::destroyContainer<StmtList>(l);	\
			Statement::RemoveList(dm, l);			\
			delete s;								\
			*ret = 0;								\
			return;									\
		}											\
	} while(false)

///////////////////////////////////////////////////////
// Macros to handle type and argument checking errors
//

#define SET_ERROR_WITH_ZERO_ARGS(var, error)								\
	var->SetError(DecisionMaker::GetConfiguration().GetStringAttribute(error))

#define SET_ERROR_WITH_ONE_ARG(var, error, arg1)							\
	var->SetError(util::MakeString(DecisionMaker::GetConfiguration().GetStringAttribute(error), arg1))

#define SET_ERROR_WITH_TWO_ARGS(var, error, arg1, arg2)						\
	var->SetError(															\
		util::MakeString(													\
			DecisionMaker::GetConfiguration().GetStringAttribute(error),	\
			arg1, arg2														\
		)																	\
	)

#define SET_ERROR_WITH_THREE_ARGS(var, error, arg1, arg2, arg3)				\
	var->SetError(															\
		util::MakeString(													\
			DecisionMaker::GetConfiguration().GetStringAttribute(error),	\
			arg1, arg2, arg3												\
		)																	\
	)

#define SET_ERROR_WITH_FOUR_ARGS(var, error, arg1, arg2, arg3, arg4)		\
	var->SetError(															\
		util::MakeString(													\
			DecisionMaker::GetConfiguration().GetStringAttribute(error),	\
			arg1, arg2, arg3, arg4											\
		)																	\
	)

//*************************************************************************

#define APPEND_WARNING_WITH_ZERO_ARGS(var, warn)						\
	var->AppendWarning(DecisionMaker::GetConfiguration().GetStringAttribute(warn))

#define APPEND_WARNING_WITH_ONE_ARG(var, warn, arg1)					\
	var->AppendWarning(util::MakeString(DecisionMaker::GetConfiguration().GetStringAttribute(warn), arg1))

#define APPEND_WARNING_WITH_TWO_ARGS(var, warn, arg1, arg2)				\
	var->AppendWarning(													\
		util::MakeString(												\
			DecisionMaker::GetConfiguration().GetStringAttribute(warn),	\
			arg1, arg2													\
		)																\
	)

#define APPEND_WARNING_WITH_THREE_ARGS(var, warn, arg1, arg2, arg3)		\
	var->AppendWarning(													\
		util::MakeString(												\
			DecisionMaker::GetConfiguration().GetStringAttribute(warn),	\
			arg1, arg2, arg3											\
		)																\
	)

#endif