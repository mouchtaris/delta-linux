/**
 *	main.cpp
 *
 *	-- Stress Test of ChordDHT Library --
 *	This is a stress test of the chord DHT implementation
 *	for talkie application.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	January 2006.
 */
#include "Common.h"
#include "Demos.h"

#include <ctime>
#include <iostream>

int main(void)
{
	//try {
		srand((unsigned int)time(0));

		RunSampleRequestsDemo();
		//RunFetchKeysDemo();
	//}
	//catch (std::exception& e) {
	//	std::cerr << "Runtime Error: " << e.what() << std::endl;
	//}

	return 0;
}

//-----------------------------------------------------------------------
