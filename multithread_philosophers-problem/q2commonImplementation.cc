#include "q2table.h"
#include<iostream>
#include<stdlib.h>

using namespace std;
//======================================================================
//  Philosopher Task
//======================================================================

//----------------------------------------------------------------------
// Constructor
Philosopher::Philosopher( unsigned int id, unsigned int noodles, Table &table, Printer &prt ) : 
    id(id), noodles(noodles), table(table), printer(prt){}

//----------------------------------------------------------------------
// main()
void Philosopher::main(){
    for ( ; ; ){
        printer.print(id, Philosopher::Hungry);

        unsigned int yieldNum = prng(0,4);
        yield(yieldNum);

        table.pickup(id);

        unsigned int bite = prng(1,5);
        if (bite > noodles) bite = noodles;
        noodles = noodles - bite;

        printer.print(id, Philosopher::Eating, bite, noodles);

        yieldNum = prng(0,4);
        yield(yieldNum);

        table.putdown(id);

        if (noodles == 0) break;

        printer.print(id, Philosopher::Thinking);

        yieldNum = prng(0,19);
        yield(yieldNum); 
    }
    printer.print(id, Philosopher::Finished);
}


//======================================================================
// Printer Monitor
//======================================================================

//----------------------------------------------------------------------
// Constructor
//
Printer::Printer( unsigned int noOfPhil){
    Printer::noOfPhil = noOfPhil;

    for  (unsigned int i = 0; i < noOfPhil; i++){
        PhilosopherInfo temp;
        temp.id = i;
        temp.empty = 1;
        buffer[i] = temp;
    }  

    for (unsigned int i = 0; i < noOfPhil; i++){
        PhilosopherInfo temp = buffer[i];
        cout << "Phil" << temp.id << "\t";
    }

    cout << endl;

    for (unsigned int i = 0; i < noOfPhil; i++){
        cout << "******\t";
    }

    cout << endl;
}


//----------------------------------------------------------------------
// Destructor
//
Printer::~Printer(){
    cout << "***********************" << endl;
    cout << "Philosophers terminated" << endl;
}

//----------------------------------------------------------------------
// Flush for unfinished tasks
//
void Printer::flush(){
    for (unsigned int i = 0; i < noOfPhil; i++){
        PhilosopherInfo temp = buffer[i];

        // If empty PhilosopherInfo, output empty column
        if (temp.empty){
            cout << "\t";
        }

        // If not empty
        else if (temp.state == Philosopher::Thinking
                ||
                temp.state == Philosopher::Hungry){
            cout << temp.state << "\t";
        } 

        else if (temp.state == Philosopher::Eating
                ||
                temp.state == Philosopher::Waiting){
            cout << temp.state << temp.biteOrLeftFork << "," << temp.noodlesOrRightFork << "\t";
        }

        temp.empty = 1;
        buffer[i] = temp;
    }

    cout << endl;
}

//----------------------------------------------------------------------
// Flush for finished tasks
//
void Printer::flushFinished(unsigned int id){
    for (unsigned int i = 0; i < noOfPhil; i++){
        PhilosopherInfo temp = buffer[i];

        if (i != id){
            cout << "...\t";
        }
        else {
            assert(temp.state == Philosopher::Finished);
            cout << temp.state << "\t";
        }
        temp.empty = 1;
        buffer[i] = temp;
    }    
    cout << endl;
}

//----------------------------------------------------------------------
// Print for ordinary states
//
void Printer::print( unsigned int id, Philosopher::States state ){
    PhilosopherInfo stateOnly = buffer[id];
    if(!stateOnly.empty){
        flush();
    }
    else if (state == Philosopher::Finished){
        flush();
    }

    stateOnly.state = state;
    stateOnly.empty = 0;
    buffer[id] = stateOnly;

    if (state == Philosopher::Finished){
        flushFinished(id);
    }
}

//----------------------------------------------------------------------
// Print for eating states
//
void Printer::print( 
        unsigned int id, 
        Philosopher::States state, 
        unsigned int bite, 
        unsigned int noodles ){
    PhilosopherInfo temp = buffer[id];
    if(!temp.empty){
        flush();
    }
    temp.state = state;
    temp.biteOrLeftFork = bite;
    temp.noodlesOrRightFork = noodles;    
    temp.empty = 0;
    buffer[id] = temp;
}
