#ifndef _MPRNG_H_
#define _MPRNG_H_

#include <uC++.h>
using namespace std;					// direct access to std
#include "PRNG.h"					// PRNG


_Monitor MPRNG : private PRNG {				// wrapper PRNG to make it thread safe 
    public:
	MPRNG( uint32_t  s = 362436069 ) : PRNG( s ) {}
	uint32_t seed() {				// read seed
		return PRNG::seed();
	}
	void seed( uint32_t  s ) {			// reset seed
		PRNG::seed( s );
	}
	uint32_t operator()() {			// [0,UINT_MAX]
		return PRNG::operator()();
	}
	uint32_t operator()( uint32_t u ) {		// [0,u]
		return PRNG::operator()( u );
	}
	uint32_t operator()( uint32_t l, uint32_t u ) {// [l,u]
		return PRNG::operator()( l, u );
	}
};


#endif	/* _MPRNG_H_ */