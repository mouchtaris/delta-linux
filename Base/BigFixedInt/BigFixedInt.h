/**
 *	BigFixedInt.h
 *
 *	-- BigInt with fixed length library --
 *
 *	BigFixedInt is a class representing an integer of
 *	with a fixed length of bytes.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	January 2007.
 */
#ifndef IDE_BIG_FIXED_INT_H_
#define IDE_BIG_FIXED_INT_H_

#pragma warning(disable: 4267)

#include "Common.h"

#ifndef NDEBUG
#	include "Streams.h"
#endif //NDEBUG

#include <boost/array.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/casts.hpp>
#include <boost/lexical_cast.hpp>

#include <algorithm>
#include <string>

namespace ide
{
	//----------------------------
	//-- class BigFixedInt

	template <int IntLength>
	struct BigFixedInt
	{
		///--- type defintions
		enum { length = IntLength, };

		typedef boost::array<byte, IntLength> int_type;

		///--- constructors / destructor
		BigFixedInt(void)
			{ *this = ZERO; }
		BigFixedInt(const int_type& buf)
			: buffer(buf) {}
		BigFixedInt(const BigFixedInt& src)
			: buffer(src.buffer) {}

		explicit BigFixedInt(uint64 value) {
			std::copy(
				(byte*)&value,
				(byte*)(&value) + (IntLength < sizeof(value) ? IntLength : sizeof(value)),
				buffer.rbegin()
			);
		}

		template <class InputIterator>
		BigFixedInt(InputIterator first, InputIterator last) {
			assert(std::distance(first, last) == IntLength);
			std::copy(first, last, buffer.begin());
		}

		///--- overloaded operators
		const int_type& GetData(void) const		{ return buffer; }
		int_type& GetData(void)					{ return buffer; }

		const std::string str(void) const;
		uint32 to_long(void) const;
		BigFixedInt Distance(const BigFixedInt& right) const;	///< returns clock-wise distance of two ints on a ring

		///--- overloaded operators
		BigFixedInt& operator =(const BigFixedInt& right);
		bool operator ==(const BigFixedInt& right) const;
		bool operator !=(const BigFixedInt& right) const;
		bool operator <(const BigFixedInt& right) const;
		bool operator <=(const BigFixedInt& right) const;

		BigFixedInt operator +(const BigFixedInt& right) const;
		BigFixedInt operator -(const BigFixedInt& right) const;
		BigFixedInt operator *(uint32 digit) const;
		BigFixedInt operator *(const BigFixedInt& right) const;
		BigFixedInt operator ^(uint32 power) const;

		BigFixedInt& operator +=(const BigFixedInt& right);

		BigFixedInt operator <<(uint32 shifts) const;
		BigFixedInt operator >>(uint32 shifts) const;

		operator bool(void) const {
			return *this == ZERO;
		}

		///--- public static members
		static const BigFixedInt MAX;
		static const BigFixedInt ZERO;

	private:
		///--- Serialization Prerequisites
		friend class boost::serialization::access;

		template <typename Archive>
		void serialize(Archive& ar, const unsigned int version) {
			ar & buffer.elems;
		}

		///--- private members
		int_type buffer;
	};


	//---- Static Template Members ---------------------------//

	template <int IntLength> const BigFixedInt<IntLength> BigFixedInt<IntLength>::MAX = BigFixedInt(0xffffffff);	///< NOTE: dependent on size
	template <int IntLength> const BigFixedInt<IntLength> BigFixedInt<IntLength>::ZERO = BigFixedInt(0x00);			///< NOTE: dependent on size

	//---- Free Template Functions ---------------------------//

	template <int IntLength>
	std::ostream& operator <<(std::ostream& os, const BigFixedInt<IntLength>& bfi)
	{
		return os << bfi.str();
	}


	//---- Template Function Definitions --------------------//

	template <int IntLength>
	const std::string BigFixedInt<IntLength>::str(void) const
	{
		std::string str;
		for (uint32 i=0; i < buffer.size(); ++i)
			str += boost::lexical_cast<std::string>((uint32)(buffer[i])) + ".";
		return str;
	}

	//-----------------------------------------------------------------------

	template <int IntLength>
	uint32 BigFixedInt<IntLength>::to_long(void) const
	{
		BOOST_STATIC_ASSERT(IntLength == 4);

		uint32 value;
		std::copy(
			buffer.rbegin(),
			buffer.rend(),
			(byte*)&value
		);

		return value;
	}

	//-----------------------------------------------------------------------

	template <int IntLength>
	BigFixedInt<IntLength> BigFixedInt<IntLength>::Distance(const BigFixedInt<IntLength>& right) const
	{
#ifndef NDEBUG
		//if (*this <= right)
		//	TRACEOUT << *this << " - " << right << " = " << right - *this << std::endl;
		//else
		//	TRACEOUT << *this << " - " << right << " = " <<  BigFixedInt::MAX - (*this - right) << std::endl;
#endif //NDEBUG

		if (*this <= right)
			return right - *this;
		else
			return BigFixedInt<IntLength>::MAX - (*this - right);
	}

	//-----------------------------------------------------------------------

	template <int IntLength>
	BigFixedInt<IntLength>& BigFixedInt<IntLength>::operator =(const BigFixedInt<IntLength>& right)
	{
		buffer = right.buffer;
		return *this;
	}

