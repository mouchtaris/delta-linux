/**
 *	Portability.h
 *
 *	--- Util ---
 *
 *  Providing an abstraction layer for the Sparrow
 *  building platform (win32/linux, as well as various
 *  versions of the Visual C++ compiler, boost library,
 *  and the WX Widget library).
 * 
 *  Nikos Mouchtaris <muhtaris@ics.forth.gr>
 *	September 2010
 */

#ifndef UTIL_PORTABILITY_H
#define UTIL_PORTABILITY_H

#include <boost/version.hpp>
#include <boost/checked_delete.hpp>
#include <wx/version.h>

/////////////////////////////////////////////////////////////////////////
//                          Compiler Specific
/////////////////////////////////////////////////////////////////////////
// Unused parameters
#define PORT_UNUSED_PARAM(x)

/////////////////////////////////////////////////////////////////////////
//                                WX
// Acquiring a C-string from a wx::string
#if		wxMAJOR_VERSION == 2 && wxMINOR_VERSION == 8 && wxRELEASE_NUMBER < 11
#	define	WX_C_STR	str
#else
#	define	WX_C_STR	c_str
#endif

#if		wxMAJOR_VERSION == 2 && wxMINOR_VERSION == 9
#	define	WX_DATE_TIME_PARSE_DATE(str)	ParseDate(str)
#	define	OFSTREAM_WXSTRING_INPUT(str)	str.c_str().AsChar()
#else
#	define	WX_DATE_TIME_PARSE_DATE(str)	ParseDate(str.c_str())
#	define	OFSTREAM_WXSTRING_INPUT(str)	str.c_str()
#endif

//


/////////////////////////////////////////////////////////////////////////
//                              Boost
/////////////////////////////////////////////////////////////////////////
namespace portability {
	namespace boost {

		// Reproduce boost::checked_delete() in a non-in-line function.
		// (warning C4396: 'boost::checked_delete' : the inline specifier cannot be used when a friend declaration refers to a specialization of a function template	...\base\timers\delayedcaller.h	24
		template<class T> void checked_delete(T* const x) {
			// intentionally complex - simplification causes regressions
			typedef char type_must_be_complete[ sizeof(T)? 1: -1 ];
			(void) sizeof(type_must_be_complete);
			delete x;
		}

	} // namespace boost
} // namespace portability

#if (BOOST_VERSION / 100000) == 1 && (BOOST_VERSION / 100 % 1000) == 34
///////////////////////////////////////////////////
// Using Boost 1.34 (OLD)
#define BOOST_1_34
// Forward declare boost asio error as it used to be in FwdDeclarations
namespace boost { namespace asio { class error; } }
typedef boost::asio::error boost_asio_error_t;
#define BOOST_ASIO_IO_SERVICE_INTERRUPT(IO_SERVICE)					IO_SERVICE.interrupt()
#define BOOST_ASIO_ERROR_MAKE_ASIO_ERROR(ERROR)						boost::asio::error(ERROR)
#define BOOST_ASIO_ERROR_DEFINE_ASIO_ERROR_VARIABLE(VARNAME, ERROR)	boost_asio_error_t VARNAME(ERROR)
#define BOOST_ASIO_ERROR_NOT_EQUAL_TO_OPERATION_ABORTED(ERROR)		( (ERROR) != boost_asio_error_t::operation_aborted)
#define BOOST_ASIO_ERROR_MAKE_INVALID_ARGUMENT_ERROR				boost::asio::error(boost::asio::error::invalid_argument)
#define BOOST_MULTI_INDEX_CONTAINER_CLEAR(CONTAINER)				CONTAINER.clear_()
#define BOOST_MULTI_INDEX_CONTAINER_EMPTY(CONTAINER)				CONTAINER.empty_()
#define BOOST_ASIO_ASSIGN_ERROR(ERROR)								boost::asio::assign_error(ERROR)
#define BOOST_ASIO_ASSIGN_ERROR_HANDLER(ERROR)						boost::asio::assign_error(ERROR)
#define BOOST_SPIRIT_CLASSIC_GRAMMAR(TYPE)							boost::spirit::grammar<TYPE>
#define BOOST_SPIRIT_CLASSIC_RULE(TYPE)								boost::spirit::rule<TYPE>
#define BOOST_SPIRIT_CLASS_PARSE_INFO(TYPE)							boost::spirit::parse_info<TYPE>
#define BOOST_SPIRIT_CLASSIC_PARSE									boost::spirit::parse
#define BOOST_SPIRIT_CLASSIC_NAMESPACE								boost::spirit
#define BOOST_FILESYSTEM_DIRECTORY_METHOD							directory_string
#define BOOST_SOCKET_IO_SERVICE										io_service

// and tuple serialisation provided by boost 
#include <boost/serialization/tuple.hpp>

