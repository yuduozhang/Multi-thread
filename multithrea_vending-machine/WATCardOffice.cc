#include "WATCardOffice.h"
#include "Printer.h"
#include "Bank.h"
#include "MPRNG.h"
#include <iostream>
using namespace std;

extern MPRNG mprng;

//----------------------------------------------------------------------
// Courier constructor
WATCardOffice::Courier::Courier( unsigned int id, Printer &prt, Bank &bank, WATCardOffice &cardOffice) : 
								id(id), prt(prt), bank(bank), cardOffice(cardOffice) {}

//----------------------------------------------------------------------
// Courier destructor
WATCardOffice::Courier::~Courier(){
}

//----------------------------------------------------------------------
// Courier main()
void WATCardOffice::Courier::main(){
    prt.print(Printer::Courier, id, 'S');
    for ( ;; ){
		_Accept (~Courier){
			break;
		} else {
			Job * job = cardOffice.requestWork();
			if (job == NULL) break; // if no more work to do, break and terminate

            // if card is null, this is a create request,
            // then create a new card
			if (job->args.card == NULL) job->args.card = new WATCard();
			
            // start transfering
            prt.print(Printer::Courier, id, 't', job->args.sid, job->args.amount);

			bank.withdraw( job->args.sid, job->args.amount ); 
			job->args.card->deposit(job->args.amount); 

            // finish transfering
            prt.print(Printer::Courier, id, 'T', job->args.sid, job->args.amount);
			
			// 1 in 6 chance to lose the watcard
            if ( mprng(5) == 0 ) {
                job->result.exception( new Lost );
                delete job->args.card;
            }

			else job->result.delivery( job->args.card );

            // job done, free the memory it allocates
			delete job;
		}
	}
	
	prt.print(Printer::Courier, id, 'F');
}


//----------------------------------------------------------------------
// WATCardOffice constructor
WATCardOffice::WATCardOffice( Printer &prt, Bank &bank, unsigned int numCouriers ) :
    prt(prt), bank(bank), numCouriers(numCouriers) {
        // create courier tasks
        couriers = new Courier *[numCouriers];
        for (unsigned int i = 0; i < numCouriers; i++){
            couriers[i] = new Courier(i, prt, bank, *this);
        }
}

//----------------------------------------------------------------------
// WATCardOffice destructor 
WATCardOffice::~WATCardOffice(){
}

//----------------------------------------------------------------------
// WATCardOffice main()
void WATCardOffice::main(){
    prt.print(Printer::WATCardOffice, 'S');

    for ( ;; ){
		_Accept (~WATCardOffice){
            break;
        } or _Accept( create, transfer ){
        } or _Accept( requestWork ) {
        }
    }
	
    // wake all sleeping couriers up
	while(!courierBench.empty()){
		courierBench.signalBlock();
	}
	
    // free memory allocated to couriers
    for (unsigned int i = 0; i < numCouriers; i++){
        delete couriers[i];
    }
    delete [] couriers;
	prt.print(Printer::WATCardOffice, 'F'); 
}


//----------------------------------------------------------------------
// request to create a new card
FWATCard WATCardOffice::create( unsigned int sid, unsigned int amount ){ 

    Args args(sid, amount, NULL);
    Job * job = new Job(args);
    jobRequest.push(job);

    prt.print(Printer::WATCardOffice, 'C', sid, amount);
	
    // wake up sleeping couriers to work
	if (!courierBench.empty()) courierBench.signal();
    return job->result;     // return future watcard object
}

//----------------------------------------------------------------------
// request to transfer
FWATCard WATCardOffice::transfer( unsigned int sid, unsigned int amount, WATCard *card ){

    Args args(sid, amount, card);
    Job * job = new Job(args);
    jobRequest.push(job);

    prt.print(Printer::WATCardOffice, 'T', sid, amount);

    // wake up sleeping couriers to work
	if (!courierBench.empty()) courierBench.signal();
    return job->result;     // return future watcard object 
}

WATCardOffice::Job *WATCardOffice::requestWork(){
	if (jobRequest.empty()) courierBench.wait();
    // after waking up, if still no work to do, then the system is closing down, hence return
	if (jobRequest.empty()) return NULL;    
	assert( !jobRequest.empty() );
	
    Job * job = jobRequest.front();
    jobRequest.pop();
	prt.print(Printer::WATCardOffice, 'W'); 
    return job;
}
