//======================================================================
//  q2northpole Driver
//======================================================================

#include "q2northpole.h"
#include <cstring>
#include <cerrno>
#include <iostream>
#include <string>
#include <sstream>
using namespace std;
#include <cstdlib>

//----------------------------------------------------------------------
// Declare prng
//
PRNG prng;

//----------------------------------------------------------------------
// Convert arguments to integer
//
bool convert( int &val, const char *buffer ) {		// convert C string to integer
    stringstream ss( buffer );			            // connect stream and buffer
    ss >> dec >> val;					            // convert integer from buffer
    return ! ss.fail() &&				            // conversion successful ?

	// characters after conversion all blank ?
	string( buffer ).find_first_not_of( " ", ss.tellg() ) == string::npos;
}   // convert

//----------------------------------------------------------------------
// Usage message
//

void usage( char *argv[] ) {
    cerr << "Usage: " << argv[0]
	     << " [ N (> 0) "
         << "[ E (> 0) "
         << "[ Seed (> 0) "
         << "[ C (>= 0) "
         << "[ D (>= 0) ] ] ] ] ]" << endl; 
    exit( EXIT_FAILURE );				            // TERMINATE
} // usage

//----------------------------------------------------------------------
// uMain driver
//
void uMain::main(void) {

    int boundReindeer = 3;                              // Default value of number of consumers
    int numElves = 3;                              // Default value of number of producers 
    int seed = getpid();                            // Default value of seed
    int numConsult = 3;
    int numDeliveries = 3;
    
    switch ( argc ) {
        case 6:
            if ( ! convert ( numDeliveries, argv[5] ) || numDeliveries < 0 ) {
                usage ( argv );
            }
            // FALL THROUGH
        case 5:
            if ( ! convert ( numConsult, argv[4] ) || numConsult < 0 ) {
                usage ( argv );
            }
            // FALL THROUGH
        case 4:
            if ( ! convert ( seed, argv[3] ) || seed <= 0 ) {
                usage ( argv );
            }
            // FALL THROUGH
        case 3:
            if ( ! convert ( numElves, argv[2] ) || numElves <= 0){
                usage ( argv );
            }
            // FALL THROUGH
        case 2:
            if ( ! convert ( boundReindeer, argv[1] ) || boundReindeer<= 0) {
                usage ( argv );
            }
            // FALL THROUGH
        case 1:
            break;
        default:						                            // wrong number of options
            usage( argv );
    } // switch
    
    // Initialize prng seed
    prng.seed(seed);

    Printer printer(numElves);
    Workshop workshop(printer, numElves, boundReindeer, numDeliveries);

    Santa *santa = new Santa(workshop, printer);
    Elf *elfs[numElves];
    Reindeer *reindeers[Reindeer::MAX_NUM_REINDEER];

    for (int i = 0; i < numElves; i++){                             // start threads
        elfs[i] = new Elf(i+1, workshop, printer, numConsult);
    }
    
    for (unsigned int i = 0; i < Reindeer::MAX_NUM_REINDEER; i++){  // start threads
        reindeers[i] = new Reindeer(i+numElves+1, workshop, printer, numDeliveries);
    }

    for (int i = 0; i < numElves; i++){                             // wait for threads to finish
        delete elfs[i];
    }
    
    for (unsigned int i = 0; i < Reindeer::MAX_NUM_REINDEER; i++){  // wait for threads to finish
        delete reindeers[i];
    }

    delete santa;
}
