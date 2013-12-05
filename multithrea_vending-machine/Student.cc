#include "Student.h"
#include "Printer.h"
#include "NameServer.h"
#include "MPRNG.h"
#include "WATCard.h"
#include "WATCardOffice.h"
#include "VendingMachine.h"
#include <iostream>
using namespace std;

extern MPRNG mprng;


//----------------------------------------------------------------------
// Student Constructor
Student::Student( Printer &prt, 
        NameServer &nameServer, 
        WATCardOffice &cardOffice, 
        unsigned int id,
        unsigned int maxPurchases ) : 
    prt(prt), 
    nameServer(nameServer), 
    cardOffice(cardOffice),
    sid(id),
    maxPurchases(maxPurchases),
    initialBalance(5){}
	
//----------------------------------------------------------------------
// Student Destructor
Student::~Student(){
	prt.print( Printer::Student, sid, 'F' );
}


//----------------------------------------------------------------------
// main()
void Student::main(){
    unsigned int numBottle = mprng(1, maxPurchases);
    unsigned int flavour = mprng(0,3);

	prt.print( Printer::Student, sid, 'S', flavour, numBottle );
	
    // create a watcard with $5 balance
    FWATCard card = cardOffice.create(sid, initialBalance); 
    VendingMachine *VM = nameServer.getMachine(sid);

	prt.print( Printer::Student, sid, 'V', VM->getId() );
	
	for(unsigned int i=0; i<numBottle; i+=1){
		yield(mprng(1,10));
		while(1){

            // try to see if card is ready or exception is raised
			for(;;){
				try {
					card();
					break;  // if WATCard has real money, break
				} catch( WATCardOffice::Lost e ){
					prt.print( Printer::Student, sid, 'L' );
					card = cardOffice.create(sid, initialBalance);
				}
			}

            // after card is ready, attempt to buy a soda
			VendingMachine::Status tryToBuy = 
                VM->buy((VendingMachine::Flavours)flavour, *card());
			if(tryToBuy == VendingMachine::BUY){
				//bought a bottle successfully
                
				prt.print( Printer::Student, sid, 'B', card()->getBalance() );
				break;
			}
			else if(tryToBuy == VendingMachine::STOCK){				
                // out of stock, choose another vending machine
                
				VM = nameServer.getMachine(sid);
				prt.print( Printer::Student, sid, 'V', VM->getId() );
			}
			else{// tryToBuy == VendingMachine::FUND
				card = cardOffice.transfer(sid, VM->cost() + initialBalance, card());
			}
		}
	}
	delete card();
}

