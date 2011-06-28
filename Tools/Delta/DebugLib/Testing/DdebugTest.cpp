#include "ddebug.h"
#include "usystem.h"
#include <string>

static FILE* fp;
static void onerror (const char* s) {
	fprintf(fp, "%s\n", s);
}

int main (int argc, char** argv) {

	fp = fopen("derror.txt", "wt");
	dinit(onerror);
	DSTARTLOGGING("memorylog.txt");
	unsigned* pi = DNEWARR(unsigned, 10);
	std::string* ps = DNEWARR(std::string, 5);
	DLATEDISPOSALON(1000);
	delete[] ps;
	delete[] pi;
//	pi[5] = 32;
	uprocesssleep(2000);
	DSTOPLOGGING();
	dclose();
	return 0;
}

/*
//	DLATEDISPOSALON(1000);
	//	delete[] ps;
	//	delete[] pi;
	
	// pi[0] = 0xffffffff;
	//DPTR(pi)[0] = 30;

	dclose();
	fclose(fp);

	uprocesssleep(1000);
	return 0;
*/