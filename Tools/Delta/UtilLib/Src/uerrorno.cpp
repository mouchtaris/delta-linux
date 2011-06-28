// This is necessary because MS thread library
// really overrides the original implementation.
// A. Savidis, June 2001.
//

#ifdef	_MSC_VER
extern "C" {
	extern int errno;
}
int errno;
#endif