// uvirtualclass.h
// Virtual class emulation support.
// ScriptFighter Project.
// A. Savidis, July 2006.
//

#ifndef	UVIRTUALCLASS_H
#define	UVIRTUALCLASS_H

#include "DDebug.h"
#include "utypes.h"
#include "ufunctors.h"

////////////////////////////////////////////////////////
// Covariant return types for virtual functions are needed.
//
#define	UVIRTUAL_CLASS(C)									\
	virtual C*			New##C (void) const					\
							{ return DNEW(C); }				\
	virtual C&			New##C (void* mem) const			\
							{ return *(new (mem) C); }		\
	virtual unsigned	SizeOf##C (void) const				\
							{ return sizeof(C); }
		
#define	UVIRTUAL_NEW(p, C) \
			utoref(p).New##C();

#define	UVIRTUAL_INSTANCE(p, C, id)							\
	uvirtualinstallocator mem##id(utoref(p).SizeOf##C());	\
	C& id = utoref(p).New##C(mem##id.GetBuffer());			\
	uvirtualinstdestructor<C> destructor##id(id);

//*************************

template <class B> class uvirtualinstdestructor {
	private:
	B& b;

	public:
	uvirtualinstdestructor (B& _b) : b(_b){}
	~uvirtualinstdestructor() { b.~B(); }
};

//*************************

class UTILLIB_CLASS uvirtualinstallocator {
	private:
	util_ui8* buffer;

	public:
	void* GetBuffer (void) { return buffer; }
	uvirtualinstallocator (util_ui32 size) 
		{ buffer = DNEWARR(util_ui8, size); }
	~uvirtualinstallocator() { DDELARR(buffer); }
};

//---------------------------------------------------------------
// A SAMLPE TEST.
/*
class A {
	public:

	class B { 
		public: 
			B(void) { printf("A::B()\n"); }
			virtual ~B() { printf("A::~B()\n"); }
		};
	UVIRTUAL_CLASS(B)
};

class AA : public A {
	public:
	class B : public A::B {
		public: 
			B(void) { printf("AA::B()\n"); }
			virtual ~B() { printf("AA::~B()\n"); }
		};
	UVIRTUAL_CLASS(B)
};

int main (int argc, char* argv[]) {

	A a;
	A::B* abp = UVIRTUAL_NEW(a, B);

	AA aa;
	UVIRTUAL_INSTANCE(aa, B, aabr);
}
*/

//---------------------------------------------------------------

#endif	// Do not add stuff beyond this point.

