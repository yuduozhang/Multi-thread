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
    chairs = new uCondition[noOfPhil];
    for (unsigned int i = 0; i < noOfPhil; i++){
        states[i] = 0;                                                  // Initialize all states as not eating
    }
}

Table::~Table(){
    delete [] states;
    delete [] chairs;
}

//----------------------------------------------------------------------
// main()
void Table::main(){
    for ( ;; ){
        _Accept( ~Table )                                               // Accept destructor
            break;
        or _Accept(putdown){
            states[currentID] = 0;                                      // Update state to not eating
            if (!states[left(left(currentID))]                          // If left's left is not eating
                    && 
                    !chairs[left(currentID)].empty()) {                 // and left is waiting
                currentID = left(currentID);                            // Update current id to left's id so that after being waken up, left can update its state
                chairs[currentID].signal();                             // Wake up left
                server.wait();                                          // Signal block
            }
            else if (!states[right(right(currentID))]                   // If right's right is not eating
                    &&                  
                    !chairs[right(currentID)].empty()) {                // and right is waiting
                currentID = right(currentID);                           // Update current id to right's id so that after being waken up, right can update its state
                chairs[currentID].signal();                             // Wake up right
                server.wait();                                          // Signal block
            }
            else {
                for (unsigned int i = 0; i < noOfPhil; i++){            // If neight left and right will eat, wake up whoever is waiting and capable to eat
                    if( !states[left(i)] 
                            && 
                        !states[right(i)] 
                            &&
                        !chairs[i].empty()
                      ){
                        currentID = i;                                  // Update current id to the waking up philosopher's id so that after being waken up,
                        chairs[i].signal();                             // he can update his state
                        server.wait();                                  // Signal block
                    }
                }
            }
        } or _Accept(pickup){
            if (!states[left(currentID)] && !states[right(currentID)]){ // If both left and right are not eating
                if (!chairs[currentID].empty()){                        // and current calling philosopher is waiting
                    chairs[currentID].signal();                         // wake up current calling philosopher
                    server.wait();                                      // Signal block
                }
            }
            else{                                               
                printer.print(currentID,                                // Print waiting message if left or right is eating
                        Philosopher::Waiting,                            
                        currentID, 
                        right(currentID));
            }
        } 
    }
}

//----------------------------------------------------------------------
// pickup()

void Table::pickup( unsigned int id ){                                  
    currentID = id;                                                     // Tell the table current calling id   
    chairs[currentID].wait();                                           // Wait immediatelly so that control flow goes back to server main to check the capabability of eating
    assert(!states[currentID]);                                     
    states[currentID] = 1;                                              
    assert(!states[left(currentID)] && !states[right(currentID)]);
    server.signal();                                                    // Signal server to accept calls
}

//----------------------------------------------------------------------
// putdown()

void Table::putdown( unsigned int id ){
    currentID = id;                                                     // Tell table the current calling id
}
