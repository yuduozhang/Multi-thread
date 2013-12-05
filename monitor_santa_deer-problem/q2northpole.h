#ifndef __Q2NORTHPOLE_H
#define __Q2NORTHPOLE_H
#include<uC++.h>
#include<string>
#include<map>

//======================================================================
//  Forward Declarations
//======================================================================

_Monitor PRNG;
_Monitor Printer;
_Task Santa;
_Task Elf;
_Task Reindeer;


//======================================================================
//  Global Variables 
//======================================================================

extern PRNG prng;

//======================================================================
//  Monitor Workshop
//======================================================================

_Monitor Workshop {
    // private members go here
    uCondition santaSleep,                        // santa sleeps here
               reindeerWait,                      // reindeers wait for santa here
               elvesWait,                         // elves wait for santa here
               office,                            // santa and elves block here until meeting is done 
               sleigh;                            // santa and reindeers block here until delivery is done

    Printer &printer;
    unsigned int numElves, boundReindeer, numDeliveries;

    int waitingReindeer,                          // count number of waiting reindeers
        waitingElves,                             // count number of waiting elves
        numServeReindeer,                         // count number of service reindeer get while there are groups elves waiting
        meetingNum,                               // count meeting number of elves to see whether enough elves have been waken up to form a consultation or not
        officeNum,                                // count number of tasks in the office (santa and elves) to see whether the meeting has done or not
        sleighNum;                                // count number of tasks on the sleigh (santa and reindeer) to see whether the delivery has done or not 

  public:
    enum Status { Consulting, Delivery, Done };
    Workshop( Printer &prt, unsigned int E, unsigned int N, unsigned int D ); // printer, elves, bound, deliveries

    Status sleep();                               // santa calls to nap; when Santa wakes status of next action
    void deliver( unsigned int id );              // reindeer call to deliver toys
    bool consult( unsigned int id );              // elves call to consult Santa,
                                                  //   true => consultation successful, false => consultation failed
    void doneConsulting( unsigned int id );       // block Santa/elves until meeting over
    void doneDelivering( unsigned int id );       // block Santa/reindeer until all toys are delivered
    void termination( unsigned int id );          // elves call to indicate termination
};


//======================================================================
//  Santa Task
//======================================================================

_Task Santa {
    // private members go here
    unsigned int id;
    Workshop &workshop;
    Printer &printer;

    void main();

  public:
    Santa( Workshop &wrk, Printer &prt );
};


//======================================================================
//  Elf Task
//======================================================================

_Task Elf {
    // private members go here
    unsigned int id;
    Workshop &workshop;
    Printer &printer;
    unsigned int numC;                            // number of consultation

    void main();

  public:
    enum { CONSULTING_GROUP_SIZE = 3 };           // number of elves for a consultation with Santa
    Elf( unsigned int id, Workshop &wrk, Printer &prt, unsigned int numConsultations );
};


//======================================================================
//  Reindeer Task
//======================================================================

_Task Reindeer {
    // private members go here
    unsigned int id;
    Workshop &workshop;
    Printer &printer;
    unsigned int numD;

    void main();

  public:
    enum { MAX_NUM_REINDEER = 5 };                // number of reindeer in system for delivering toys
    Reindeer( unsigned int id, Workshop &wrk, Printer &prt, unsigned int numDeliveries );
};


//======================================================================
//  Printer Monitor
//======================================================================

_Monitor Printer {                              // choose monitor or cormonitor

    struct TaskInfo{
        int empty;                              // 1 if empty
        std::string taskType;
        unsigned int id;
        char state;                     
        int numBlocked;
    };

    unsigned int numElves;
    std::map<unsigned int,TaskInfo> buffer;     // store voters' information

    void flush();                               // for unfinished voter tasks

    void flushFinished(unsigned int id);        // for finished voter tasks

  public:
    enum States { Starting = 'S', Blocked = 'B', Unblocked = 'U', Finished = 'F', // general
          Napping = 'N', Awake = 'A',             // Santa
          Working = 'W', NeedHelp = 'H',          // elf
          OnVacation = 'V', CheckingIn = 'I',     // reindeer
          DeliveringToys = 'D', DoneDelivering = 'd', // Santa, reindeer
          Consulting = 'C', DoneConsulting = 'c', // Santa, elves
          ConsultingFailed = 'X',                 // elf
    };

    Printer( const unsigned int MAX_NUM_ELVES );
    ~Printer();
    void print( unsigned int id, States state );
    void print( unsigned int id, States state, unsigned int numBlocked );
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

#endif  //__Q2NORTHPOLE_H
