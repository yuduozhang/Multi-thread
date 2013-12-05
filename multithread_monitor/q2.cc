//======================================================================
//  q2tallyVotes Driver
//======================================================================

#include "q2tallyVotes.h"
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
	     << " [ Voters (> 0 & V mod G = 0, default 6) "
         << "[ Group (> 0 & odd, default 3) [ Seed (> 0) ] ] ]" << endl; 
    exit( EXIT_FAILURE );				            // TERMINATE
} // usage

//----------------------------------------------------------------------
// uMain driver
//
void uMain::main(void) {

    int sizeVoter = 6;                              // Default value of number of consumers
    int sizeGroup = 3;                              // Default value of number of producers 
    int seed = getpid();                            // Default value of seed
    
    switch ( argc ) {
        case 4:
            if ( ! convert ( seed, argv[3] ) || seed <= 0 ) {
                usage ( argv );
            }
            // FALL THROUGH
        case 3:
            if ( ! convert ( sizeGroup, argv[2] ) 
                    || 
                sizeGroup <= 0 
                    ||
                sizeGroup%2 == 0
                    ) {
                usage ( argv );
            }
            // FALL THROUGH
        case 2:
            if ( 
                    ! convert ( sizeVoter, argv[1] ) 
                    || 
                    sizeVoter <= 0
                    ||
                    sizeVoter%sizeGroup != 0
                ) {
                usage ( argv );
            }
            // FALL THROUGH
        case 1:
            break;
        default:						            // wrong number of options
            usage( argv );
    } // switch
    
    // Initialize prng seed
    prng.seed(seed);

    Printer printer(sizeVoter);
    TallyVotes voteTallier(sizeGroup, printer);
    Voter *voters[sizeVoter]; 

    for (int i = 0; i < sizeVoter; i++){           // start threads
        voters[i] = new Voter(i, voteTallier, printer);
    }
    
    for (int i = 0; i < sizeVoter; i++){           // wait for threads to finish
        delete voters[i];
    }
}
