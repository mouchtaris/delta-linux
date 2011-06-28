/**
 *	Encoding.h
 *
 *	-- Encoding Base --
 *
 *	The encoder/decoder pair classes provides
 *	a serialization mechanism on top of
 *	boost::Searialize.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	January 2007
 */
#ifndef COMM_ENCODING_H_
#define COMM_ENCODING_H_

#include "Common.h"
#include "Serialization.h"
#include "LibraryAPIDecls.h"

#include <string>
#include <sstream>
#include <iomanip>
#include <list>

namespace comm
{
	//----------------------------
	//-- struct encoder

	class _BASE_API encoder
	{
	public:
		///--- constructors / destructor
		encoder(void) {}

		///--- public API
		template <typename T>
		encoder& encode(const T& t) {
			archives.push_back(do_encode(t));
			return *this;
		}
		template <typename T>
		encoder& operator <<(const T& t) { return encode(t); }

		operator const std::string(void) const	{ return do_encode(archives); }
		const std::string str(void) const		{ return do_encode(archives); }

	private:
		///--- private API
		template <typename T>
		const std::string do_encode(const T& t) const {
			std::ostringstream archive_stream;
			boost::archive::text_oarchive archive(archive_stream);
			archive << t;
			return archive_stream.str();
		}

		///--- private members
		typedef std::list<std::string> string_list;
		string_list archives;
	};


	//----------------------------
	//-- struct decoder

	class _BASE_API decoder
	{
	public:
		///--- constructors / destructor
		decoder(const std::string& str) {
			do_decode(str, archives);
		}

		///--- public API
		template <typename T>
		decoder& decode(T& t) {
			const std::string& str = archives.front();
			do_decode(str, t);
			archives.pop_front();
			return *this;
		}
		template <typename T>
		decoder& operator >>(T& t) { return decode(t); }


	private:
		///--- private API
		template <typename T>
		void do_decode(const std::string& str, T& t) const {
			std::istringstream archive_stream(str);
			boost::archive::text_iarchive archive(archive_stream);
			archive >> t;
		}

		///--- private members
		typedef std::list<std::string> string_list;
		string_list archives;
	};


	//----------------------------
	//-- function encode_tuple

	template <class Tuple, int N>
	struct encode_tuple_t {					
		void operator ()(encoder& enc, const Tuple& t) const {
			enum { index = boost::tuples::length<Tuple>::value - N };
			enc << boost::get<index>(t);
			encode_tuple_t<Tuple, N-1>()(enc, t);
		}
	};
	template <class Tuple>
	struct encode_tuple_t<Tuple, 0>	{
		void operator ()(encoder& enc, const Tuple& t) const {}
	};

	template <class Tuple>
	encoder& encode_tuple(encoder& enc, const Tuple& t) {	///< auxiliary convenience function for encoding to tuple
		encode_tuple_t<Tuple, boost::tuples::length<Tuple>::value>()(enc, t);
		return enc;
	}


	//----------------------------
	//-- function decode_tuple

	template <class Tuple, int N>
	struct decode_tuple_t {					
		void operator ()(decoder& dec, Tuple& t) const {
			enum { index = boost::tuples::length<Tuple>::value - N };
			dec >> boost::get<index>(t);
			decode_tuple_t<Tuple, N-1>()(dec, t);
		}
	};
	template <class Tuple>
	struct decode_tuple_t<Tuple, 0>	{
		void operator ()(decoder& dec, Tuple& t) const {}
	};

	template <class Tuple>
	Tuple& decode_tuple(decoder& dec, Tuple& t) {	///< auxiliary convenience function for decoding to tuple
		decode_tuple_t<Tuple, boost::tuples::length<Tuple>::value>()(dec, t);
		return t;
	}
}

#endif	//COMM_ENCODING_H_
