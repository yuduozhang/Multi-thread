#ifndef __Q2PHIL_H
#define __Q2PHIL_H
#include<uC++.h>
#include "AutomaticSignal.h"
#include<string>
#include<map>

//======================================================================
//  Forward Declarations
//======================================================================

_Task Philosopher;
_Monitor PRNG;
_Monitor Printer;

//======================================================================
//  Global Variables 
//======================================================================

extern PRNG prng;

//======================================================================
//  Table
//======================================================================

#if defined( TABLETYPE_INT )                    // internal scheduling monitor solution
// includes for this kind of table
_Monitor Table {
    // private declarations for this kind of table
    uCondition *chairs;

#elif defined( TABLETYPE_AUTO )                 // automatic-signal monitor solution
// includes for this kind of table
_Monitor Table {
    // private declarations for this kind of table
    AUTOMATIC_SIGNAL;

#elif defined( TABLETYPE_TASK )                 // internal/external scheduling task solution
_Task Table {
    // private declarations for this kind of table
    int currentID;

    uCondition *chairs;
    uCondition server;

    void main();
#else
    #error unsupported table
#endif
    // common declarations
    unsigned int noOfPhil;
    Printer &printer;

    int *states;                                // 0 for not eating, 1 for eating

    int left( int id ){ 
        return (id + noOfPhil -1)%noOfPhil;
    }

    int right( int id ){
        return (id + 1)%noOfPhil;
    }

  public:                                       // common interface
    Table( unsigned int noOfPhil, Printer &prt );
    ~Table();
    void pickup( unsigned int id );
    void putdown( unsigned int id );
};

//======================================================================
//  Philosopher Task
//======================================================================

_Task Philosopher {
    unsigned int id;
    unsigned int noodles;
    Table &table;
    Printer &printer;

    void main();

  public:
    enum States { Thinking = 'T', Hungry = 'H', Eating ='E', Waiting = 'W', Finished = 'F' };
    Philosopher( unsigned int id, unsigned int noodles, Table &table, Printer &prt );
};

//======================================================================
//  Printer Monitor
//======================================================================

_Monitor Printer {    
    struct PhilosopherInfo{
        int empty;                                          // 1 if empty
        unsigned int id;
        char state;                     
        unsigned int biteOrLeftFork;
        unsigned int noodlesOrRightFork;
    };

    unsigned int noOfPhil;
    std::map<unsigned int, PhilosopherInfo> buffer;         // store Philosopher's information
    

    void flush();                                           // for unfinished Philosopher tasks
    void flushFinished(unsigned int id);                    // for finished Philosopher tasks
  public:
    Printer( unsigned int noOfPhil );
    ~Printer();
    void print( unsigned int id, Philosopher::States state );
    void print( unsigned int id, Philosopher::States state, unsigned int bite, unsigned int noodles );
};


//======================================================================
//  PRNG Monitor
//======================================================================

_Monitor PRNG {
    public:
        PRNG( unsigned int seed = 1009 ) { srand( seed ); } // set seed
        void seed( unsigned int seed ) { srand( seed ); }   // set seed
        unsigned int operator()() { return rand(); }        // [0,UINT MAX]
        unsigned int operator()( unsigned int u ) 
        { return operator()() % (u + 1); }                  // [0,u]
        unsigned int operator()( unsigned int l, unsigned int u ) 
        { return operator()( u - l ) + l; }                 // [l,u]
};
#endif  //__Q2PHIL_H
