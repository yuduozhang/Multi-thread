#include "Truck.h"
#include "MPRNG.h"
#include "Printer.h"
#include "NameServer.h"
#include "BottlingPlant.h"
#include <iostream>
using namespace std;

/***************************************************************************
	IMPORTANT NOTE: Most of the codes are derived from my 
					past assignment in cs246(fall 2010)
****************************************************************************/


extern MPRNG mprng;

Truck::Truck( Printer &prt, NameServer &nameServer, BottlingPlant &plant,
           unsigned int numVendingMachines, unsigned int maxStockPerFlavour ) : 
		   prt(prt), nameServer(nameServer), plant(plant), numVM(numVendingMachines), 
		   maxStockPF(maxStockPerFlavour) {
	for( unsigned int i = 0; i < VendingMachine::Num_Of_Flavours; i+=1 ){		// initialize the flavour array in the truck
		truckStock[i] = 0;
	}
}

Truck::~Truck(){
	prt.print( Printer::Truck, 'F' );
}

void Truck::main(){
	prt.print( Printer::Truck, 'S');	// print the starting of Truck
	for(;;){
		//obtain the location of each VendingMachine from nameServer
		VendingMachine **parr;
		parr = nameServer.getMachineList();
		//get a coffee from Tom Hortons
		yield(mprng(1,10));
		//throw away all soda still on the truck
		for(unsigned int i = 0; i<VendingMachine::Num_Of_Flavours; i+=1) truckStock[i] = 0;
		bool close = plant.getShipment(truckStock);
		if( close ) break;
		else{
			unsigned int remainS = 0;					// # of bottles remains in the shipment
			unsigned int rep = 0;						// # of bottles not replenished
			// calculate the # of Bottles picked up from Plant
			for( unsigned int i = 0; i < VendingMachine::Num_Of_Flavours; i+=1) remainS += truckStock[i];
			prt.print( Printer::Truck, 'P', remainS);	//print the truck picked up shipment
			if ( remainS == 0 ){ }						// if no bottle was picked Truck doesnt deliver anything to VendingMachine 
			else{
				// deliver until ran out VendingMachine or the shipment == 0
				for( unsigned int i = 0; i < numVM && remainS > 0; i+=1 ){
					unsigned int *Machine;
					Machine = parr[i]->inventory();								//get the machine inventory
					prt.print( Printer::Truck, 'd', parr[i]->getId(), remainS);	//print begin to deliver
					
					// begin to fill in the inventory
					for( unsigned int index = 0; index < VendingMachine::Num_Of_Flavours; index +=1){
						Machine[index] = Machine[index] + truckStock[index];
						if( Machine[index] > maxStockPF){ 
							truckStock[index] = Machine[index] - maxStockPF;
							Machine[index] = maxStockPF;
						}
						else { truckStock[index] = 0; }
						//cout << Machine[index] << endl;
						rep = rep + (maxStockPF - Machine[index]);
					}
					
					// check for total number of bottles not replenished is > 0
					if ( rep != 0 ) { 
						//print the # of unsuccessfully filled bottles in that VendingMachine
						prt.print( Printer::Truck, 'U', parr[i]->getId(), rep);
						rep = 0;
					}
					
					// update the remaining Shipment in the truck
					remainS = 0;
					for( unsigned int index = 0; index < VendingMachine::Num_Of_Flavours; index += 1){
						remainS += truckStock[index];
					}
					
					//end delivert to vending machine
					prt.print( Printer::Truck, 'D', parr[i]->getId(), remainS);
					parr[i]->restocked();
				}
			}
		}
	}
}