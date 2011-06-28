/**
 *	DeltaASTDataTypes.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef DELTA_DELTAASTDATATYPES_H
#define DELTA_DELTAASTDATATYPES_H

#include "Common.h"

#include <vector>
#include <iosfwd>

namespace delta {

////////////////////////////////////////////////////////////////////////

enum ConstValueType {
	CONST_NUMBER,
	CONST_STRING,
};

////////////////////////////////////////////////////////////////////////

enum TableConstKeyType {
	CONST_KEY_NUMBER,
	CONST_KEY_STRING,
	CONST_KEY_DOTID,
	CONST_KEY_OPSTRING,
	CONST_KEY_IDENT,
	CONST_KEY_STRINGIFIED_ID,
};

////////////////////////////////////////////////////////////////////////

extern const char * DeltaASTNodeTypeStr	(int type);
extern const char * ConstValueTypeStr	(ConstValueType type);
extern const char * TableConstKeyTypeStr(TableConstKeyType type);

////////////////////////////////////////////////////////////////////////

struct Range {
	Range (uint left = 0, uint right = 0) : left(left), right(right) {}
	~Range (void) {}

	bool Contains (uint pos) const
		{ return left <= pos && right >= pos; }

	bool Contains (const Range& r) const
		{ return left <= r.left && right >= r.right; }

	bool Empty (void) const { return right == left; }
	uint Length (void) const { return right - left; }

	bool IsLeftOf (uint pos) const { return right < pos; }
	bool IsRightOf (uint pos) const { return left > pos; }

	bool IsLeftOf (const Range& r) const { return right < r.left; }
	bool IsRightOf (const Range& r) const { return left > r.right; }

	//******************************************************************

	bool operator== (const Range& r) const { return left == r.left && right == r.right; }
	bool operator!= (const Range& r) const { return left != r.left || right != r.right; }

	operator bool (void) const { return !this->Empty(); }

	//******************************************************************

	void Expand (const Range& r);
	void ExpandLeft (const Range& r);
	void ExpandRight (const Range& r);

	void Update (uint pos, int offset);

	//******************************************************************

	uint left;
	uint right;

private:
	inline uint applyOffset (uint value, uint pos, int offset);
};

////////////////////////////////////////////////////////////////////////

class Literal {
public:
	Literal (const char* symbol = 0) : m_symbol(symbol) {}

	operator const char* (void) const { return m_symbol; }
	operator const std::string (void) const { return m_symbol; }

	const std::string GetValue(void) const { return m_symbol; }

	size_t Size (void) const { return ::strlen(m_symbol); }

private:
	const char* m_symbol;
};

////////////////////////////////////////////////////////////////////////

class DeltaFunction {
public:
	DeltaFunction (void) {}
	DeltaFunction (const std::string& type, const std::string& name) :
		m_type(type), m_name(name) {}

	const std::string& GetName (void) const { return m_name; }
	const std::string& GetType (void) const { return m_type; }

private:
	std::string m_type;
	std::string	m_name;
};

////////////////////////////////////////////////////////////////////////

} // namespace delta

////////////////////////////////////////////////////////////////////////
// Stream funcs
//
inline std::ostream& operator<< (std::ostream& stream, const delta::Range& range)
	{ return stream << "(" << range.left << ", " << range.right << ")"; }


#endif // DELTA_DELTAASTDATATYPES_H
