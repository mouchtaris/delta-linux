// DeltaConstantProgramValues.h
// Handling of arrays of constant values as a separated
// class used by both the compiler and the virtual machine
// implementation.
// ScriptFighter Project.
// A. Savidis, June 2005 (re-factoring).
//

#ifndef	DELTACONSTANTPROGRAMVALUES_H
#define	DELTACONSTANTPROGRAMVALUES_H

#include "utypes.h"
#include "DeltaByteCodeTypes.h"
#include "GenericReader.h"
#include "GenericWriter.h"
#include "ufunctors.h"
#include "uerrorclass.h"
#include <string>

////////////////////////////////////////////////////////////////

template <typename T> struct memset_traits{
	static void wipeout (T*, unsigned N){}
};

template <> struct memset_traits<DeltaNumberValueType> {
	static void wipeout (double* p, unsigned N){ memset(p, 0, N * sizeof(double)) ;}
};

template <class T, const util_ui16 N>
class udistinctvaluearray {
	protected:
	T			values[N];
	util_ui16	total;

	public:
	util_ui16	GetTotal (void) const
					{ return total; }
	const T&	Get (util_ui16 index) const {
					DASSERT(index < total && total <= N);
					return values[index];
				}

	udistinctvaluearray(void): total(0) 
		{ memset_traits<T>::wipeout(&values[0], N); }
	virtual ~udistinctvaluearray(){} 
};

////////////////////////////////////////////////////////////////

template <class T, const util_ui16 N>
class udistinctvaluearraywriter : public udistinctvaluearray<T, N>  {

	public:
	struct FullArrayException {};
	
	void Write (FILE* fp, void (*f)(FILE*, util_ui16, const T*)) const
		{ f(fp, this->total, (const T*) this->values); }

	void Write (GenericWriter& writer, void (*f)(GenericWriter& writer, util_ui16, const T*)) const
		{ f(writer, this->total, (const T*) this->values); }

	// Checks if it matches an existing entry, 
	// and returns it. If full, throws an exception.
	//
	util_ui16 Add (const T& val)  {
		DASSERT(this->total < N);

		if (this->total == N)
#ifdef	UHAS_EXCEPTIONS
			throw FullArrayException();
#else
		return N;
#endif
		for (util_ui16 i = 0; i < this->total; ++i)
			if (val == this->values[i])
				return i;

		// Else, append it.
		//
		this->values[this->total] = val;
		return this->total++;	
	}

	udistinctvaluearraywriter(void){}
	~udistinctvaluearraywriter(){}
};

////////////////////////////////////////////////////////////////

template <class T, const util_ui16 N>
class udistinctvaluearrayreader : public udistinctvaluearray<T, N>  {
	public:
	bool Read (GenericReader& reader, bool (*f)(GenericReader&, util_ui16*, T*))
		{ return f(reader, &this->total, (T*) this->values); }

	udistinctvaluearrayreader(void){}
	~udistinctvaluearrayreader(){}
};

////////////////////////////////////////////////////////////////
	
class DBYTECODE_CLASS DeltaConstantArrays {
	public:
	virtual const char*				GetStringConst (util_ui16 index)	const = 0;
	virtual const char*				GetExternFuncUsed (util_ui16 index) const = 0;
	virtual DeltaNumberValueType	GetNumericConst (util_ui16 index)	const = 0;

	DeltaConstantArrays (void){}
	virtual ~DeltaConstantArrays(){}
};

////////////////////////////////////////////////////////////////

class DBYTECODE_CLASS DeltaConstantArraysWriter : public DeltaConstantArrays  {

	private:
	udistinctvaluearraywriter<std::string, DELTA_MAX_STRING_CONSTS>				stringConsts;
	udistinctvaluearraywriter<std::string, DELTA_MAX_EXTERN_FUNCS>				libFuncs;
	udistinctvaluearraywriter<DeltaNumberValueType, DELTA_MAX_NUMERIC_CONSTS>	numericConsts;

	static void 			swritetext (FILE* fp, util_ui16 total, const std::string* values);
	static void 			nwritetext (FILE* fp, util_ui16 total, const DeltaNumberValueType* values);
	static void 			swritebin (GenericWriter& writer, util_ui16 total, const std::string* values);
	static void 			nwritebin (GenericWriter& writer, util_ui16 total, const DeltaNumberValueType* values);

	public:
	const char*				GetStringConst (util_ui16 index) const;
	const char*				GetExternFuncUsed (util_ui16 index) const;
	DeltaNumberValueType	GetNumericConst (util_ui16 index) const;

	util_ui16				AddStringConst (const char* s);
	util_ui16				AddExternFuncUsed (const char* s);
	util_ui16				AddNumericConst (DeltaNumberValueType val);

	void					Write (GenericWriter& writer) const;
	void					WriteText (FILE* fp) const;

	DeltaConstantArraysWriter (void);
	~DeltaConstantArraysWriter();
};


////////////////////////////////////////////////////////////////

class DBYTECODE_CLASS DeltaConstantArraysReader : public DeltaConstantArrays  {

	private:
	udistinctvaluearrayreader<std::string, DELTA_MAX_STRING_CONSTS>				stringConsts;
	udistinctvaluearrayreader<std::string, DELTA_MAX_EXTERN_FUNCS>				libFuncs;
	udistinctvaluearrayreader<DeltaNumberValueType, DELTA_MAX_NUMERIC_CONSTS>	numericConsts;

	static bool 			sreadbin (GenericReader& reader, util_ui16* total, std::string* arr);
	static bool 			nreadbin (GenericReader& reader, util_ui16* total, DeltaNumberValueType* arr);

	public:
	const char*				GetStringConst (util_ui16 index) const;
	const char*				GetExternFuncUsed (util_ui16 index) const;
	DeltaNumberValueType	GetNumericConst (util_ui16 index) const;
	const DeltaNumberValueType*	
							GetNumericConstPtr (util_ui16 index) const;

	bool					Read (GenericReader& reader);

	DeltaConstantArraysReader (void);
	~DeltaConstantArraysReader();
};

////////////////////////////////////////////////////////////////

#endif	// Do not ad stuff beyond this point.