	//-----------------------------------------------------------------------

	template <int IntLength>
	bool BigFixedInt<IntLength>::operator ==(const BigFixedInt<IntLength>& right) const {
		return (buffer == right.buffer);
	}

	//-----------------------------------------------------------------------

	template <int IntLength>
	bool BigFixedInt<IntLength>::operator !=(const BigFixedInt<IntLength>& right) const {
		return (buffer != right.buffer);
	}

	//-----------------------------------------------------------------------

	template <int IntLength>
	bool BigFixedInt<IntLength>::operator <(const BigFixedInt<IntLength>& right) const {
		uint32 i=0;
		while (i < buffer.size() && buffer[i] == right.buffer[i]) ++i;
		return (i == buffer.size()) ? false : buffer[i] < right.buffer[i];
	}

	//-----------------------------------------------------------------------

	template <int IntLength>
	bool BigFixedInt<IntLength>::operator <=(const BigFixedInt<IntLength>& right) const {
		uint32 i=0;
		while (i < buffer.size() && buffer[i] == right.buffer[i]) ++i;
		return (i == buffer.size()) ? true : buffer[i] < right.buffer[i];
	}

	//-----------------------------------------------------------------------

	template <int IntLength>
	BigFixedInt<IntLength> BigFixedInt<IntLength>::operator +(const BigFixedInt<IntLength>& right) const
	{
		BigFixedInt result;
		short carry=0;

		for (int32 i=buffer.size()-1; i>=0; --i)
		{
			carry = (short)buffer[i] + (short)right.buffer[i];
			result.buffer[i] = carry % 256;
			carry /= 256;
		}

		return result;
	}

	//-----------------------------------------------------------------------

	template <int IntLength>
	BigFixedInt<IntLength> BigFixedInt<IntLength>::operator -(const BigFixedInt<IntLength>& right) const
	{
		BigFixedInt result;
		short carry=0;

		for (int32 i=buffer.size()-1; i>=0; --i)
		{
			carry = (short)buffer[i] - (short)right.buffer[i] - ((carry < 0) ? 1 : 0);
			result.buffer[i] = (carry >= 0) ? carry : 256 + carry;
		}

		return result;
	}

	//-----------------------------------------------------------------------

	template <int IntLength>
	BigFixedInt<IntLength> BigFixedInt<IntLength>::operator *(uint32 digit) const
	{
		if (digit > 255)
			return *this * BigFixedInt<IntLength>(digit);

		BigFixedInt<IntLength> result;
		uint32 carry=0;

		for (int32 i=buffer.size()-1; i>=0; --i)
		{
			carry += (uint32)buffer[i] * digit;
			result.buffer[i] = carry % 256;
			carry /= 256;
		}

		return result;
	}

	//-----------------------------------------------------------------------

	template <int IntLength>
	BigFixedInt<IntLength> BigFixedInt<IntLength>::operator *(const BigFixedInt<IntLength>& right) const
	{
		BigFixedInt<IntLength> result;

		for (int32 i=buffer.size()-1; i>=0; --i)
		{
			result += *this * (uint32)right.buffer[i];
			result << (uint32)8;
		}

		return result;
	}

	//-----------------------------------------------------------------------

	template <int IntLength>
	BigFixedInt<IntLength> BigFixedInt<IntLength>::operator ^(uint32 power) const
	{
		BigFixedInt<IntLength> result = *this;

		if (power == 0)
			return BigFixedInt<IntLength>(1);

		for (uint32 i=1; i < power; ++i)
			result = result * (*this);
		return result;
	}

	//-----------------------------------------------------------------------

	template <int IntLength>
	BigFixedInt<IntLength>& BigFixedInt<IntLength>::operator +=(const BigFixedInt<IntLength>& right)
	{
		short carry=0;

		for (int32 i=buffer.size()-1; i>=0; --i)
		{
			carry = (short)buffer[i] + (short)right.buffer[i];
			buffer[i] = carry % 256;
			carry /= 256;
		}

		return *this;
	}

	//-----------------------------------------------------------------------

	template <int IntLength>
	BigFixedInt<IntLength> BigFixedInt<IntLength>::operator <<(uint32 shifts) const
	{
		BigFixedInt<IntLength> result = *this;
		uint32 carry=0, tmp;

		for (uint32 k=0; k < shifts; ++k)
			for (int32 i=buffer.size()-1; i>=0; --i)
			{
				tmp = (result.buffer[i] & 0x80) ? 1 : 0;
				result.buffer[i] = (result.buffer[i] << 1) + carry;
				carry = tmp;
			}

		return result;
	}

	//-----------------------------------------------------------------------

	template <int IntLength>
	BigFixedInt<IntLength> BigFixedInt<IntLength>::operator >>(uint32 shifts) const
	{
		BigFixedInt<IntLength> result = *this;
		uint32 carry=0, tmp;

		for (uint32 k=0; k < shifts; ++k)
			for (uint32 i=0; i < buffer.size(); ++i)
			{
				tmp = (result.buffer[i] & 0x01) ? 0x80 : 0;
				result.buffer[i] = (result.buffer[i] >> 1) + carry;
				carry = tmp;
			}

		return result;
	}

	//-----------------------------------------------------------------------
}

#endif	//IDE_BIG_FIXED_INT_H_
