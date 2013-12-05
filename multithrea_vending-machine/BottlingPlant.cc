#include "BottlingPlant.h"
#include "MPRNG.h"
#include "Printer.h"
#include "NameServer.h"
//#include "VendingMachine.h"
#include "Truck.h"
#include <iostream>
using namespace std;

/***************************************************************************
	IMPORTANT NOTE: Most of the codes are derived from my 
					past assignment in cs246(fall 2010)
****************************************************************************/


extern MPRNG mprng;

void BottlingPlant::productionRun(){
	for( int i = 0; i< VendingMachine::Num_Of_Flavours; i+=1){		// run production
		prod[i] = mprng(0,maxShippedPF);
		prodSum += prod[i];
	}
	yield(timeBS);
	prt.print( Printer::BottlingPlant, 'G', prodSum);			// print the # of bottles hv produced
}

void BottlingPlant::main(){
	productionRun();
	car = new Truck( prt, nameServer, *this, numVM, maxStockPF );	//creat a truck
	for(;;){
		_Accept(~BottlingPlant){
			break;
		} or _Accept(getShipment){
			//run a production if only if truck pick up the shipment
			productionRun();
		}
	}
	closing = true;
	_Accept(getShipment){}
	delete car;
}

BottlingPlant::BottlingPlant( Printer &prt, NameServer &nameServer, unsigned int numVendingMachines,
                 unsigned int maxShippedPerFlavour, unsigned int maxStockPerFlavour,
                 unsigned int timeBetweenShipments ) : 
				 prt(prt), nameServer(nameServer), numVM(numVendingMachines), maxShippedPF(maxShippedPerFlavour),
				 maxStockPF(maxStockPerFlavour), timeBS(timeBetweenShipments){
	prodSum = 0;
	closing = false;
	prt.print( Printer::BottlingPlant, 'S');	//print starting of BottlingPlant
}

bool BottlingPlant::getShipment( unsigned int cargo[] ){
	if(closing) return closing;
	else{
		for(int i=0; i < VendingMachine::Num_Of_Flavours; i+=1){
			cargo[i] = prod[i];
			prod[i] = 0;
		}
		prt.print( Printer::BottlingPlant, 'P');		// print the production had picked
		prodSum = 0;
		return closing;
	}
}

BottlingPlant::~BottlingPlant(){
	prt.print( Printer::BottlingPlant, 'F');
}