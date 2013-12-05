//======================================================================
//  q2phil Driver
//======================================================================

#include "q2table.h"
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
	     << " [ philosophers (> 1) "
         << "[ noodles (> 0) "
         << "[ Seed (> 0) "
         << "] ] ]" << endl; 
    exit( EXIT_FAILURE );				            // TERMINATE
} // usage

//----------------------------------------------------------------------
// uMain driver
//
void uMain::main(void) {

    int philosopherNum = 5;                             // Default value of number of philosopher
    int noodleNum = 30;                                 // Default value of number of producers 
    int seed = getpid();                                // Default value of seed
    
    switch ( argc ) {
        case 4:
            if ( ! convert ( seed, argv[3] ) || seed <= 0 ) {
                usage ( argv );
            }
            // FALL THROUGH
        case 3:
            if ( ! convert ( noodleNum, argv[2] ) || noodleNum <= 0){
                usage ( argv );
            }
            // FALL THROUGH
        case 2:
            if ( ! convert ( philosopherNum, argv[1] ) || philosopherNum <= 1) {
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

    Printer printer(philosopherNum);
    Table table(philosopherNum, printer);
    Philosopher *philosophers[philosopherNum];

    for (int i = 0; i < philosopherNum; i++){                       // start threads
        philosophers[i] = new Philosopher(i, noodleNum, table, printer);
    }
    
    for (int i = 0; i < philosopherNum; i++){                       // wait for threads to finish
        delete philosophers[i];
    }
}
