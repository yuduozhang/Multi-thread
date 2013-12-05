#include "Parent.h"
#include "Printer.h"
#include "Bank.h"
#include "MPRNG.h"
#include <iostream>
using namespace std;

extern MPRNG mprng;

//----------------------------------------------------------------------
// Parent Constructor

Parent::Parent( 
        Printer &prt, 
        Bank &bank, 
        unsigned int numStudents,
        unsigned int parentalDelay ) : 
    prt(prt), bank(bank), numStudents(numStudents), parentalDelay(parentalDelay){}

//----------------------------------------------------------------------
// Parent destructor 
Parent::~Parent(){
	prt.print(Printer::Parent, 'F');
}

//----------------------------------------------------------------------
// main()
void Parent::main(){
    prt.print(Printer::Parent, 'S');

    for (;;){
        // wait for the call of destructor 
        _Accept( ~Parent ){
            break;
        } else {
            yield(parentalDelay);

            int giftAmount = mprng(1,3);
            int studentID = mprng(0,numStudents-1);

            prt.print(Printer::Parent, 'D', studentID, giftAmount);
            bank.deposit( studentID, giftAmount );
        }
    } // for
}
