#include "q2table.h"
#include<iostream>
#include<stdlib.h>

using namespace std;

//======================================================================
//  Monitor Table 
//======================================================================


//----------------------------------------------------------------------
// Constructor

Table::Table( unsigned int noOfPhil, Printer &prt ) : noOfPhil(noOfPhil), printer(prt) {
    states = new int[noOfPhil];                                 
    for (unsigned int i = 0; i < noOfPhil; i++){
        states[i] = 0;                                                      // Initialize states as not eating
    }
}

Table::~Table(){
    delete [] states;
}


//----------------------------------------------------------------------
// pickup()

void Table::pickup( unsigned int id ){
    WAITUNTIL( (!states[left(id)] && !states[right(id)] ),                  // Wait until left and right are not eating
            printer.print(id, Philosopher::Waiting, id, right(id)), );
     
    states[id] = 1;                                                         // Update state to eating
    assert(!states[left(id)] && !states[right(id)]);

    RETURN();
}

void Table::putdown( unsigned int id ){
    states[id] = 0;                                                         // Update state to not eating

    RETURN();
}
