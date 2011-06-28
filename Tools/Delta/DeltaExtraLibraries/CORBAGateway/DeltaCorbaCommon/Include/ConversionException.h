#ifndef CONVERSION_EXCEPTION_H
#define CONVERSION_EXCEPTION_H

#include <exception>

class CORBACOMMON_CLASS ConversionException : public std::exception {
public:
	ConversionException(const std::string& msg) : std::exception(msg.c_str()) {}
};

#endif