#elif (BOOST_VERSION / 100000) == 1 && (BOOST_VERSION / 100 % 1000) >= 45
///////////////////////////////////////////////////
// Using Boost 1.45 or higher (New -- Sparrow not written according to this version)
#include <boost/system/error_code.hpp>
#include <boost/tuple/tuple.hpp>

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
//
// Error handling was changed in revision 35911 in order to comply
// to the TR2 proposal.
typedef boost::system::error_code boost_asio_error_t;
//
// clear_() and empty_() in multi-index-containers *might* be public:
// with some compilers (or older boost versions), but they are mainly
// protected:. clear() and empty() are always public:.
#define BOOST_MULTI_INDEX_CONTAINER_CLEAR(CONTAINER)	CONTAINER.clear()
#define BOOST_MULTI_INDEX_CONTAINER_EMPTY(CONTAINER)	CONTAINER.empty()
//
// boost::asio::service_interrupt() was rename to stop()
#define BOOST_ASIO_IO_SERVICE_INTERRUPT(IO_SERVICE)													\
		IO_SERVICE.stop()
#define BOOST_ASIO_ERROR_MAKE_ASIO_ERROR(ERROR)														\
		boost::asio::error::make_error_code(ERROR)
#define BOOST_ASIO_ERROR_DEFINE_ASIO_ERROR_VARIABLE(VARNAME, ERROR)									\
		boost_asio_error_t VARNAME(BOOST_ASIO_ERROR_MAKE_ASIO_ERROR(ERROR))
#define BOOST_ASIO_ERROR_NOT_EQUAL_TO_OPERATION_ABORTED(ERROR)										\
	(	(ERROR) == BOOST_ASIO_ERROR_MAKE_ASIO_ERROR(boost::asio::error::operation_aborted)			\
	)
#define BOOST_ASIO_ERROR_MAKE_INVALID_ARGUMENT_ERROR												\
		BOOST_ASIO_ERROR_MAKE_ASIO_ERROR(boost::asio::error::invalid_argument)
//
// Error handlers were removed in revision 35911 in order to
// change the error handling according to the TR2 proposal.
//
// All boost::asio calls that used to accept an error_handler, now
// accept a system_error directly, so no handler creation is
// necessary.
#define BOOST_ASIO_ASSIGN_ERROR(ERROR)		ERROR
// However, all Sparrow calls that used to accept boost::asio::error_hanlder-s
// still do. Therefore, code copy-paste is required, as below:
namespace portability {
	namespace boost {
		namespace asio {
			template <typename Target>
			class assign_error_t {
			public:
				typedef void result_type;
				assign_error_t (Target& target): target_(&target) { }

				template <typename Error> void operator () (const Error& err) const
					{ *target_ = err; }
			private:
				Target* target_;
			};
			template <typename Target>
			inline assign_error_t<Target> assign_error(Target& target)
				{ return assign_error_t<Target>(target); }
		} // namespace asio
	} // namespace boost
} // namespace portability
#define BOOST_ASIO_ASSIGN_ERROR_HANDLER(TARGET)	::portability::boost::asio::assign_error(TARGET)
//
// Spirit seems to have had an update and now the 1.34 (old) spirit
// is called spirit::classic
#define BOOST_SPIRIT_CLASSIC_GRAMMAR(TYPE)		boost::spirit::classic::grammar<TYPE>
#define BOOST_SPIRIT_CLASSIC_RULE(TYPE)			boost::spirit::classic::rule<TYPE>
#define BOOST_SPIRIT_CLASS_PARSE_INFO(TYPE)		boost::spirit::classic::parse_info<TYPE>
#define BOOST_SPIRIT_CLASSIC_PARSE				boost::spirit::classic::parse
#define BOOST_SPIRIT_CLASSIC_NAMESPACE			boost::spirit::classic
//
#define BOOST_FILESYSTEM_DIRECTORY_METHOD		string
//
#define BOOST_SOCKET_IO_SERVICE					get_io_service

// We have to explicitly serialise tuple
namespace boost {
	namespace serialization {
		template <typename Archive, typename T0, typename T1, typename T2>
		static void inline serialize (Archive& ar, boost::tuple<T0,T1,T2>& t, const unsigned int version)
			{ ar & t.get<0>(); ar & t.get<1>(); ar & t.get<2>(); }
		template <typename Archive, typename T0, typename T1, typename T2, typename T3>
		static void inline serialize (Archive& ar, boost::tuple<T0,T1,T2,T3>& t, const unsigned int version)
			{ ar & t.get<0>(); ar & t.get<1>(); ar & t.get<2>(); ar & t.get<3>(); }
		template <typename Archive, typename T0, typename T1, typename T2, typename T3, typename T4>
		static void inline serialize (Archive& ar, boost::tuple<T0,T1,T2,T3,T4>& t, const unsigned int version)
			{ ar & t.get<0>(); ar & t.get<1>(); ar & t.get<2>(); ar & t.get<3>(); ar & t.get<4>(); }
	} // namespace serialization
} // namespace boost

#else
#error "Unknown boost version"
#endif // Boost version case


#endif // UTIL_PORTABILITY_H
