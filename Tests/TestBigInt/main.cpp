/**
 *	main.cpp
 *
 *	-- Stress Test of Key big int arithmetic --
 *	This is a stress test of the chord implementation of
 *	talkie application.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	December 2006.
 */
#include "Common.h"
#include "Streams.h"

#include "BigFixedInt.h"

#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>

#include <iostream>
#include <algorithm>

//-----------------------------------------------------------------------

enum { KEY_SIZE = 2 };
typedef ide::BigFixedInt<KEY_SIZE> Key;

int main(void)
{
	Key zero(0), two(200), three(300);
	std::cout /*<< two.Distance(zero) << " " << three.Distance(zero)*/
		<< " " << two.Distance(three) << std::endl;

	Key key(1000);
	key = key * Key(5);
	std::cout << key << std::endl;

	key = key - Key(20);
	std::cout << key << std::endl;
	std::cout << (key >> (uint32)2) << std::endl;
	std::cout << (key << (uint32)2) << std::endl;

	return 0;
}

//-----------------------------------------------------------------------
