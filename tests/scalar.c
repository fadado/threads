// Scalars test
// gcc -Wall -O2 -I. tests/scalar.c

#include <stdio.h>

#define DEBUG
#include "scalar.h"

int main(int argc, char** argv)
{
////////////////////////////////////////////////////////////////////////

	Scalar s1, s2;

	int i1, i2;
	s1 = coerce(-777);			// assign literal int to Scalar
	s2 = s1;					// assign (copy) Scalar to Scalar
	s2 = (Scalar)s1;			// assign (copy) Scalar to Scalar
	i1 = cast(s1, int);			// restore int from Scalar
	i2 = cast(s1, int);			// restore int from Scalar
	assert(i1 == i2);
	assert(i1 == s1.integer);
	assert(i1 == s2.integer);
	//
	s1 = (Scalar)-777LL;		// assign literal int to Scalar
	s2 = s1;					// assign (copy) Scalar to Scalar
	i1 = cast(s1, int);			// restore integer from Scalar
	i2 = cast(s1, int);			// restore integer from Scalar
	assert(i1 == i2);
	assert(i1 == s1.integer);
	assert(i1 == s2.integer);

	unsigned u1, u2;
	s1 = (Scalar)777ULL;		// assign literal unsigned to Scalar
	s2 = s1;					// assign (copy) Scalar to Scalar
	u1 = cast(s1, unsigned);	// restore unsigned from Scalar
	u2 = cast(s1, unsigned);	// restore unsigned from Scalar
	assert(u1 == u2);
	assert(u1 == s1.integer);
	assert(u1 == s2.integer);

	unsigned w1;
	s1 = coerce(0x1000000U);	// assign literal unsigned to Scalar
	s2 = s1;					// assign (copy) Scalar to Scalar
	w1 = cast(s1, unsigned);	// restore word from Scalar
	assert(w1 == s1.word);
	assert(w1 == s2.word);

	double d1, d2;
	s1 = coerce(123.456);		// assign literal double to Scalar
	s2 = (Scalar)123.456e0;		// assign literal double to Scalar
	d1 = cast(s1, double);		// restore integer from Scalar
	d2 = cast(s2, double);		// restore integer from Scalar
	assert(d1 == d2);
	assert(d1 == s1.real);
	assert(d1 == s2.real);

////////////////////////////////////////////////////////////////////////

	int *p1;
	i1 = 123456;
	p1 = &i1;
	s1 = (Scalar)(void*)p1;		// assign pointer to scalar
	p1 = cast(s1, int*);
	assert(p1 == s1.pointer);

	int *p2;
	i1 = 123456;
	p2 = &i1;
	s1 = COERCE(p2);			// assign pointer to scalar
	p2 = cast(s1, int*);
	assert(p2 == s1.pointer);
	s1 = COERCE(0);				// assign pointer to scalar
	p2 = cast(s1, int*);
	assert(p2 == s1.pointer);

////////////////////////////////////////////////////////////////////////

	s1 = (Scalar)(Integer)0;
	s1 = (Scalar)(Word)0x0;
	s1 = (Scalar)(Real)0e0;
	s1 = (Scalar)(Pointer)NULL;

#define Integer(x) (Scalar)(Integer)(x)
#define Word(x) (Scalar)(Word)(x)
#define Real(x) (Scalar)(Real)(x)
#define Pointer(x) (Scalar)(Pointer)(x)

	s1 = Integer(0);
	s1 = Word(0x0);
	s1 = Real(0e0);
	s1 = Pointer(00);

	return 0;
}

// vim:ai:sw=4:ts=4:syntax=cpp
