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
    chairs = new uCondition[noOfPhil];
    states = new int[noOfPhil];
    for (unsigned int i = 0; i < noOfPhil; i++){
        states[i] = 0;                                          // Initialize all states as not eating
    }
}                                                               

//----------------------------------------------------------------------
// Destructor 

Table::~Table(){
    delete [] chairs;
    delete [] states;
}


//----------------------------------------------------------------------
// pickup()

void Table::pickup( unsigned int id ){
    assert(!states[id]);                                        // If want to pick up, should be not eating
    if (
            !states[ left(id) ]
            &&
            !states[ right(id) ]
       ){
        states[id] = 1 ;
        assert(!states[left(id)] && !states[right(id)]);
    }
    else{
        printer.print(id, Philosopher::Waiting, id, right(id));
        chairs[id].wait(); 
        states[id] = 1;                                         // Waken up by neighbours
        assert(!states[left(id)] && !states[right(id)]);    
    }
}

void Table::putdown( unsigned int id ){
    states[id] = 0;                                             // Change states to not eating
    if ( !states[left(left(id))]                                // If left's left is not eating
            &&
            !chairs[left(id)].empty()){                         // If left is waiting

        chairs[left(id)].signal();                              // Wake up left if left can eat

    }
    else if ( !states[right(right(id))]                         // If right's right is not eating
            &&
            !chairs[right(id)].empty()){                        // If right is waiting

        chairs[right(id)].signal();                             // Wake up right if right can eat

    }
    else{                                                       // If both left and right will not eat, wake up whoever is waiting and capable to eat
        for (unsigned int i = 0; i < noOfPhil; i++){
            if( !states[left(i)] 
                    && 
                    !states[right(i)] 
                    &&
                    !chairs[i].empty()
              ){
                chairs[i].signal();
            }
        }
    } // else
} // putdown
