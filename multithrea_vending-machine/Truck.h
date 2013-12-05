#ifndef __TRUCK_H__
#define __TRUCK_H__

#include <uC++.h>
#include "VendingMachine.h"

_Monitor Printer;
_Task NameServer;
_Task BottlingPlant;
//_Task VendingMachine;

_Task Truck {
	Printer &prt;
	NameServer &nameServer;
	BottlingPlant &plant;
	unsigned int numVM;
	unsigned int maxStockPF;
    unsigned int truckStock[VendingMachine::Num_Of_Flavours];
	void main();
  public:
    Truck( Printer &prt, NameServer &nameServer, BottlingPlant &plant,
           unsigned int numVendingMachines, unsigned int maxStockPerFlavour );
	~Truck();
};

#endif
