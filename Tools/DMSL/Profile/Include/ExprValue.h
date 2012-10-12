/**
 *	ExprValue.h
 *
 *	-- DMSL Project --
 *
 *	Representation of the value of an expression.
 *
 *	Giannis Lilis <lilis@ics.forth.gr>
 *	August 2006 (Version 1) - July 2007 (Version 2)
 */

#ifndef EXPR_VALUE_H
#define EXPR_VALUE_H

#include <list>
#include <string>
#include <new>

#include <assert.h>

#include "util.h"

#if defined(PROFILE_EXPORT)
#	define PROFILE_DLL_API __declspec(dllexport)
#elif defined(PROFILE_IMPORT)
#	define PROFILE_DLL_API __declspec(dllimport)
#else
#	define PROFILE_DLL_API
#endif

namespace dmsl {
	class ExprValue;
	typedef std::list<ExprValue *> ExprValueList;

	typedef std::pair<double, double>	Range;
	class RangeList : public util::Clonable<RangeList>, public std::list<Range> {
	public:

		bool IsNormalized(void) const;

		void Normalize(void) {
			const RangeList range = Normalize(*this);
			clear();
			operator=(range);
		}

		static const RangeList		Normalize		(const RangeList& range);
		static const std::string	RangeToString	(const Range& range);

		const std::string toString(void) const;

		bool operator <= (const RangeList& range);
		bool operator == (const RangeList& range);

		RangeList *Clone(void) const { return new RangeList(*this); }

		RangeList(void) : std::list<Range>() {}
		RangeList(size_type count, const Range& val) : std::list<Range>(count, val) {}
		RangeList(const RangeList& range) : std::list<Range>(range) {}
	};

	enum ExprValueType {
		ValueTypeInt		= 0,
		ValueTypeReal		= 1,
		ValueTypeBool		= 2,
		ValueTypeString		= 3,
		ValueTypeRange		= 4,
		ValueTypeRangeList	= 5,
		ValueTypeList		= 6,
		ValueTypeVoid		= 7,
		ValueTypeError		= 8
	};

	//*********************

	union ExprValueContent {
		int				intVal;
		double			realVal;
		bool			boolVal;
		std::string*	strVal;
		Range*			rangeVal;
		RangeList*		rangeListVal;
		ExprValueList*	listVal;
		std::string*	errorVal;
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////

	class PROFILE_DLL_API ExprValue : public util::Clonable<ExprValue> {
	private:
		ExprValueType type;
		ExprValueContent content;

		static ExprValueList * CloneList(const ExprValueList& list)
			{ return util::cloneContainer<ExprValueList>(list, std::mem_fun(&ExprValue::Clone)); }
		
		template<class T> static void DeleteList(T* list) { util::destroyContainer<T>(list); }

	public:
		
		void Clear (void);
		ExprValueType GetType (void) const { return type; }

		//*********************

		double	GetNumber (void) const;
		void	SetNumber (double val);

		//*********************

		const std::string&	GetString (void) const;
		void				SetString (const std::string& str);
		
		//*********************

		bool GetBool (void) const;
		void SetBool (bool val);

		//*********************
		
		Range&			GetRange (void);
		const Range&	GetRange (void) const;
		void			SetRange (const Range& range);

		//*********************
		
		RangeList&			GetRangeList (void);
		const RangeList&	GetRangeList (void) const;
		void				SetRangeList (const RangeList& range);

		//*********************
		
		ExprValueList&			GetList (void);
		const ExprValueList&	GetList (void) const;
		void					SetList (const ExprValueList& list);

		//*********************

		const std::string	GetError (void) const;
		void				SetError (const std::string& error);

		//*********************

		void SetVoid (void);

		//*********************

		bool IsNumber	(void) const { return type == ValueTypeInt || type == ValueTypeReal ; }
		bool IsInteger	(void) const { return type == ValueTypeInt;			}
		bool IsReal		(void) const { return type == ValueTypeReal ;		}
		bool IsBool		(void) const { return type == ValueTypeBool;		}
		bool IsString	(void) const { return type == ValueTypeString;		}
		bool IsRange	(void) const { return type == ValueTypeRange;		}
		bool IsRangeList(void) const { return type == ValueTypeRangeList;	}
		bool IsList		(void) const { return type == ValueTypeList;		}
		bool IsVoid		(void) const { return type == ValueTypeVoid;		}
		bool IsError	(void) const { return type == ValueTypeError;		}

		void operator =	(const ExprValue& expr) { new (this) ExprValue(expr);	}
		bool operator ==(const ExprValue& expr);
		bool operator !=(const ExprValue& expr) { return !operator==(expr);		}

		const std::string toString(void) const;

		ExprValue *Clone(void) const;

		ExprValue (void) : type(ValueTypeError) { content.errorVal = (std::string *) 0;}
		
		ExprValue (int val)					: type(ValueTypeInt)		{ content.intVal = val; }
		ExprValue (double val)				: type(ValueTypeReal)		{ content.realVal = val; }
		ExprValue (bool val)				: type(ValueTypeBool)		{ content.boolVal = val; }
		ExprValue (const char *val)			: type(ValueTypeString)		{ content.strVal = new std::string(val); }
		ExprValue (const std::string& val)	: type(ValueTypeString)		{ content.strVal = new std::string(val.c_str()); }
		ExprValue (Range* val)				: type(ValueTypeRange)		{ content.rangeVal	= val; }
		ExprValue (RangeList* val)			: type(ValueTypeRangeList)	{ content.rangeListVal	= val; }
		ExprValue (ExprValueList *val)		: type(ValueTypeList)		{ content.listVal = val; }
		
		ExprValue (const ExprValue& expr);	//Deep copy

		~ExprValue() { Clear(); }
	};
}

#endif