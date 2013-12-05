#include <uC++.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <limits>					// numeric_limits
#include <cstdlib>					// exit
using namespace std;

#include "config.h"					// YOUR INCLUDES FILES
#include "MPRNG.h"
#include "Printer.h"
#include "Bank.h"
#include "Parent.h"
#include "WATCardOffice.h"
#include "NameServer.h"
#include "VendingMachine.h"
#include "BottlingPlant.h"
#include "Student.h"

MPRNG mprng;



bool convert( int &val, char *buffer ) {		// convert C string to integer
    std::stringstream ss( buffer );			// connect stream and buffer
    ss >> dec >> val;					// convert integer from buffer
    return ! ss.fail() &&				// conversion successful ?
	// characters after conversion all blank ?
	string( buffer ).find_first_not_of( " ", ss.tellg() ) == string::npos;
} // convert

void usage( char *argv[] ) {
    cerr << "Usage: " << argv[0] << " [ config-file [ random-seed (>0)] ]" << endl;
    exit( -1 );
} // usage


/***********************************************
******************uMain::main*******************
************************************************/
void uMain::main(){
    const char *configFile = "soda.config";
    ConfigParms pm;
    //unsigned int i;
    int seed = getpid();					// seed for random number generator

    switch ( argc ) {
      case 3:
		if ( ! convert( seed, argv[2] ) || seed <= 0 ) usage( argv ); // invalid ?
		// FALL THROUGH
      case 2:
		configFile = argv[1];
		// FALL THROUGH
      case 1:						// all defaults
		break;
      default:						// wrong number of options
	usage( argv );
    } // switch
	
	mprng.seed( seed );				// seed the random number generator
    processConfigFile( configFile, pm );		// process configuration file
    
	
	// printer and bank monitor declaration
	Printer prt( pm.numStudents, pm.numVendingMachines, pm.numCouriers );
	Bank bank( pm.numStudents );
	
	
	// tasks delcaration
	Parent *parent = new Parent( prt, bank, pm.numStudents, pm.parentalDelay );
	WATCardOffice *cardOffice = new WATCardOffice( prt, bank, pm.numCouriers );
	NameServer *nameServer = new NameServer( prt, pm.numVendingMachines, pm.numStudents );
	VendingMachine *Vlist[pm.numVendingMachines];
	for(unsigned int i=0; i<pm.numVendingMachines; i+=1){
		Vlist[i] = new VendingMachine( prt, *nameServer, i, pm.sodaCost, pm.maxStockPerFlavour );
	}
	BottlingPlant *plant = new BottlingPlant( prt, *nameServer, pm.numVendingMachines, pm.maxShippedPerFlavour, 
											pm.maxStockPerFlavour, pm.timeBetweenShipments );
	Student *Slist[pm.numStudents];
	for(unsigned int i=0; i<pm.numStudents; i+=1){
		Slist[i] = new Student( prt, *nameServer, *cardOffice, i, pm.maxPurchases );
	}
	
	
	// delete tasks
	for(unsigned int i=0; i<pm.numStudents; i+=1){
		delete Slist[i];
	}
	delete plant;
	for(unsigned int i=0; i<pm.numVendingMachines; i+=1){
		delete Vlist[i];
	}
	delete nameServer;
	delete cardOffice;
	delete parent;
} // uMain::main


// Local Variables: //
// compile-command: "make" //
// End: //