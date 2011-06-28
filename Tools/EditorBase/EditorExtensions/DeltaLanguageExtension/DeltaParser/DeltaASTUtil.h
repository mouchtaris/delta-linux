/**
 *	DeltaASTUtil.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef DELTA_DELTAASTUTIL_H
#define DELTA_DELTAASTUTIL_H

struct YYLTYPE;

namespace delta {

////////////////////////////////////////////////////////////////////////

template<class T>
T* CreateNode (YYLTYPE& start, YYLTYPE& end, typename T::ParamDataType value)
{
	T* result = new T(Range(start.first_column, end.last_column));
	result->SetValue(value);
	return result;
}

template<class T>
T* CreateUnaryNode(
		YYLTYPE&					start,
		YYLTYPE&					end,
		typename T::ValueType*		v,
		typename T::ParamDataType	value
	)
{
	T* result = new T(Range(start.first_column, end.last_column), v);
	result->SetValue(value);
	return result;
}

template<class T>
T* CreateBinaryNode(
		YYLTYPE&					start,
		YYLTYPE&					end,
		typename T::LeftValueType*	v1,
		typename T::RightValueType* v2,
		typename T::ParamDataType	value
	)
{
	T* result = new T(Range(start.first_column, end.last_column), v1, v2);
	result->SetValue(value);
	return result;
}

//**********************************************************************

template<class T>
T* CreateNode(YYLTYPE& start, YYLTYPE& end)
{
	T* result = new T(Range(start.first_column, end.last_column));
	return result;
}

template<class T>
T* CreateUnaryNode(YYLTYPE& start, YYLTYPE& end, typename T::ValueType* v)
{
	T* result = new T(Range(start.first_column, end.last_column), v);
	return result;
}

template<class T>
T* CreateBinaryNode(
		YYLTYPE&					start,
		YYLTYPE&					end,
		typename T::LeftValueType*	v1,
		typename T::RightValueType* v2
	)
{
	T* result = new T(Range(start.first_column, end.last_column), v1, v2);
	return result;
}

////////////////////////////////////////////////////////////////////////

} // namespace delta

#endif // DELTA_DELTAASTUTIL_H
