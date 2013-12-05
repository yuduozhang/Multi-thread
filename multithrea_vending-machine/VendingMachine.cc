#include "VendingMachine.h"
#include "Printer.h"
#include "NameServer.h"
#include "WATCard.h"
#include <iostream>
using namespace std;

/***************************************************************************
	IMPORTANT NOTE: Most of the codes are derived from my 
					past assignment in cs246(fall 2010)
****************************************************************************/


//VendingMachine main
void VendingMachine::main(){
	for(;;){
		_Accept(~VendingMachine){
			break;
		} or _Accept(inventory, restocked){
		} or _When(!restoring) _Accept(buy){
		}
	}
}

//VendingMachine constructor
VendingMachine::VendingMachine( Printer &prt, NameServer &nameServer,
		unsigned int id, unsigned int sodaCost, unsigned int maxStockPerFlavour ) : 
		prt(prt), nameServer(nameServer), id(id), price(sodaCost),
		maxSPF(maxStockPerFlavour){
	restoring = false;
	prt.print( Printer::Vending, id, 'S', price );		//print the Starting of VendingMachine
	nameServer.VMregister(this);						//register the VendingMachine itself
	for( int i = 0; i < VendingMachine::Num_Of_Flavours; i+=1 ){ //initialize the array storing the flavours
		stock[i] = 0;
	}
}

//VendingMachine destructor
VendingMachine::~VendingMachine(){
	prt.print( Printer::Vending, id, 'F');
}

VendingMachine::Status VendingMachine::buy( VendingMachine::Flavours flavour, WATCard &card ){
	if(stock[flavour] == 0) return VendingMachine::STOCK;
	else if(card.getBalance() < price) return VendingMachine::FUNDS;
	else {
		card.withdraw(price);
		stock[flavour]-=1;
		prt.print( Printer::Vending, id, 'B', (unsigned int)flavour, (unsigned int)stock[flavour]);
		return VendingMachine::BUY;
	}
}

//return the array storing the flavours
unsigned int *VendingMachine::inventory(){
	prt.print( Printer::Vending, id, 'r' );
	restoring = true;
	return stock;
}

//print if vendingMachine finish reloading
void VendingMachine::restocked(){
	prt.print( Printer::Vending, id, 'R' );
	restoring = false;
}

//return vendingMachine one Bottle cost
unsigned int VendingMachine::cost(){
	return price;
}

//return vendingMachine ID
unsigned int VendingMachine::getId(){
	return id;
}