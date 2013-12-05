#ifndef __Q2TALLYVOTES_H
#define __Q2TALLYVOTES_H

#include<uC++.h>
#include<uSemaphore.h>
#include<map>

//======================================================================
//  Forward Declarations
//======================================================================

_Monitor PRNG;
_Monitor Printer;
class TallyVotes;
_Task Voter;

//======================================================================
//  Global Variables 
//======================================================================

extern PRNG prng;

//======================================================================
//  TallyVotes Class
//======================================================================

#if defined( IMPLTYPE_MC )            // mutex/condition solution
// includes for this kind of vote-tallier
class TallyVotes {

    // private declarations for this kind of vote-tallier
    uOwnerLock mlk;
    uCondLock voters;
    uCondLock bargingTask;


    unsigned int signalFlag;
    unsigned int waitingCounter;


#elif defined( IMPLTYPE_SEM )        // semaphore solution
// includes for this kind of vote-tallier
class TallyVotes {

    // private declarations for this kind of vote-tallier
    uSemaphore mSmp;
    uSemaphore oneGroup;
    uSemaphore waitingForGroup;

#else
    #error unsupported voter type
#endif

    // common declarations
    std::map<unsigned,bool> votingBox;
    bool votingResult;

    unsigned group;                  // group size
    Printer &printer;


  public:                            // common interface
    TallyVotes( unsigned int group, Printer &printer );
    bool vote( unsigned int id, bool ballot );
};


//======================================================================
//  Voter Task
//======================================================================

_Task Voter {
    unsigned int id;
    TallyVotes &voteTallier;
    Printer &printer;

    void main();

    public:
    enum States { Start = 'S', 
        Vote = 'V', 
        Block = 'B', 
        Unblock = 'U', 
        Complete = 'C', 
        Finished = 'F' };
    Voter( unsigned int id, 
            TallyVotes &voteTallier, 
            Printer &printer );
};


//======================================================================
//  Printer Monitor
//======================================================================

_Monitor Printer {     

    struct VoterInfo{
        int empty;                              // 1 if empty
        unsigned int id;
        char state;                     
        bool vote;
        unsigned int numBlocked;
    };

    unsigned int numVoters;
    std::map<unsigned int,VoterInfo> buffer;    // store voters' information

    void flush();                               // for unfinished voter tasks

    void flushFinished(unsigned int id);        // for finished voter tasks

  public:
    Printer( unsigned int voters );
    ~Printer();
    void print( unsigned int id, Voter::States state );
    void print( unsigned int id, Voter::States state, bool vote );
    void print( unsigned int id, Voter::States state, unsigned int numBlocked );
};


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

#endif  //__Q2TALLYVOTES_H
