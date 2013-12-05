#ifndef __WATCARDOFFICE_H__
#define __WATCARDOFFICE_H__

#include <uC++.h>
#include <queue>
#include <vector>
#include "WATCard.h"
using namespace std;

_Monitor Printer;
_Monitor Bank;

_Task WATCardOffice {

    // Arguments structure
    struct Args {
        unsigned int sid;
        unsigned int amount;
        WATCard *card;
        Args(unsigned int sid, unsigned int amount, WATCard * card) : 
			sid(sid), amount(amount), card(card) {}
    };

    struct Job {				// marshalled arguments and return future
        Args args;				// call arguments (YOU DEFINE "Args")
        FWATCard result;		// return future
        Job( Args args ) : args( args ) {}
    };
	
    _Task Courier { 
        unsigned int id;
        Printer &prt;
        Bank &bank;
		WATCardOffice &cardOffice;
        void main();        
       public: 
        Courier( unsigned int id, Printer &prt, Bank &bank, WATCardOffice &cardOffice );
        ~Courier();
    };			                // communicates with bank
	
	Printer &prt;
    Bank &bank;
    unsigned int numCouriers;
    queue< Job * > jobRequest;
    Courier **couriers;
	uCondition courierBench;    // couriers wait here for new jobs
    void main();
	
    public:
    _Event Lost {};
    WATCardOffice( Printer &prt, Bank &bank, unsigned int numCouriers );
	~WATCardOffice();
    FWATCard create( unsigned int sid, unsigned int amount ); 							// called by students (clients)
    FWATCard transfer( unsigned int sid, unsigned int amount, WATCard *card );          // called by students (clients)
    Job *requestWork();
};

#endif
